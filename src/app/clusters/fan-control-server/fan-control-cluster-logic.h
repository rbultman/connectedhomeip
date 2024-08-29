/*
 *
 *    Copyright (c) 2020-2024 Project CHIP Authors
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

#include <stdint.h>

#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>
#include <lib/core/Optional.h>
#include <lib/support/Span.h>
#include <lib/support/BitFlags.h>

#include <protocols/interaction_model/StatusCode.h>

#include <app/ConcreteAttributePath.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/cluster-enums.h>

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

/* Device features, limits, and attributes with fixed quality */
struct FanControlCapabilities
{
    FanControlCapabilities();

    BitFlags<Clusters::FanControl::Feature> supported_features;

    // Mandatory
    FanModeSequenceEnum fan_mode_sequence;

    // Only valid if supported_features.Has(FanControl::Feature::kMultiSpeed)
    uint8_t speed_max;

    // Only valid if supported_features.Has(FanControl::Feature::kRocking)
    RockBitmap rock_support;

    // Only valid if supported_features.Has(FanControl::Feature::kWind)
    WindBitmap wind_support;
};

struct FanControlClusterState
{
    FanControlClusterState() = default;

    CHIP_ERROR Init(EndpointId endpoint_id);
    void Deinit()
    {
        mIsInitialized = false;
    }

    bool IsInitialized() const { return mStorage != nullptr; }

    // Attributes storage
    FanModeEnum fan_mode;
    Percent percent_setting;
    Percent percent_current;
    uint8_t speed_setting;
    uint8_t speed_current;
    RockBitmap rock_setting;
    WindBitmap wind_setting;
    AirflowDirectionEnum airflow_direction;

private:
    bool mIsInitialized = false;
    EndpointId mEndpointId = kInvalidEndpointId;
}

class FanControlClusterLogic
{
public:
    class DriverInterface
    {
    public:
        virtual ~DriverInterface() = default;
        virtual void Init() {}
        virtual FanControlCapabilities GetCapabilities(EndpointId endpoint_id) const = 0;

        virtual Protocols::InteractionModel::Status OnClusterStateChange(EndpointId endpoint_id, BitFlags<FanControlFunction> changes, FanControlClusterLogic & cluster) = 0;
        // need some callbacks here to the business logic
        virtual void StartPatternTimer(EndpointId endpoint_id, uint16_t num_seconds, FanControlTimerCallback timer_cb, void * ctx) = 0;
        virtual void CancelPatternTimer(EndpointId endpoint_id) = 0;
        virtual void MarkAttributeDirty(const ConcreteAttributePath& path) = 0;
    };
    
    FanControlClusterLogic() = default;

    // Not copyable.
    FanControlClusterLogic(FanControlClusterLogic const&) = delete;
    FanControlClusterLogic& operator=(FanControlClusterLogic const&) = delete;

    CHIP_ERROR Init(EndpointId endpoint_id, FanControlClusterState::NonVolatileStorageInterface & storage, DriverInterface & driver);
    void Deinit()
    {
        mDriver = nullptr;
        mEndpointId = kInvalidEndpointId;
    }

    EndpointId GetEndpointId() const { return mEndpointId; }
    ClusterId GetClusterId() const { return Clusters::FanControl::Id; }

    //
    // Attribute reads and writes are indirected to the FanControlClusterState by the FanControlClusterLogic.
    //
    FanModeEnum GetFanModeAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetFanModeAttribute(FanModeEnum fan_mode);

    FanModeSequenceEnum GetFanModeSequenceAttribute() const;

    Percent GetPercentSettingAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetPercentSettingAttribute(Percent percent_setting);

    Percent GetPercentCurrentAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetPercentCurrentAttribute(Percent percent_current);

    uint8_t GetSpeedMaxAttribute() const;

    uint8_t GetSpeedSettingAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetSpeedSettingAttribute(uint8_t speed_setting);

    uint8_t GetSpeedCurrentAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetSpeedCurrentAttribute(uint8_t speed_current);

    RockBitmap GetRockSupportAttribute() const;

    RockBitmap GetRockSettingAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetRockSettingAttribute(RockBitmap rock_setting);

    WindBitmap GetWindSupportAttribute() const;

    WindBitmap GetWindSettingAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetWindSettingAttribute(WindBitmap wind_setting);

    AirflowDirectionEnum GetAirflowDirectionAttribute() const;
    Protocols::InteractionModel::ClusterStatusCode SetAirflowDirectionAttribute(AirflowDirectionEnum airflow_direction);

    //
    // Command handlers
    //
    Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::HandleStepRequest(const Clusters::FanControl::Commands::Step::DecodableType & args);

private:
    EndpointId mEndpointId = kInvalidEndpointId;
    DriverInterface * mDriver = nullptr;

    // Configuration from the business end.
    FanControlCapabilities mCapabilities;

    // All possibly volatile data state for this cluster.
    FanControlClusterState mClusterState{};
}

} // FanControl
} // Clusters
} // app
} // chip
