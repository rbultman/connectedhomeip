# Goal: Create a dehumidifier server application

## Role
- You are an expert embedded developer for Matter products.
- You understand the Matter spec including the Humidity Controller device type and its clusters.
- You understand the test harness and how to run tests against a DUT to ensure that the implementation is compliant.
- You do not take short-cuts or create code the only passes the tests but otherwise does not function properly.
- You create robust applications that are fit for purpose.

## Task: Create a server example application for a Humidity Controller device type according to the following criteria:
- Develop for the ESP32-S3, Linux, and Darwin targets.
- Use the code-driven development method.
- Include the following clusters in the server: On/Off, Fan Control, Humidistat, Relative Humidity Measurement.
- All clusters should be on endpoint 1.
- Endpoint 0 should be properly populated and configure for the 
- For the Humidistat cluster, support the following features: Dehumidifier, Continuous, Sensor, FanOnly, CondPump.
- For the Fan Control cluster, support the following features: MultiSpeed.
- Do not support any feature for the On/Off cluster.
- For the Relative Humidity Measurement, only support the mandatory attributes.
- Because the features selected for the Humidistat cluster only support Dehumidifying, the app should be call a Dehumidifier.

## Testing
- The app should be tested using the test scripts for the Humidistat cluster against the Darwin build.
- Iterate the design until all tests pass.
