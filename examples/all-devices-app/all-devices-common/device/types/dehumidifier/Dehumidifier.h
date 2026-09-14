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

#pragma once

#include <app/clusters/fan-control-server/FanControlCluster.h>
#include <app/clusters/fan-control-server/fan-control-delegate.h>
#include <app/clusters/humidistat-server/HumidistatCluster.h>
#include <app/clusters/identify-server/IdentifyCluster.h>
#include <app/clusters/on-off-server/OnOffCluster.h>
#include <app/clusters/on-off-server/OnOffDelegate.h>
#include <app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <device/api/SingleEndpoint.h>
#include <lib/support/TimerDelegate.h>

namespace chip {
namespace app {

class Dehumidifier : public SingleEndpoint,
                     public Clusters::OnOffDelegate,
                     public Clusters::FanControl::Delegate,
                     public Clusters::HumidistatDelegate,
                     public Clusters::IdentifyDelegate
{
public:
    struct Context
    {
        TimerDelegate & timerDelegate;
        Clusters::IdentifyDelegate * identifyDelegate     = nullptr;
        Clusters::OnOffDelegate * onOffDelegate           = nullptr;
        Clusters::FanControl::Delegate * fanDelegate      = nullptr;
        Clusters::HumidistatDelegate * humidistatDelegate = nullptr;
    };

    explicit Dehumidifier(TimerDelegate & timerDelegate);
    Dehumidifier(const Context & context);
    ~Dehumidifier() override = default;

    CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                        EndpointComposition composition = {}) override;
    void Unregister(CodeDrivenDataModelProvider & provider) override;

    // Public cluster getters
    Clusters::OnOffCluster & OnOffCluster() { return mOnOffCluster.Cluster(); }
    Clusters::FanControlCluster & FanControlCluster() { return mFanControlCluster.Cluster(); }
    Clusters::HumidistatCluster & HumidistatCluster() { return mHumidistatCluster.Cluster(); }
    Clusters::RelativeHumidityMeasurementCluster & RelativeHumidityMeasurementCluster()
    {
        return mRelativeHumidityMeasurementCluster.Cluster();
    }
    Clusters::IdentifyCluster & IdentifyCluster() { return mIdentifyCluster.Cluster(); }

    // --- OnOffDelegate ---
    void OnOffStartup(bool on) override;
    void OnOnOffChanged(bool on) override;

    // --- FanControl::Delegate ---
    Protocols::InteractionModel::Status HandleStep(Clusters::FanControl::StepDirectionEnum aDirection, bool aWrap,
                                                   bool aLowestOff) override;
    void OnFanDriveStateChanged(const Clusters::FanControl::FanDriveState & newState) override;

    // --- HumidistatDelegate ---
    void OnModeChanged(Clusters::Humidistat::ModeEnum newMode) override;
    void OnSystemStateChanged(Clusters::Humidistat::SystemStateEnum newSystemState) override;
    void OnUserSetpointChanged(chip::Percent newUserSetpoint) override;
    void OnContinuousChanged(bool newContinuous) override;
    void OnCondPumpEnabledChanged(bool newCondPumpEnabled) override;
    void OnCondRunCountChanged(uint16_t newCondRunCount) override;

    // --- IdentifyDelegate ---
    void OnIdentifyStart(Clusters::IdentifyCluster & cluster) override;
    void OnIdentifyStop(Clusters::IdentifyCluster & cluster) override;
    void OnTriggerEffect(Clusters::IdentifyCluster & cluster) override;
    bool IsTriggerEffectEnabled() const override { return true; }

private:
    TimerDelegate & mTimerDelegate;
    Clusters::IdentifyDelegate * mExternalIdentifyDelegate;
    Clusters::OnOffDelegate * mExternalOnOffDelegate;
    Clusters::FanControl::Delegate * mExternalFanDelegate;
    Clusters::HumidistatDelegate * mExternalHumidistatDelegate;

    bool mIsOn = true;

    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
    LazyRegisteredServerCluster<Clusters::OnOffCluster> mOnOffCluster;
    LazyRegisteredServerCluster<Clusters::FanControlCluster> mFanControlCluster;
    LazyRegisteredServerCluster<Clusters::HumidistatCluster> mHumidistatCluster;
    LazyRegisteredServerCluster<Clusters::RelativeHumidityMeasurementCluster> mRelativeHumidityMeasurementCluster;
};

} // namespace app
} // namespace chip
