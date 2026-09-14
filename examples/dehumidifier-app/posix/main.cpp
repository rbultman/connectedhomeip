/*
 *    Copyright (c) 2026 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "AppOptions.h"
#include "DehumidifierManager.h"

#include <LinuxCommissionableDataProvider.h>
#include <TracingCommandLineArgument.h>
#include <app/DefaultSafeAttributePersistenceProvider.h>
#include <app/DeviceLoadStatusProvider.h>
#include <app/InteractionModelEngine.h>
#include <app/SafeAttributePersistenceProvider.h>
#include <app/TestEventTriggerDelegate.h>
#include <app/clusters/humidistat-server/HumidistatTestEventTriggerHandler.h>
#if CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
#include <app/clusters/terms-and-conditions-server/TermsAndConditionsManager.h>
#endif
#include <app/persistence/DefaultAttributePersistenceProvider.h>
#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <credentials/GroupDataProviderImpl.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/DefaultTimerDelegate.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <platform/DiagnosticDataProvider.h>
#include <platform/PlatformManager.h>
#include <providers/AllDevicesExampleDACProvider.h>
#include <providers/AllDevicesExampleDeviceInfoProviderImpl.h>
#include <providers/AllDevicesExampleDeviceInstanceInfoProviderImpl.h>
#include <setup_payload/OnboardingCodesUtil.h>
#include <system/SystemLayer.h>

#include <csignal>

using namespace chip;
using namespace chip::app;
using namespace chip::Platform;
using namespace chip::DeviceLayer;
using namespace chip::ArgParser;

namespace {

Credentials::GroupDataProviderImpl gGroupDataProvider;
DefaultSafeAttributePersistenceProvider gSafeAttributePersistenceProvider;
DefaultTimerDelegate gTimerDelegate;
LinuxCommissionableDataProvider gCommissionableDataProvider;

void StopSignalHandler(int /* signal */)
{
    Server::GetInstance().GenerateShutDownEvent();
    SuccessOrDie(SystemLayer().ScheduleLambda([]() { VerifyOrDie(PlatformMgr().StopEventLoopTask() == CHIP_NO_ERROR); }));
}

void SetTerminateHandler(void (*handler)(int))
{
#if defined(__APPLE__) && CHIP_SYSTEM_CONFIG_USE_DISPATCH
    auto & platformMgr = chip::DeviceLayer::PlatformMgrImpl();
    platformMgr.RegisterSignalHandler(SIGINT, ^{
        platformMgr.UnregisterAllSignalHandlers();
        handler(SIGINT);
    });

    platformMgr.RegisterSignalHandler(SIGTERM, ^{
        platformMgr.UnregisterAllSignalHandlers();
        handler(SIGTERM);
    });
#else
    struct sigaction sa = {};
    sa.sa_handler       = handler;
    sa.sa_flags         = SA_RESETHAND;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
#endif
}

CHIP_ERROR InitBle(uint32_t bleController)
{
#if CONFIG_NETWORK_LAYER_BLE
#if defined(__APPLE__)
    ReturnErrorOnFailure(DeviceLayer::Internal::BLEMgrImpl().ConfigureBle(bleController, false));
#else
    ReturnErrorOnFailure(DeviceLayer::ConnectivityMgr().SetBLEDeviceName(nullptr));
    ReturnErrorOnFailure(DeviceLayer::Internal::BLEMgrImpl().ConfigureBle(bleController, false));
    ReturnErrorOnFailure(DeviceLayer::ConnectivityMgr().SetBLEAdvertisingEnabled(true));
#endif
#endif
    return CHIP_NO_ERROR;
}

void EventHandler(const DeviceLayer::ChipDeviceEvent * event, intptr_t arg)
{
    (void) arg;
    if (event->Type == DeviceLayer::DeviceEventType::kCHIPoBLEConnectionEstablished)
    {
        ChipLogProgress(DeviceLayer, "Receive kCHIPoBLEConnectionEstablished");
    }
    else if (event->Type == chip::DeviceLayer::DeviceEventType::kInternetConnectivityChange)
    {
        DnssdServer::Instance().StartServer();
    }
}

CHIP_ERROR InitCommissionableDataProvider(LinuxCommissionableDataProvider & provider, const AppOptions::AppConfig & config)
{
    auto discriminator = config.discriminator.value_or(static_cast<uint16_t>(CHIP_DEVICE_CONFIG_USE_TEST_SETUP_DISCRIMINATOR));

    const auto setupPasscode             = MakeOptional(static_cast<uint32_t>(CHIP_DEVICE_CONFIG_USE_TEST_SETUP_PIN_CODE));
    const uint32_t spake2pIterationCount = Crypto::kSpake2p_Min_PBKDF_Iterations;

    Optional<std::vector<uint8_t>> serializedSpake2pVerifier = NullOptional;
    Optional<std::vector<uint8_t>> spake2pSalt               = NullOptional;

    return provider.Init(serializedSpake2pVerifier, spake2pSalt, spake2pIterationCount, setupPasscode, discriminator);
}

