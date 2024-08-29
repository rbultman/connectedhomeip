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

#include "fan-control-cluster-logic.h"

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

FanControlCapabilities::FanControlCapabilities() :
    supported_features(BitFlags<Clusters::FanControl::Feature>{0}),
    fan_mode_sequence{0},
    speed_max{0},
    rock_support(BitFlags<Clusters::FanControl::RockBitmap>{0}),
    wind_support(BitFlags<Clusters::FanControl::WindBitmap>{0}) {}

/* =========================== Start of FanControlClusterState =====================*/
CHIP_ERROR FanControlClusterState::Init(EndpointId endpoint_id)
{
    mEndpointId = endpoint_id;

    // Initialize defaults
    this->fan_mode = FanMode::Off;
    this->percent_setting = 0;
    this->percent_current = 0;
    this->speed_setting = 0;
    this->speed_current = 0;
    this->rock_setting;
    this->wind_setting;
    this->airflow_direction;

    return CHIP_NO_ERROR;
}

/* =========================== Start of FanControlClusterLogic =====================*/
CHIP_ERROR FanControlClusterLogic::Init(EndpointId endpoint_id, FanControlClusterState::NonVolatileStorageInterface & storage, DriverInterface & driver)
{
    mEndpointId = endpoint_id;

    mDriver = &driver;
    mDriver->Init();
    mCapabilities = mDriver->GetCapabilities(mEndpointId);

    CHIP_ERROR err = mClusterState.Init(endpoint_id, storage);
    if (err != CHIP_NO_ERROR)
    {
        Deinit();
        return err;
    }

    // Override defaults with capabilities;
    mClusterState.speed_attribute = mCapabilities.min_speed_value;
    mClusterState.axis_attribute = mCapabilities.min_axis_value;
    mClusterState.wobble_speed_attribute = mCapabilities.min_wobble_speed_value;

    ChipLogProgress(Zcl, "DiscoBall cluster id 0x%04x initialized on Endpoint %u", static_cast<unsigned>(GetClusterId()), static_cast<unsigned>(GetEndpointId()));

    return CHIP_NO_ERROR;
}

FanModeEnum FanControlClusterLogic::GetFanModeAttribute()
{
    return mClusterState.fan_mode;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetFanModeAttribute(FanModeEnum fan_mode)
{

}

FanModeSequenceEnum FanControlClusterLogic::GetFanModeSequenceAttribute()
{
    return mCapabilities.fan_mode_sequence;
}

Percent FanControlClusterLogic::GetPercentSettingAttribute()
{
    return mClusterState.percent_setting;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetPercentSettingAttribute(Percent percent_setting)
{

}

Percent FanControlClusterLogic::FanControlClusterLogic::GetPercentCurrentAttribute()
{
    return mClusterState.percent_current;
}

Protocols::InteractionModel::ClusterStatusCode SetPercentCurrentAttribute(Percent percent_current)
{

}

uint8_t FanControlClusterLogic::GetSpeedMaxAttribute()
{
    return mCapabilities.speed_max;
}

uint8_t FanControlClusterLogic::GetSpeedSettingAttribute()
{
    return mClusterState.speed_setting;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetSpeedSettingAttribute(uint8_t speed_setting)
{

}

uint8_t FanControlClusterLogic::GetSpeedCurrentAttribute()
{
    return mClusterState.speed_current;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetSpeedCurrentAttribute(uint8_t speed_current)
{

}

RockBitmap FanControlClusterLogic::GetRockSupportAttribute()
{
    return mCapabilities.rock_support;
}

RockBitmap FanControlClusterLogic::GetRockSettingAttribute()
{
    return mClusterState.speed_current;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetRockSettingAttribute(RockBitmap rock_setting)
{

}

WindBitmap FanControlClusterLogic::GetWindSupportAttribute()
{
    return mCapabilities.wind_support;
}

WindBitmap FanControlClusterLogic::GetWindSettingAttribute()
{
    return mClusterState.wind_setting;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetWindSettingAttribute(WindBitmap wind_setting)
{

}

AirflowDirectionEnum FanControlClusterLogic::GetAirflowDirectionAttribute()
{
    return mClusterState.airflow_direction;
}

Protocols::InteractionModel::ClusterStatusCode FanControlClusterLogic::SetAirflowDirectionAttribute(AirflowDirectionEnum airflow_direction)
{

}

ClusterStatusCode FanControlClusterLogic::HandleStepRequest(const Clusters::FanControl::Commands::Step::DecodableType & args)
{
    // TODO: Use actual driver capabilities for speed/rotate limits.
    VerifyOrReturnValue(mDriver != nullptr, Status::Failure);

    // Verify that the Step feature is support, else fail UnsupportedCommand

    if (args.direction == StepDirectionEnum::Increase)
    {

    }
    else if (args.direction == StepDirectionEnum::Decrease)
    {

    }
    else
    {
        return Status::ConstraintError;
    }

    if (args.wrap.HasValue())
    {

    }
    if (args.lowestOff.HasValue())
    {
        
    }

    // TODO: Validate these errors.
    VerifyOrReturnValue(args.speed <= mCapabilities.max_speed_value, Status::InvalidCommand);
    if (args.rotate.HasValue())
    {
        if (!mCapabilities.supported_features.Has(Clusters::FanControl::Feature::kReverse))
        {
            return ClusterStatusCode::ClusterSpecificFailure(Clusters::FanControl::StatusCode::kUnsupportedPattern);
        }
        ClusterStatusCode status = SetRotateAttribute(args.rotate.Value());
        VerifyOrReturnValue(status.IsSuccess(), status);
    }
    else
    {
        VerifyOrReturnValue(SetRotateAttribute(Clusters::FanControl::RotateEnum::kClockwise).IsSuccess(), Status::Failure);
    }

    VerifyOrReturnValue(SetSpeedAttribute(args.speed).IsSuccess(), Status::InvalidCommand);
    VerifyOrReturnValue(SetRunAttribute(true).IsSuccess(), Status::InvalidCommand);

    return Status::Success;
}

} // FanControl
} // Clusters
} // app
} // chip
