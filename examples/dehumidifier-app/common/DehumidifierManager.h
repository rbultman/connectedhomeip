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

#pragma once

#include <app/DeviceLoadStatusProvider.h>
#include <app/EventManagement.h>
#include <app/FailSafeContext.h>
#include <app/TestEventTriggerDelegate.h>
#include <app/persistence/DefaultAttributePersistenceProvider.h>
#include <app/server/CommissioningWindowManager.h>
#include <app/server/Dnssd.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/GroupDataProvider.h>
#include <crypto/CHIPCryptoPAL.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <device/types/dehumidifier/Dehumidifier.h>
#include <device/types/root-node/RootNode.h>
#include <lib/core/CHIPError.h>
#include <lib/support/TimerDelegate.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ConfigurationManager.h>
#include <platform/DeviceControlServer.h>
#include <platform/DiagnosticDataProvider.h>
#include <platform/PlatformManager.h>
#include <transport/SecureSessionTable.h>
#include <transport/SessionManager.h>

#include <memory>

namespace chip::app {

class DehumidifierManager
{
public:
    struct Context
    {
        PersistentStorageDelegate & storageDelegate;
        CommissioningWindowManager & commissioningWindowManager;
        DeviceLayer::ConfigurationManager & configurationManager;
        DeviceLayer::DeviceControlServer & deviceControlServer;
        FabricTable & fabricTable;
        Access::AccessControl & accessControl;
        PersistentStorageDelegate & persistentStorage;
        FailSafeContext & failSafeContext;
        DeviceLayer::DeviceInstanceInfoProvider & deviceInstanceInfoProvider;
        DeviceLayer::PlatformManager & platformManager;
        Credentials::GroupDataProvider & groupDataProvider;
        SessionManager & sessionManager;
        DnssdServer & dnssdServer;
        DeviceLoadStatusProvider & deviceLoadStatusProvider;
        DeviceLayer::DiagnosticDataProvider & diagnosticDataProvider;
        TestEventTriggerDelegate * testEventTriggerDelegate;
        Credentials::DeviceAttestationCredentialsProvider & dacProvider;
        EventManagement & eventManagement;
        TimerDelegate & timerDelegate;
        uint16_t minGuaranteedSubscriptionsPerFabric;
#if CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
        TermsAndConditionsProvider & termsAndConditionsProvider;
#endif // CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
    };

    static DehumidifierManager & Instance();

    explicit DehumidifierManager(const Context & context);
    ~DehumidifierManager();

    CHIP_ERROR Startup();
    void Shutdown();

    CodeDrivenDataModelProvider & DataModelProvider() { return mDataModelProvider; }
    Dehumidifier & GetDehumidifier() { return *mDehumidifier; }

private:
    Context mContext;
    DefaultAttributePersistenceProvider mAttributePersistence;
    CodeDrivenDataModelProvider mDataModelProvider;
    std::unique_ptr<RootNode> mRootNode;
    std::unique_ptr<Dehumidifier> mDehumidifier;
};

} // namespace chip::app