void RunApplication()
{
    static chip::CommonCaseDeviceServerInitParams initParams;
    SuccessOrDie(initParams.InitializeStaticResourcesBeforeServerInit());

    static SimpleTestEventTriggerDelegate sTestEventTriggerDelegate;
    SuccessOrDie(sTestEventTriggerDelegate.Init(ByteSpan(AppOptions::GetConfig().testEventTriggerEnableKey)));
    static HumidistatTestEventTriggerHandler sHumidistatTestEventTriggerHandler;
    SuccessOrDie(sTestEventTriggerDelegate.AddHandler(&sHumidistatTestEventTriggerHandler));
    initParams.testEventTriggerDelegate = &sTestEventTriggerDelegate;

    gGroupDataProvider.SetStorageDelegate(initParams.persistentStorageDelegate);
    gGroupDataProvider.SetSessionKeystore(initParams.sessionKeystore);
    SuccessOrDie(gGroupDataProvider.Init());
    Credentials::SetGroupDataProvider(&gGroupDataProvider);

    DeviceLayer::DeviceInstanceInfoProvider * provider = DeviceLayer::GetDeviceInstanceInfoProvider();
    if (provider == nullptr)
    {
        ChipLogError(AppServer, "Failed to get the DeviceInstanceInfoProvider.");
        chipDie();
    }

    SuccessOrDie(gSafeAttributePersistenceProvider.Init(initParams.persistentStorageDelegate));
    SetSafeAttributePersistenceProvider(&gSafeAttributePersistenceProvider);

    static DeviceLayer::AllDevicesExampleDACProvider sDacProvider;
    SuccessOrDie(sDacProvider.Init(AppOptions::GetConfig().dacProvider));
    SetDeviceAttestationCredentialsProvider(&sDacProvider);

    static DehumidifierManager sDehumidifierManager({
        .storageDelegate                     = *initParams.persistentStorageDelegate,
        .commissioningWindowManager          = Server::GetInstance().GetCommissioningWindowManager(),
        .configurationManager                = DeviceLayer::ConfigurationMgr(),
        .deviceControlServer                 = DeviceLayer::DeviceControlServer::DeviceControlSvr(),
        .fabricTable                         = Server::GetInstance().GetFabricTable(),
        .accessControl                       = Server::GetInstance().GetAccessControl(),
        .persistentStorage                   = Server::GetInstance().GetPersistentStorage(),
        .failSafeContext                     = Server::GetInstance().GetFailSafeContext(),
        .deviceInstanceInfoProvider          = *provider,
        .platformManager                     = DeviceLayer::PlatformMgr(),
        .groupDataProvider                   = gGroupDataProvider,
        .sessionManager                      = Server::GetInstance().GetSecureSessionManager(),
        .dnssdServer                         = DnssdServer::Instance(),
        .deviceLoadStatusProvider            = *InteractionModelEngine::GetInstance(),
        .diagnosticDataProvider              = DeviceLayer::GetDiagnosticDataProvider(),
        .testEventTriggerDelegate            = initParams.testEventTriggerDelegate,
        .dacProvider                         = *Credentials::GetDeviceAttestationCredentialsProvider(),
        .eventManagement                     = EventManagement::GetInstance(),
        .timerDelegate                       = gTimerDelegate,
        .minGuaranteedSubscriptionsPerFabric = InteractionModelEngine::GetInstance()->GetMinGuaranteedSubscriptionsPerFabric(),
#if CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
        .termsAndConditionsProvider = TermsAndConditionsManager::GetInstance(),
#endif // CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
    });

    SuccessOrDie(sDehumidifierManager.Startup());

    initParams.dataModelProvider             = &sDehumidifierManager.DataModelProvider();
    initParams.groupDataProvider             = &gGroupDataProvider;
    initParams.operationalServicePort        = AppOptions::GetConfig().port.value_or(CHIP_PORT);
    initParams.userDirectedCommissioningPort = CHIP_UDC_PORT;

    if (AppOptions::GetConfig().interfaceId.has_value())
    {
        initParams.interfaceId =
            Inet::InterfaceId(static_cast<Inet::InterfaceId::PlatformType>(AppOptions::GetConfig().interfaceId.value()));
    }
    else
    {
        initParams.interfaceId = Inet::InterfaceId::Null();
    }

#if defined(ENABLE_TRACING) && ENABLE_TRACING
    chip::CommandLineApp::TracingSetup tracingSetup;
    for (const auto & traceDestination : AppOptions::GetConfig().traceTo)
    {
        tracingSetup.EnableTracingFor(traceDestination.c_str());
    }
#endif

    CHIP_ERROR err = Server::GetInstance().Init(initParams);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Server init failed: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    ConfigurationMgr().LogDeviceConfig();

    chip::PayloadContents payload;
    payload.version = 0;
    payload.rendezvousInformation.SetValue(RendezvousInformationFlag::kBLE);

    if (GetCommissionableDataProvider()->GetSetupPasscode(payload.setUpPINCode) != CHIP_NO_ERROR)
    {
        payload.setUpPINCode = CHIP_DEVICE_CONFIG_USE_TEST_SETUP_PIN_CODE;
    }

    uint16_t discriminator = 0;
    SuccessOrDie(GetCommissionableDataProvider()->GetSetupDiscriminator(discriminator));
    payload.discriminator.SetLongValue(discriminator);

    SuccessOrDie(chip::DeviceLayer::GetDeviceInstanceInfoProvider()->GetVendorId(payload.vendorID));
    SuccessOrDie(chip::DeviceLayer::GetDeviceInstanceInfoProvider()->GetProductId(payload.productID));
    PrintOnboardingCodes(payload);

    SetTerminateHandler(StopSignalHandler);

    ChipLogProgress(DeviceLayer, "===== APP STATUS: Starting event loop =====");

    DeviceLayer::PlatformMgr().RunEventLoop();

    sDehumidifierManager.Shutdown();
    Server::GetInstance().Shutdown();
    DeviceLayer::PlatformMgr().Shutdown();

#if defined(ENABLE_TRACING) && ENABLE_TRACING
    tracingSetup.StopTracing();
#endif
}

