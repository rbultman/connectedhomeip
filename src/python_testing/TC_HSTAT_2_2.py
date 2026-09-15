#
#    Copyright (c) 2026 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

# See https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/python.md#defining-the-ci-test-arguments
# for details about the block below.
#
# === BEGIN CI TEST ARGUMENTS ===
# test-runner-runs:
#   run1:
#     app: ${ALL_CLUSTERS_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --endpoint 1
#       --storage-path admin_storage.json
#       --commissioning-method on-network
#       --discriminator 1234
#       --passcode 20202021
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
# === END CI TEST ARGUMENTS ===

import logging

from mobly import asserts
from support_modules.hstat_common import HSTATBase

from matter.interaction_model import Status
from matter.testing.decorators import async_test_body, pics
from matter.testing.event_attribute_reporting import AttributeSubscriptionHandler
from matter.testing.runner import default_matter_test_main

log = logging.getLogger(__name__)


class TC_HSTAT_2_2(HSTATBase):

    @property
    def default_endpoint(self) -> int:
        return 1

    @pics('HSTAT.S')
    @async_test_body
    async def test_TC_HSTAT_2_2(self):
        """[TC-HSTAT-2.2] Mode functionality with DUT as Server"""
        self.step(1, "Commission DUT to TH (can be skipped if done in a preceding test)", is_commissioning=True)
        await self.setup()

        self.step(2, "TH reads from the DUT the SupportedModesMode attribute.",
                  expectation="Store the value as SupportedModes.")
        SupportedModes = await self.read_attribute_expect_success(attribute=self.attributes.SupportedModes)

        self.step(3, "TH sends command Off to the On/Off cluster on the same endpoint as this cluster.",
                  expectation="Verify DUT responds w/ status SUCCESS(0x00)")
        await self.send_onoff_off_cmd_expect_success()

        self.step(4, "TH reads from the DUT the SystemState attribute.",
                  expectation="Verify that the DUT response contains a value of Idle")
        dut_SystemState = await self.read_attribute_expect_success(attribute=self.attributes.SystemState)
        asserts.assert_equal(dut_SystemState, self.stateIdle, "SystemState is not idle")

        if self.humidifierFeatureSupported:
            self.step(5, "TH reads from the DUT the MistType attribute.",
                      expectation="Verify that the DUT response contains a value between 1 and 3 inclusive.")
            dut_MistType = await self.read_attribute_expect_success(attribute=self.attributes.MistType)
            asserts.assert_greater_equal(dut_MistType, 1, "MistType is less than 1")
            asserts.assert_less_equal(dut_MistType, 3, "MistType is greater than 3")
        else:
            self.skip_step(5)

        self.step(6, "TH sends command On to the On/Off cluster on the same endpoint as this cluster.",
                  expectation="Verify DUT responds w/ status SUCCESS(0x00)")
        await self.send_onoff_on_cmd_expect_success()

        self.step(7, "TH sends command SetSettings with the Continuous, Sleep, and Optimal fields set to False",
                  expectation="Verify DUT responds w/ status SUCCESS(0x00)")
        await self.send_SetSettingsCommand_expect_success(continuous=False, sleep=False, optimal=False)

        self.step(8, "Establish a subscription to the Mode and SystemState attributes",
                  expectation="This will receive updates when these attributes change value.")
        modeSubscription = AttributeSubscriptionHandler(self.cluster, self.attributes.Mode)
        stateSubscription = AttributeSubscriptionHandler(self.cluster, self.attributes.SystemState)
        await modeSubscription.start(self.default_controller, self.dut_node_id, self.endpoint)
        await stateSubscription.start(self.default_controller, self.dut_node_id, self.endpoint)
        dut_CurrentMode = await self.read_attribute_expect_success(attribute=self.attributes.Mode)
        dut_CurrentState = await self.read_attribute_expect_success(attribute=self.attributes.SystemState)
        log.info("Current mode: %s", dut_CurrentMode)
        log.info("Current state: %s", dut_CurrentState)

        modeReportsExpected = []
        modeReportsReceived = []

        self.step(9, "Iteratively write the value of the Mode attribute with the values in SupportedModes except for one.",
                  expectation="For each update, the DUT shall return a SUCCESS status code.")
        *mostModes, lastMode = SupportedModes
        for mode in mostModes:
            await self.write_single_attribute(attribute_value=self.attributes.Mode(mode), endpoint_id=self.endpoint, expect_success=True)
            if dut_CurrentMode != mode:
                modeReportsReceived.append(modeSubscription.wait_for_attribute_report().value)
                modeReportsExpected.append(mode)
                dut_CurrentMode = await self.read_attribute_expect_success(attribute=self.attributes.Mode)
                dut_CurrentState = await self.read_attribute_expect_success(attribute=self.attributes.SystemState)

        self.step(10, "Send the SetSettings command with the Mode field set to the remaining value from SupportedModes",
                  expectation="Verify DUT responds w/ status SUCCESS(0x00) "
                              "After all updates have been performed, "
                              "verify: The order of the values of the Mode attribute reports matches the order of the SupportedModes list. "
                              "The order of the values of the SystemState attribute reports matches the order of the SupportedModes list. "
                              "Where the value in the SupportedModes list is Humidifier, the value in associated report SHALL be Humidifying or Idle. "
                              "Where the value in the SupportedModes list is Dehumidifier, the value in associated report SHALL be Dehumidifying or Idle. "
                              "Where the value in the SupportedModes list is FanOnly, the value in associated report SHALL be Fan. "
                              "Where the value in the SupportedModes list is Auto, there MAY be one fewer report than for the Mode attribute or the corresponding report value SHALL be Idle, Humidifying or Dehumidifying.")
        dut_CurrentMode = await self.read_attribute_expect_success(attribute=self.attributes.Mode)
        await self.send_SetSettingsCommand_expect_success(mode=lastMode)
        if dut_CurrentMode != lastMode:
            modeReportsReceived.append(modeSubscription.wait_for_attribute_report().value)
            modeReportsExpected.append(lastMode)
        asserts.assert_equal(modeReportsExpected, modeReportsReceived, "Did not receive the expected attribute reports for mode.")
        # TODO: #74086

        modeSubscription.cancel()
        stateSubscription.cancel()

        self.step(11, "TH sends command SetSettings with the Mode field to Humidifier if supported or otherwise to Dehumidifier",
                  expectation="Verify DUT responds w/ status SUCCESS(0x00)")
        if self.humidifierFeatureSupported:
            await self.send_SetSettingsCommand_expect_success(mode=self.modeHumidifier)
        else:
            await self.send_SetSettingsCommand_expect_success(mode=self.modeDehumidifier)

        if not self.humidifierFeatureSupported:
            self.step(12, "TH sends command SetSettings with the Mode field set to Humidifier",
                      expectation="Verify DUT responds w/ status CONSTRAINT_ERROR(0x87)")
            await self.send_SetSettingsCommand_expect_error(mode=self.modeHumidifier, error=Status.ConstraintError)
        else:
            self.skip_step(12)

        if not self.dehumidifierFeatureSupported:
            self.step(13, "TH sends command SetSettings with the Mode field set to Dehumidifier",
                      expectation="Verify DUT responds w/ status CONSTRAINT_ERROR(0x87)")
            await self.send_SetSettingsCommand_expect_error(mode=self.modeDehumidifier, error=Status.ConstraintError)
        else:
            self.skip_step(13)

        if not self.fanOnlyFeatureSupported:
            self.step(14, "TH sends command SetSettings with the Mode field set to FanOnly",
                      expectation="Verify DUT responds w/ status CONSTRAINT_ERROR(0x87)")
            await self.send_SetSettingsCommand_expect_error(mode=self.modeFanOnly, error=Status.ConstraintError)
        else:
            self.skip_step(14)

        if not self.autoFeatureSupported:
            self.step(15, "TH sends command SetSettings with the Mode field set to Auto",
                      expectation="Verify DUT responds w/ status CONSTRAINT_ERROR(0x87)")
            await self.send_SetSettingsCommand_expect_error(mode=self.modeAuto, error=Status.ConstraintError)
        else:
            self.skip_step(15)

        if not self.continuousFeatureSupported:
            self.step(16, "If the Continuous feature is not supported then TH sends command SetSettings with the Continuous field set to True, otherwise skip this step",
                      expectation="Verify DUT responds w/ status SUCCESS(0x00)")
            await self.send_SetSettingsCommand_expect_success(continuous=True)
        else:
            self.skip_step(16)

        if self.attributes.Sleep.attribute_id not in self.supported_attributes:
            self.step(17, "If the Sleep feature is not supported then TH sends command SetSettings with the Sleep field set to True, otherwise skip this step",
                      expectation="Verify DUT responds w/ status SUCCESS(0x00)")
            await self.send_SetSettingsCommand_expect_success(sleep=True)
        else:
            self.skip_step(17)

        if not self.optimalFeatureSupported:
            self.step(18, "If the Optimal feature is not supported then TH sends command SetSettings with the Optimal field set to True, otherwise skip this step",
                      expectation="Verify DUT responds w/ status SUCCESS(0x00)")
            await self.send_SetSettingsCommand_expect_success(optimal=True)
        else:
            self.skip_step(18)

        if self.attributes.MistType.attribute_id not in self.supported_attributes:
            self.step(19, "If the MistType attribute is not supported then TH sends command SetSettings with the MistType field set to Cold, otherwise skip this step",
                      expectation="Verify DUT responds w/ status SUCCESS(0x00)")
            await self.send_SetSettingsCommand_expect_success(mistType=self.MistTypeBitmap.kMistCold)
        else:
            self.skip_step(19)


if __name__ == "__main__":
    default_matter_test_main()
