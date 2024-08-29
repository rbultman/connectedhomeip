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
#include "fan-control-cluster-server.h"

#include <app-common/zap-generated/cluster-objects.h>
#include <app/AttributeAccessInterface.h>
#include <app/CommandHandlerInterface.h>
#include <app/ConcreteAttributePath.h>
#include <app/InteractionModelEngine.h>
#include <app/MessageDef/StatusIB.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>
#include <lib/core/CHIPError.h>
#include <protocols/interaction_model/StatusCode.h>

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

using chip::Protocols::InteractionModel::ClusterStatusCode;
using chip::Protocols::InteractionModel::Status;
using chip::Access::SubjectDescriptor;

// This is the fan control command handler for all endpoints (endpoint ID is NullOptional)
FanControlServer::FanControlServer(ClusterId cluster_id) : CommandHandlerInterface(chip::NullOptional, cluster_id), AttributeAccessInterface(chip::NullOptional, cluster_id)
{
  registerAttributeAccessOverride(this);
  chip::app::InteractionModelEngine::GetInstance()->RegisterCommandHandler(this);
}

FanControlServer::~FanControlServer()
{
  unregisterAttributeAccessOverride(this);
  chip::app::InteractionModelEngine::GetInstance()->UnregisterCommandHandler(this);
}

/* =========================== Start of FanControlServer =====================*/
void FanControlServer::InvokeCommand(HandlerContext & handlerContext)
{
    EndpointId endpoint_id = handlerContext.mRequestPath.mEndpointId;
    FanControlClusterLogic * cluster = FindEndpoint(endpoint_id);
    if (cluster == nullptr)
    {
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::UnsupportedEndpoint);
        return;
    }

    SubjectDescriptor subject_descriptor = handlerContext.mCommandHandler.GetSubjectDescriptor();

    /*
        These are the commands handled by this cluster.
        |===
        | ID    | Name | Direction        | Response | Access | Conformance
        | 0x00 s| Step | client => server | Y        | O      | STEP
        |===
    */
    switch (handlerContext.mRequestPath.mCommandId)
    {
    case Clusters::FanControl::Commands::Step::Id:
        HandleCommand<Clusters::FanControl::Commands::Step::DecodableType>(handlerContext, [&](auto & _u, auto & payload) {
            ChipLogProgress(Zcl, "FanControl Step command received");
            ClusterStatusCode status = cluster->HandleStepRequest(payload);
            handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, status);
        });
        break;
    default:
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::UnsupportedCommand);
        break;
    }
}

