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

#include <lib/core/CHIPError.h>
#include <lib/support/CHIPArgParser.hpp>
#include <platform/CHIPDeviceConfig.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace chip::app {

class AppOptions
{
public:
    struct AppConfig
    {
        std::optional<uint16_t> port;
        std::vector<std::string> traceTo;

        std::optional<uint16_t> discriminator;
        std::optional<uint16_t> vendorId;
        std::optional<uint16_t> productId;
        std::optional<uint32_t> interfaceId;
        std::string kvsPath;
        std::optional<std::string> dacProvider;
        uint8_t testEventTriggerEnableKey[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        bool enableWiFi                       = false;
        uint32_t bleController                = 0;
    };

    static chip::ArgParser::OptionSet * GetOptions();
    static const AppConfig & GetConfig();
    static CHIP_ERROR ValidateConfig();

private:
    static bool DehumidifierAppOptionHandler(const char * program, chip::ArgParser::OptionSet * options, int identifier,
                                             const char * name, const char * value);

    static AppConfig mConfig;
    static bool sIsConfigValidated;
};

} // namespace chip::app
