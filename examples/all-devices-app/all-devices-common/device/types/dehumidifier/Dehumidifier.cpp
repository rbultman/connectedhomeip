/*
 *    Copyright (c) 2026 Project CHIP Authors
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

#include "Dehumidifier.h"
#include <devices/Types.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/TypeTraits.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip::app::Clusters;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {

Dehumidifier::Dehumidifier(TimerDelegate & timerDelegate) :
    SingleEndpoint(Span<const DataModel::DeviceTypeEntry>(&Device::Type::kHumidityConditioner, 1)), mTimerDelegate(timerDelegate),
    mExternalIdentifyDelegate(nullptr), mExternalOnOffDelegate(nullptr), mExternalFanDelegate(nullptr),
    mExternalHumidistatDelegate(nullptr)
{}

Dehumidifier::Dehumidifier(const Context & context) :
    SingleEndpoint(Span<const DataModel::DeviceTypeEntry>(&Device::Type::kHumidityConditioner, 1)),
    mTimerDelegate(context.timerDelegate), mExternalIdentifyDelegate(context.identifyDelegate),
    mExternalOnOffDelegate(context.onOffDelegate), mExternalFanDelegate(context.fanDelegate),
    mExternalHumidistatDelegate(context.humidistatDelegate)
{}

CHIP_ERROR Dehumidifier::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                                  EndpointComposition composition)
{
    VerifyOrReturnError(mEndpointId == kInvalidEndpointId, CHIP_ERROR_INCORRECT_STATE);
    DeviceRegistrationTransaction transaction(*this, provider);

    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, composition));

    // 1. Identify Cluster
    mIdentifyCluster.Create(IdentifyCluster::Config(endpoint, mTimerDelegate)
                                .WithDelegate(mExternalIdentifyDelegate ? mExternalIdentifyDelegate : this));
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    // 2. On/Off Cluster (No features supported)
    OnOffCluster::Context onOffContext{ mTimerDelegate };
    onOffContext.featureMap     = {};
    onOffContext.defaults.onOff = true;
    mOnOffCluster.Create(endpoint, onOffContext);
    mOnOffCluster.Cluster().AddDelegate(this);
    ReturnErrorOnFailure(provider.AddCluster(mOnOffCluster.Registration()));

    // 3. Fan Control Cluster (MultiSpeed feature only)
    FanControlCluster::Config fanConfig(endpoint, mExternalFanDelegate ? *mExternalFanDelegate : *this);
    fanConfig.WithSpeedMax(10);
    fanConfig.WithFanModeSequence(FanControl::FanModeSequenceEnum::kOffLowMedHigh);
    mFanControlCluster.Create(fanConfig);
    ReturnErrorOnFailure(provider.AddCluster(mFanControlCluster.Registration()));

    // 4. Humidistat Cluster (Dehumidifier, Continuous, Sensor, FanOnly, CondPump)
    BitFlags<Humidistat::Feature> features;
    features.Set(Humidistat::Feature::kDehumidifier);
    features.Set(Humidistat::Feature::kContinuous);
    features.Set(Humidistat::Feature::kSensor);
    features.Set(Humidistat::Feature::kFanOnly);
    features.Set(Humidistat::Feature::kCondPump);

    HumidistatCluster::StartupConfiguration hstatConfig;
    hstatConfig.mode         = Humidistat::ModeEnum::kDehumidifier;
    hstatConfig.systemState  = Humidistat::SystemStateEnum::kDehumidifying;
    hstatConfig.minSetpoint  = 10;
    hstatConfig.maxSetpoint  = 90;
    hstatConfig.step         = 5;
    hstatConfig.userSetpoint = 50;
    hstatConfig.continuous   = false;

    mHumidistatCluster.Create(endpoint, features, HumidistatCluster::OptionalAttributeSet(), hstatConfig);
    mHumidistatCluster.Cluster().SetDelegate(this);
    ReturnErrorOnFailure(provider.AddCluster(mHumidistatCluster.Registration()));

    // 5. Relative Humidity Measurement Cluster (Mandatory attributes only)
    RelativeHumidityMeasurementCluster::Config rhConfig;
    rhConfig.minMeasuredValue.SetNonNull(1000);
    rhConfig.maxMeasuredValue.SetNonNull(9000);
    mRelativeHumidityMeasurementCluster.Create(endpoint, rhConfig);
    ReturnErrorOnFailure(
        mRelativeHumidityMeasurementCluster.Cluster().SetMeasuredValue(DataModel::MakeNullable(static_cast<uint16_t>(5000))));
    ReturnErrorOnFailure(provider.AddCluster(mRelativeHumidityMeasurementCluster.Registration()));

    ReturnErrorOnFailure(provider.AddEndpoint(mEndpointRegistration));
    transaction.Commit();
    return CHIP_NO_ERROR;
}

void Dehumidifier::Unregister(CodeDrivenDataModelProvider & provider)
{
    UnregisterDescriptor(provider);

    if (mRelativeHumidityMeasurementCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mRelativeHumidityMeasurementCluster.Cluster()));
        mRelativeHumidityMeasurementCluster.Destroy();
    }
    if (mHumidistatCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mHumidistatCluster.Cluster()));
        mHumidistatCluster.Destroy();
    }
    if (mFanControlCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mFanControlCluster.Cluster()));
        mFanControlCluster.Destroy();
    }
    if (mOnOffCluster.IsConstructed())
    {
        mOnOffCluster.Cluster().RemoveDelegate(this);
        LogErrorOnFailure(provider.RemoveCluster(&mOnOffCluster.Cluster()));
        mOnOffCluster.Destroy();
    }
    if (mIdentifyCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mIdentifyCluster.Cluster()));
        mIdentifyCluster.Destroy();
    }
}

void Dehumidifier::OnOffStartup(bool on)
{
    mIsOn = on;
    if (mExternalOnOffDelegate != nullptr)
    {
        mExternalOnOffDelegate->OnOffStartup(on);
    }

    if (mFanControlCluster.IsConstructed())
    {
        if (!on)
        {
            mFanControlCluster.Cluster().SetPercentCurrent(0);
            mFanControlCluster.Cluster().SetSpeedCurrent(0);
        }
        else
        {
            const auto percentSetting = mFanControlCluster.Cluster().GetPercentSetting();
            if (!percentSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetPercentCurrent(percentSetting.Value());
            }
            const auto speedSetting = mFanControlCluster.Cluster().GetSpeedSetting();
            if (!speedSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetSpeedCurrent(speedSetting.Value());
            }
        }
    }
}

void Dehumidifier::OnOnOffChanged(bool on)
{
    mIsOn = on;
    if (mExternalOnOffDelegate != nullptr)
    {
        mExternalOnOffDelegate->OnOnOffChanged(on);
    }

    if (mFanControlCluster.IsConstructed())
    {
        if (!on)
        {
            mFanControlCluster.Cluster().SetPercentCurrent(0);
            mFanControlCluster.Cluster().SetSpeedCurrent(0);
        }
        else
        {
            const auto percentSetting = mFanControlCluster.Cluster().GetPercentSetting();
            if (!percentSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetPercentCurrent(percentSetting.Value());
            }
            const auto speedSetting = mFanControlCluster.Cluster().GetSpeedSetting();
            if (!speedSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetSpeedCurrent(speedSetting.Value());
            }
        }
    }

    if (mHumidistatCluster.IsConstructed())
    {
        if (!on)
        {
            LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kIdle));
        }
        else
        {
            auto mode = mHumidistatCluster.Cluster().GetMode();
            if (mode == Humidistat::ModeEnum::kDehumidifier)
            {
                LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kDehumidifying));
            }
            else if (mode == Humidistat::ModeEnum::kFanOnly)
            {
                LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kFan));
            }
        }
    }
}

Status Dehumidifier::HandleStep(FanControl::StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    if (mExternalFanDelegate != nullptr)
    {
        return mExternalFanDelegate->HandleStep(aDirection, aWrap, aLowestOff);
    }

    VerifyOrReturnError(aDirection != FanControl::StepDirectionEnum::kUnknownEnumValue, Status::InvalidCommand);

    auto & cluster          = FanControlCluster();
    const uint8_t speedMax  = cluster.GetSpeedMax();
    const auto speedSetting = cluster.GetSpeedSetting();
    uint8_t newSpeedSetting = speedSetting.ValueOr(0);

    if (aDirection == FanControl::StepDirectionEnum::kIncrease)
    {
        if (speedSetting.IsNull())
        {
            newSpeedSetting = 1;
        }
        else if (speedSetting.Value() < speedMax)
        {
            newSpeedSetting = static_cast<uint8_t>(speedSetting.Value() + 1);
        }
        else if (speedSetting.Value() == speedMax)
        {
            if (aWrap)
            {
                newSpeedSetting = aLowestOff ? 0 : 1;
            }
        }
    }
    else if (aDirection == FanControl::StepDirectionEnum::kDecrease)
    {
        if (speedSetting.IsNull())
        {
            newSpeedSetting = aLowestOff ? 0 : 1;
        }
        else if ((speedSetting.Value() > 1) && (speedSetting.Value() <= speedMax))
        {
            newSpeedSetting = static_cast<uint8_t>(speedSetting.Value() - 1);
        }
        else if (speedSetting.Value() == 1)
        {
            if (aLowestOff)
            {
                newSpeedSetting = 0;
            }
            else if (aWrap)
            {
                newSpeedSetting = speedMax;
            }
        }
        else if (speedSetting.Value() == 0)
        {
            if (aWrap)
            {
                newSpeedSetting = speedMax;
            }
        }
    }

    Status status = cluster.SetSpeedSetting(DataModel::MakeNullable(newSpeedSetting));
    VerifyOrReturnError(status == Status::Success, status);
    VerifyOrReturnError(cluster.SetSpeedCurrent(newSpeedSetting), Status::Failure);
    return Status::Success;
}

void Dehumidifier::OnFanDriveStateChanged(const FanControl::FanDriveState & newState)
{
    if (mExternalFanDelegate != nullptr)
    {
        mExternalFanDelegate->OnFanDriveStateChanged(newState);
    }

    if (mFanControlCluster.IsConstructed())
    {
        if (!mIsOn)
        {
            mFanControlCluster.Cluster().SetPercentCurrent(0);
            mFanControlCluster.Cluster().SetSpeedCurrent(0);
        }
        else
        {
            if (!newState.percentSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetPercentCurrent(newState.percentSetting.Value());
            }
            if (!newState.speedSetting.IsNull())
            {
                mFanControlCluster.Cluster().SetSpeedCurrent(newState.speedSetting.Value());
            }
        }
    }
}

void Dehumidifier::OnModeChanged(Humidistat::ModeEnum newMode)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnModeChanged(newMode);
    }

    if (mHumidistatCluster.IsConstructed())
    {
        if (!mIsOn)
        {
            LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kIdle));
        }
        else
        {
            if (newMode == Humidistat::ModeEnum::kDehumidifier)
            {
                LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kDehumidifying));
            }
            else if (newMode == Humidistat::ModeEnum::kFanOnly)
            {
                LogErrorOnFailure(mHumidistatCluster.Cluster().SetSystemState(Humidistat::SystemStateEnum::kFan));
            }
        }
    }
}

void Dehumidifier::OnSystemStateChanged(Humidistat::SystemStateEnum newSystemState)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnSystemStateChanged(newSystemState);
    }
}

void Dehumidifier::OnUserSetpointChanged(chip::Percent newUserSetpoint)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnUserSetpointChanged(newUserSetpoint);
    }
}

void Dehumidifier::OnContinuousChanged(bool newContinuous)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnContinuousChanged(newContinuous);
    }
}

void Dehumidifier::OnCondPumpEnabledChanged(bool newCondPumpEnabled)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnCondPumpEnabledChanged(newCondPumpEnabled);
    }
}

void Dehumidifier::OnCondRunCountChanged(uint16_t newCondRunCount)
{
    if (mExternalHumidistatDelegate != nullptr)
    {
        mExternalHumidistatDelegate->OnCondRunCountChanged(newCondRunCount);
    }
}

void Dehumidifier::OnIdentifyStart(Clusters::IdentifyCluster & cluster)
{
    ChipLogProgress(DeviceLayer, "Dehumidifier: Identify started");
}

void Dehumidifier::OnIdentifyStop(Clusters::IdentifyCluster & cluster)
{
    ChipLogProgress(DeviceLayer, "Dehumidifier: Identify stopped");
}

void Dehumidifier::OnTriggerEffect(Clusters::IdentifyCluster & cluster)
{
    ChipLogProgress(DeviceLayer, "Dehumidifier: Identify TriggerEffect");
}

} // namespace app
} // namespace chip
