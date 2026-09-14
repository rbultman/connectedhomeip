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

#include "AppOptions.h"

#include <lib/support/BytesToHex.h>
#include <lib/support/CHIPArgParser.hpp>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

#include <cstdlib>
#include <cstring>

using namespace chip;
using namespace chip::ArgParser;

namespace chip::app {

namespace {

constexpr uint16_t kOptionWiFi          = 0xffd2;
constexpr uint16_t kOptionKVS           = 0xffd3;
constexpr uint16_t kOptionDiscriminator = 0xffd4;
constexpr uint16_t kOptionVendorId      = 0xffd5;
constexpr uint16_t kOptionProductId     = 0xffd6;
constexpr uint16_t kOptionPort          = 0xffd7;
constexpr uint16_t kOptionInterfaceId   = 0xffd8;
constexpr uint16_t kOptionBLE           = 0xffd9;
constexpr uint16_t kOptionTraceTo       = 0xffdc;
constexpr uint16_t kOptionDacProvider   = 0xffdd;
constexpr uint16_t kOptionEnableKey     = 0xffde;

} // namespace

AppOptions::AppConfig AppOptions::mConfig;
bool AppOptions::sIsConfigValidated = false;

const AppOptions::AppConfig & AppOptions::GetConfig()
{
    VerifyOrDie(sIsConfigValidated);
    return mConfig;
}

CHIP_ERROR AppOptions::ValidateConfig()
{
    sIsConfigValidated = true;
    return CHIP_NO_ERROR;
}

bool AppOptions::DehumidifierAppOptionHandler(const char * program, OptionSet * options, int identifier, const char * name,
                                              const char * value)
{
    switch (identifier)
    {
    case kOptionBLE:
        if (!ParseInt(value, mConfig.bleController))
        {
            ChipLogError(Support, "Invalid BLE controller specified: %s", value);
            return false;
        }
        return true;
    case kOptionWiFi:
        mConfig.enableWiFi = true;
        ChipLogProgress(AppServer, "WiFi usage enabled");
        return true;
    case kOptionKVS:
        mConfig.kvsPath = value;
        return true;
    case kOptionDiscriminator: {
        char * endptr;
        unsigned long val = strtoul(value, &endptr, 0);
        if (*endptr != '\0' || val > 0xFFF)
        {
            ChipLogError(Support, "Invalid discriminator: %s", value);
            return false;
        }
        mConfig.discriminator = static_cast<uint16_t>(val);
        return true;
    }
    case kOptionVendorId:
        mConfig.vendorId = static_cast<uint16_t>(strtoul(value, nullptr, 0));
        return true;
    case kOptionProductId:
        mConfig.productId = static_cast<uint16_t>(strtoul(value, nullptr, 0));
        return true;
    case kOptionPort: {
        char * endptr;
        unsigned long val = strtoul(value, &endptr, 0);
        if (*endptr != '\0' || val > 0xFFFF)
        {
            ChipLogError(Support, "Invalid port: %s", value);
            return false;
        }
        mConfig.port = static_cast<uint16_t>(val);
        ChipLogProgress(AppServer, "Port option set to %u", static_cast<uint16_t>(val));
        return true;
    }
    case kOptionInterfaceId:
        mConfig.interfaceId = static_cast<uint32_t>(strtoul(value, nullptr, 0));
        return true;
    case kOptionTraceTo:
        mConfig.traceTo.push_back(value);
        ChipLogProgress(AppServer, "Added trace destination: %s", value);
        return true;
    case kOptionDacProvider:
        mConfig.dacProvider = value;
        ChipLogProgress(AppServer, "DAC provider file set to %s", value);
        return true;
    case kOptionEnableKey: {
        constexpr size_t kEnableKeyLength = sizeof(mConfig.testEventTriggerEnableKey);
        if (Encoding::HexToBytes(value, strlen(value), mConfig.testEventTriggerEnableKey, kEnableKeyLength) != kEnableKeyLength)
        {
            ChipLogError(Support, "%s: ERROR: invalid value specified for %s\n", program, name);
            return false;
        }
        ChipLogProgress(AppServer, "TestEventTrigger enable key configured");
        return true;
    }
    default:
        ChipLogError(Support, "%s: INTERNAL ERROR: Unhandled option: %s\n", program, name);
        return false;
    }

    return true;
}

OptionSet * AppOptions::GetOptions()
{
    static OptionDef sDehumidifierAppOptionDefs[] = {
#if CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE
        { "ble-controller", kArgumentRequired, kOptionBLE },
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
        { "wifi", kNoArgument, kOptionWiFi },
#endif
        { "KVS", kArgumentRequired, kOptionKVS },
        { "discriminator", kArgumentRequired, kOptionDiscriminator },
        { "vendor-id", kArgumentRequired, kOptionVendorId },
        { "product-id", kArgumentRequired, kOptionProductId },
        { "port", kArgumentRequired, kOptionPort },
        { "interface-id", kArgumentRequired, kOptionInterfaceId },
        { "trace-to", kArgumentRequired, kOptionTraceTo },
        { "dac_provider", kArgumentRequired, kOptionDacProvider },
        { "enable-key", kArgumentRequired, kOptionEnableKey },
        {}, // need empty terminator
    };

    static const std::string gHelpText = []() {
        std::string result;
#if CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE
        result += "  --ble-controller <number>\n";
        result += "       Select the BLE controller to use (default: 0)\n\n";
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
        result += "  --wifi\n";
        result += "       Enable wifi support for commissioning\n\n";
#endif
        result += "  --KVS <path>\n";
#if defined(CHIP_CONFIG_KVS_PATH)
        result += "       Path to the Key Value Store file (default: " CHIP_CONFIG_KVS_PATH ")\n\n";
#else
        result += "       Path to the Key Value Store file\n\n";
#endif
        result += "  --discriminator <number>\n";
        result += "       Discriminator value for commissioning (default: 3840)\n\n";
        result += "  --vendor-id <number>\n";
        result += "       Vendor ID value for commissioning\n\n";
        result += "  --product-id <number>\n";
        result += "       Product ID value for commissioning\n\n";
        result += "  --port <number>\n";
        result += "       Listen port for secure device messages (default: 5540)\n\n";
        result += "  --interface-id <number>\n";
        result += "       Interface ID to use for multicast DNS\n\n";
        result += "  --trace-to <destination>\n";
        result += "       Enable tracing destination (e.g., json:log, json:file_path)\n\n";
        result += "  --dac_provider <path>\n";
        result += "       Path to JSON file containing device attestation credentials\n\n";
        result += "  --enable-key <key>\n";
        result += "       A 16-byte, hex-encoded key, used to validate TestEventTrigger command\n\n";
        return result;
    }();

    static OptionSet sCmdLineOptions = { DehumidifierAppOptionHandler, sDehumidifierAppOptionDefs, "PROGRAM OPTIONS",
                                         gHelpText.c_str() };

    return &sCmdLineOptions;
}

} // namespace chip::app
