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

#include <app/clusters/humidistat-server/HumidistatTestEventTriggerHandler.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip::app {

namespace {
DehumidifierManager * sInstance = nullptr;
} // namespace

DehumidifierManager & DehumidifierManager::Instance()
{
    VerifyOrDie(sInstance != nullptr);
    return *sInstance;
}

DehumidifierManager::DehumidifierManager(const Context & context) :
    mContext(context), mDataModelProvider(mContext.storageDelegate, mAttributePersistence)
{
    VerifyOrDie(sInstance == nullptr);
    sInstance = this;
}

DehumidifierManager::~DehumidifierManager()
{
    if (sInstance == this)
    {
        sInstance = nullptr;
    }
}

CHIP_ERROR DehumidifierManager::Startup()
{
    ReturnErrorOnFailure(mAttributePersistence.Init(&mContext.storageDelegate));

    mRootNode = std::make_unique<RootNode>(RootNode::Context{
        .commissioningWindowManager          = mContext.commissioningWindowManager,
        .configurationManager                = mContext.configurationManager,
        .deviceControlServer                 = mContext.deviceControlServer,
        .fabricTable                         = mContext.fabricTable,
        .accessControl                       = mContext.accessControl,
        .persistentStorage                   = mContext.persistentStorage,
        .failSafeContext                     = mContext.failSafeContext,
        .deviceInstanceInfoProvider          = mContext.deviceInstanceInfoProvider,
        .platformManager                     = mContext.platformManager,
        .groupDataProvider                   = mContext.groupDataProvider,
        .sessionManager                      = mContext.sessionManager,
        .dnssdServer                         = mContext.dnssdServer,
        .deviceLoadStatusProvider            = mContext.deviceLoadStatusProvider,
        .diagnosticDataProvider              = mContext.diagnosticDataProvider,
        .testEventTriggerDelegate            = mContext.testEventTriggerDelegate,
        .dacProvider                         = mContext.dacProvider,
        .eventManagement                     = mContext.eventManagement,
        .timerDelegate                       = mContext.timerDelegate,
        .minGuaranteedSubscriptionsPerFabric = mContext.minGuaranteedSubscriptionsPerFabric,
#if CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
        .termsAndConditionsProvider = mContext.termsAndConditionsProvider,
#endif // CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED
    });

    ReturnErrorOnFailure(mRootNode->Register(kRootEndpointId, mDataModelProvider));

    mDehumidifier = std::make_unique<Dehumidifier>(mContext.timerDelegate);
    ReturnErrorOnFailure(mDehumidifier->Register(EndpointId(1), mDataModelProvider));

    return CHIP_NO_ERROR;
}

void DehumidifierManager::Shutdown()
{
    if (mDehumidifier)
    {
        mDehumidifier->Unregister(mDataModelProvider);
        mDehumidifier.reset();
    }

    if (mRootNode)
    {
        mRootNode->Unregister(mDataModelProvider);
        mRootNode.reset();
    }
}

} // namespace chip::app

bool HandleHumidistatTestEventTrigger(uint64_t eventTrigger)
{
    auto & cluster = chip::app::DehumidifierManager::Instance().GetDehumidifier().HumidistatCluster();
    switch (static_cast<chip::HumidistatTrigger>(eventTrigger))
    {
    case chip::HumidistatTrigger::kDisallowContinuous:
        cluster.SetSetSettingsAllowContinuous(false);
        return true;
    case chip::HumidistatTrigger::kAllowContinuous:
        cluster.SetSetSettingsAllowContinuous(true);
        return true;
    case chip::HumidistatTrigger::kDisallowSleep:
        cluster.SetSetSettingsAllowSleep(false);
        return true;
    case chip::HumidistatTrigger::kAllowSleep:
        cluster.SetSetSettingsAllowSleep(true);
        return true;
    case chip::HumidistatTrigger::kDisallowOptimal:
        cluster.SetSetSettingsAllowOptimal(false);
        return true;
    case chip::HumidistatTrigger::kAllowOptimal:
        cluster.SetSetSettingsAllowOptimal(true);
        return true;
    default:
        return false;
    }
}
