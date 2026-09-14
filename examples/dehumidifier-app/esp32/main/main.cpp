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

#include "DehumidifierManager.h"

#include <app/DefaultSafeAttributePersistenceProvider.h>
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
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <platform/DefaultTimerDelegate.h>
#include <platform/DiagnosticDataProvider.h>
#include <platform/ESP32/ESP32Utils.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <platform/PlatformManager.h>
#include <setup_payload/OnboardingCodesUtil.h>

#if CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER
#include <platform/ESP32/ESP32FactoryDataProvider.h>
#endif

#if CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER
#include <platform/ESP32/ESP32DeviceInfoProvider.h>
#else
#include <DeviceInfoProviderImpl.h>
#endif

using namespace chip;
using namespace chip::app;
using namespace chip::DeviceLayer;
using namespace chip::Credentials;

static const char TAG[] = "dehumidifier-app";

namespace {

DeviceLayer::NetworkCommissioning::ESPWiFiDriver & sWiFiDriver = DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance();

Credentials::GroupDataProviderImpl gGroupDataProvider;
DefaultSafeAttributePersistenceProvider gSafeAttributePersistenceProvider;
DefaultTimerDelegate gTimerDelegate;
SimpleTestEventTriggerDelegate sTestEventTriggerDelegate;
HumidistatTestEventTriggerHandler sHumidistatTestEventTriggerHandler;

#if CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER
DeviceLayer::ESP32FactoryDataProvider sFactoryDataProvider;
#endif

#if CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER
DeviceLayer::ESP32DeviceInfoProvider gExampleDeviceInfoProvider;
#else
DeviceLayer::DeviceInfoProviderImpl gExampleDeviceInfoProvider;
#endif

static void DeviceEventHandler(const ChipDeviceEvent * event, intptr_t arg)
{
    if (event->Type == DeviceEventType::kInternetConnectivityChange)
    {
        if (event->InternetConnectivityChange.IPv4 == kConnectivity_Established ||
            event->InternetConnectivityChange.IPv6 == kConnectivity_Established)
        {
            ESP_LOGI(TAG, "Internet connectivity established -> restarting DNS-SD");
            chip::app::DnssdServer::Instance().StartServer();
        }
    }
}

void InitServer(intptr_t context)
{
    static chip::CommonCaseDeviceServerInitParams initParams;
    SuccessOrDie(initParams.InitializeStaticResourcesBeforeServerInit());

    static uint8_t sTestEventTriggerEnableKey[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    SuccessOrDie(sTestEventTriggerDelegate.Init(ByteSpan(sTestEventTriggerEnableKey)));
    SuccessOrDie(sTestEventTriggerDelegate.AddHandler(&sHumidistatTestEventTriggerHandler));
    initParams.testEventTriggerDelegate = &sTestEventTriggerDelegate;

    gGroupDataProvider.SetStorageDelegate(initParams.persistentStorageDelegate);
    gGroupDataProvider.SetSessionKeystore(initParams.sessionKeystore);
    SuccessOrDie(gGroupDataProvider.Init());
    Credentials::SetGroupDataProvider(&gGroupDataProvider);

    SuccessOrDie(gSafeAttributePersistenceProvider.Init(initParams.persistentStorageDelegate));
    SetSafeAttributePersistenceProvider(&gSafeAttributePersistenceProvider);

    DeviceLayer::DeviceInstanceInfoProvider * provider = DeviceLayer::GetDeviceInstanceInfoProvider();
    VerifyOrDie(provider != nullptr);

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
#endif
    });

    SuccessOrDie(sDehumidifierManager.Startup());

    initParams.dataModelProvider = &sDehumidifierManager.DataModelProvider();
    initParams.groupDataProvider = &gGroupDataProvider;

    CHIP_ERROR err = Server::GetInstance().Init(initParams);
    if (err != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Server::GetInstance().Init failed: %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }

    ConfigurationMgr().LogDeviceConfig();
    PrintOnboardingCodes(RendezvousInformationFlag::kBLE);
}

} // namespace

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Starting chip-dehumidifier-app");

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = esp_event_loop_create_default();
    ESP_ERROR_CHECK(err);

#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    if (DeviceLayer::Internal::ESP32Utils::InitWiFiStack() != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to initialize WiFi stack");
        return;
    }
#endif

    DeviceLayer::SetDeviceInfoProvider(&gExampleDeviceInfoProvider);

    CHIP_ERROR error = chip::Platform::MemoryInit();
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Platform::MemoryInit() failed: %" CHIP_ERROR_FORMAT, error.Format());
        return;
    }

    error = PlatformMgr().InitChipStack();
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "PlatformMgr().InitChipStack() failed: %" CHIP_ERROR_FORMAT, error.Format());
        return;
    }

    SuccessOrDie(PlatformMgr().AddEventHandler(DeviceEventHandler, 0));

#if CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER
    SetCommissionableDataProvider(&sFactoryDataProvider);
#if CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER
    SetDeviceInstanceInfoProvider(&sFactoryDataProvider);
#endif
    SetDeviceAttestationCredentialsProvider(&sFactoryDataProvider);
#else
    SetDeviceAttestationCredentialsProvider(Examples::GetExampleDACProvider());
#endif

    error = PlatformMgr().StartEventLoopTask();
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "PlatformMgr().StartEventLoopTask() failed: %" CHIP_ERROR_FORMAT, error.Format());
        return;
    }

    PlatformMgr().ScheduleWork(InitServer, 0);
}