/*
|===
| ID      | Name             | Type                 | Constraint   | Quality | Default | Access  | Conformance
| 0x0000 s| FanMode          | FanModeEnum          | all          | N       | 0       | RW VO   | M
| 0x0001 s| FanModeSequence  | FanModeSequenceEnum  | all          | F       | MS      | R V     | M
| 0x0002 s| PercentSetting   | percent              | max 100      | X       | 0       | RW VO   | M
| 0x0003 s| PercentCurrent   | percent              | max 100      | Q       | desc    | R V     | M
| 0x0004 s| SpeedMax         | uint8                | 1 to 100     | F       | MS      | R V     | SPD
| 0x0005 s| SpeedSetting     | uint8                | max SpeedMax | X       | 0       | RW VO   | SPD
| 0x0006 s| SpeedCurrent     | uint8                | max SpeedMax | P Q     | desc    | R V     | SPD
| 0x0007 s| RockSupport      | RockBitmap           | desc         | F       | 0       | R V     | RCK
| 0x0008 s| RockSetting      | RockBitmap           | desc         | P       | 0       | RW VO   | RCK
| 0x0009 s| WindSupport      | WindBitmap           | desc         | F       | 0       | R V     | WND
| 0x000A s| WindSetting      | WindBitmap           | desc         | P       | 0       | RW VO   | WND
| 0x000B s| AirflowDirection | AirflowDirectionEnum | desc         | P       | 0       | RW VO   | DIR
|===
*/
CHIP_ERROR FanControlServer::Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder)
{
    FanControlClusterLogic * cluster = FindEndpoint(aPath.mEndpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint));

    switch (aPath.mAttributeId)
    {
    case Clusters::FanControl::Attributes::FanMode::Id:
        ChipLogProgress(Zcl, "Read FanMode attribute");
        return aEncoder.Encode(cluster->GetFanModeAttribute());
    case Clusters::FanControl::Attributes::FanModeSequence::Id:
        ChipLogProgress(Zcl, "Read FanModeSequence attribute");
        return aEncoder.Encode(cluster->GetFanModeSequenceAttribute());
    case Clusters::FanControl::Attributes::PercentSetting::Id:
        ChipLogProgress(Zcl, "Read PercentSetting attribute");
        return aEncoder.Encode(cluster->GetPercentSettingAttribute());
    case Clusters::FanControl::Attributes::PercentCurrent::Id:
        ChipLogProgress(Zcl, "Read PercentCurrent attribute");
        return aEncoder.Encode(cluster->GetPercentCurrentAttribute());
    case Clusters::FanControl::Attributes::SpeedMax::Id:
        ChipLogProgress(Zcl, "Read SpeedMax attribute");
        return aEncoder.Encode(cluster->GetSpeedMaxAttribute());
    case Clusters::FanControl::Attributes::SpeedSetting::Id:
        ChipLogProgress(Zcl, "Read SpeedSetting attribute");
        return aEncoder.Encode(cluster->GetSpeedSettingAttribute());
    case Clusters::FanControl::Attributes::SpeedCurrent::Id:
        ChipLogProgress(Zcl, "Read SpeedCurrent attribute");
        return aEncoder.Encode(cluster->GetSpeedCurrentAttribute());
    case Clusters::FanControl::Attributes::RockSupport::Id:
        ChipLogProgress(Zcl, "Read RockSupport attribute");
        return aEncoder.Encode(cluster->GetRockSupportAttribute());
    case Clusters::FanControl::Attributes::RockSetting::Id:
        ChipLogProgress(Zcl, "Read RockSetting attribute");
        return aEncoder.Encode(cluster->GetRockSettingAttribute());
    case Clusters::FanControl::Attributes::WindSupport::Id:
        ChipLogProgress(Zcl, "Read WindSupport attribute");
        return aEncoder.Encode(cluster->GetWindSupportAttribute());
    case Clusters::FanControl::Attributes::WindSetting::Id:
        ChipLogProgress(Zcl, "Read WindSetting attribute");
        return aEncoder.Encode(cluster->GetWindSettingAttribute());
    case Clusters::FanControl::Attributes::AirflowDirection::Id:
        ChipLogProgress(Zcl, "Read AirflowDirection attribute");
        return aEncoder.Encode(cluster->GetAirflowDirectionAttribute());
    default:
        return Status::UnsupportedAttribute;
        break;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR FanControlServer::Write(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder)
{
    FanControlClusterLogic * cluster = FindEndpoint(aPath.mEndpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint));

    switch (aPath.mAttributeId)
    {
    case Clusters::FanControl::Attributes::FanMode::Id:
        ChipLogProgress(Zcl, "Write FanMode attribute");
        FanModeEnum fanMode;
        ReturnErrorOnFailure(aDecoder.Decode(fanMode));
        return cluster->SetFanModeAttribute(fanMode);

    case Clusters::FanControl::Attributes::FanModeSequence::Id:
        ChipLogProgress(Zcl, "Write FanModeSequence attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::PercentSetting::Id:
        ChipLogProgress(Zcl, "Write PercentSetting attribute");
        Percent percentSetting;
        ReturnErrorOnFailure(aDecoder.Decode(percentSetting));
        return cluster->SetPercentSettingAttribute(percentSetting);

    case Clusters::FanControl::Attributes::PercentCurrent::Id:
        ChipLogProgress(Zcl, "Write PercentCurrent attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::SpeedMax::Id:
        ChipLogProgress(Zcl, "Write SpeedMax attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::SpeedSetting::Id:
        ChipLogProgress(Zcl, "Write SpeedSetting attribute");
        uint8_t speedSetting;
        ReturnErrorOnFailure(aDecoder.Decode(speedSetting));
        return cluster->SetSpeedSettingAttribute(speedSetting);

    case Clusters::FanControl::Attributes::SpeedCurrent::Id:
        ChipLogProgress(Zcl, "Write SpeedCurrent attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::RockSupport::Id:
        ChipLogProgress(Zcl, "Write RockSupport attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::RockSetting::Id:
        ChipLogProgress(Zcl, "Write RockSetting attribute");
        RockBitmap rockSetting;
        ReturnErrorOnFailure(aDecoder.Decode(rockSetting));
        return cluster->SetRockSettingAttribute(rockSetting);

    case Clusters::FanControl::Attributes::WindSupport::Id:
        ChipLogProgress(Zcl, "Write WindSupport attribute");
        return Status::UnsupportedWrite;

    case Clusters::FanControl::Attributes::WindSetting::Id:
        ChipLogProgress(Zcl, "Write WindSetting attribute");
        WindBitmap windSetting;
        ReturnErrorOnFailure(aDecoder.Decode(windSetting));
        return cluster->SetWindSettingAttribute(windSetting);

    case Clusters::FanControl::Attributes::AirflowDirection::Id:
        ChipLogProgress(Zcl, "Write AirflowDirection attribute");
        AirflowDirectionEnum airflowDirection;
        ReturnErrorOnFailure(aDecoder.Decode(airflowDirection));
        return cluster->SetAirflowDirectionAttribute(airflowDirection);

    default:
        return Status::UnsupportedAttribute;
        break;
    }
    return CHIP_NO_ERROR;
}

} // FanControl
} // Clusters
} // app
} // chip
