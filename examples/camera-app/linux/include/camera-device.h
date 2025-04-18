/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
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
#include "camera-device-interface.h"
#include "chime-manager.h"
#include "webrtc-provider-manager.h"
#include <protocols/interaction_model/StatusCode.h>

namespace Camera {

class CameraDevice : public CameraDeviceInterface
{

public:
    CameraDevice();

    virtual chip::app::Clusters::ChimeDelegate & GetChimeDelegate();
    virtual chip::app::Clusters::WebRTCTransportProvider::Delegate & GetWebRTCProviderDelegate();

private:
    // Various cluster server delegates
    ChimeManager mChimeManager;
    WebRTCProviderManager mWebRTCProviderManager;
};

} // namespace Camera