CHIP_ERROR Initialize(int argc, char * argv[])
{
    ChipLogProgress(AppServer, "Initializing...");
    ReturnErrorOnFailure(Platform::MemoryInit());

    static HelpOptions sHelpOptions(argv[0], "Usage: chip-dehumidifier-app [options]", "1.0");
    static OptionSet * sAppOptionSets[] = { AppOptions::GetOptions(), &sHelpOptions, nullptr };
    if (!ArgParser::ParseArgs(argv[0], argc, argv, sAppOptionSets))
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    ReturnErrorOnFailure(AppOptions::ValidateConfig());

    const char * kvsPath = AppOptions::GetConfig().kvsPath.empty() ? CHIP_CONFIG_KVS_PATH : AppOptions::GetConfig().kvsPath.c_str();
    ReturnErrorOnFailure(DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init(kvsPath));
    ReturnErrorOnFailure(DeviceLayer::PlatformMgr().InitChipStack());

    ReturnErrorOnFailure(InitCommissionableDataProvider(gCommissionableDataProvider, AppOptions::GetConfig()));
    DeviceLayer::SetCommissionableDataProvider(&gCommissionableDataProvider);

    static AllDevicesExampleDeviceInfoProviderImpl sExampleDeviceInfoProvider;
    DeviceLayer::SetDeviceInfoProvider(&sExampleDeviceInfoProvider);

    const auto & config = AppOptions::GetConfig();
    auto vendorId       = config.vendorId.has_value() ? config.vendorId : std::nullopt;
    auto productId      = config.productId.has_value() ? config.productId : std::nullopt;
    static AllDevicesExampleDeviceInstanceInfoProviderImpl sAppDeviceInstanceInfoProvider(
        DeviceLayer::GetDeviceInstanceInfoProvider(), vendorId, productId);
    DeviceLayer::SetDeviceInstanceInfoProvider(&sAppDeviceInstanceInfoProvider);

    ConfigurationMgr().LogDeviceConfig();

    ReturnErrorOnFailure(DeviceLayer::PlatformMgrImpl().AddEventHandler(EventHandler, 0));

    ReturnErrorOnFailure(InitBle(AppOptions::GetConfig().bleController));

    return CHIP_NO_ERROR;
}

} // namespace

int main(int argc, char * argv[])
{
    ChipLogProgress(AppServer, "Initializing");

    if (CHIP_ERROR err = Initialize(argc, argv); err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Initialize() failed: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    ChipLogProgress(AppServer, "Hello from dehumidifier-app!");
    RunApplication();

    return 0;
}
