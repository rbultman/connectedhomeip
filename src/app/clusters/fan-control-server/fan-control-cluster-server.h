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

#include "fan-control-cluster-logic.h"

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

class FanControlServer : public CommandHandlerInterface, public AttributeAccessInterface
{
public:
    explicit FanControlServer(ClusterId cluster_id);
    virtual ~FanControlServer();

    FanControlServer() = delete;
    FanControlServer(FanControlServer const&) = delete;
    FanControlServer& operator=(FanControlServer const&) = delete;

    // Inherited from CommandHandlerInterface
    void InvokeCommand(HandlerContext & handlerContext) override;

    // AttributeAccessInterface
    CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;
    CHIP_ERROR Write(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder) override;

    CHIP_ERROR RegisterEndpoint(EndpointId endpoint_id, FanControlClusterState::NonVolatileStorageInterface & storage, FanControlClusterLogic::DriverInterface & driver);
    void UnregisterEndpoint(EndpointId endpoint_id);
    FanControlClusterLogic * FindEndpoint(EndpointId endpoint_id);

private:
// TODO: Handle dynamic memory management of cluster state.
    FanControlClusterLogic mEndpoints[1];
};

} // FanControl
} // Clusters
} // app
} // chip
