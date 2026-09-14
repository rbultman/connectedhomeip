# Dehumidifier App Build & AI Implementation Summary

This document summarizes the design, architecture, build integration, and verification of the code-driven Matter **Dehumidifier** application based on the requirements in `dehumifier.md` and the guidelines in `AGENTS.md`.

---

## 1. Overview & Data Model Specifications

The application implements the Matter **Humidity Controller** device type (`0x007D`, revision 1) using the modern code-driven data model (`CodeDrivenDataModelProvider`, `SingleEndpoint`, `DefaultServerCluster`) rather than legacy ZAP/Ember codegen.

### Endpoint Structure

* **Endpoint 0 (Root Node)**:
  * Implemented using the code-driven `RootNode` endpoint (`examples/all-devices-app/all-devices-common/device/types/root/RootNode.h`).
  * Hosts standard mandatory clusters: Basic Information, Access Control, General Diagnostics, Network Commissioning, Administrator Commissioning, Operational Credentials, Localization, and Time Synchronization.

* **Endpoint 1 (Humidity Controller - Device Type `0x007D`, Rev 1)**:
  * **Descriptor**: Device type `0x007D` (Humidity Controller), revision 1.
  * **Identify (`0x0003`)**: Mandatory cluster with default identify delegate handling.
  * **On/Off (`0x0006`)**: Feature map `0` (no optional features). Synchronized with Humidistat cluster state: toggling On/Off updates Humidistat `SystemState` (`kDehumidifying` when On, `kIdle` when Off).
  * **Fan Control (`0x0202`)**: `MultiSpeed` feature only (`WithSpeedMax(10)`).
  * **Humidistat (`0x0205`)**:
    * Features enabled: `Dehumidifier` (bit 1), `Continuous` (bit 3), `Sensor` (bit 5), `FanOnly` (bit 7), `CondPump` (bit 8).
    * Startup / initial attributes:
      * `Mode`: `kDehumidifier`
      * `SystemState`: `kDehumidifying`
      * `UserSetpoint`: `50%`
      * `MinSetpoint`: `30%`
      * `MaxSetpoint`: `80%`
      * `SetpointStep`: `5%`
      * `Continuous`: `false`
      * `CondPumpEnabled`: `false`
      * `CondRunCount`: `0`
    * Command handling: Implements `SetSettings` command validation and execution for Mode, UserSetpoint, and Continuous flags.
    * Test event trigger handling: Supports event codes `0x0205000000000000` through `0x0205000000000005` to disallow/allow continuous, sleep, and optimal modes for certification tests.
  * **Relative Humidity Measurement (`0x0405`)**: Mandatory attributes (`MeasuredValue = 5000` [50.00%], `MinMeasuredValue = 0`, `MaxMeasuredValue = 10000`).

---

## 2. Source Code Architecture

### 2.1 Code-Driven Device Type
Located at: `examples/all-devices-app/all-devices-common/device/types/dehumidifier/`

* **`Dehumidifier.h` / `Dehumidifier.cpp`**:
  * Inherits from `SingleEndpoint`.
  * Instantiates and registers the `IdentifyCluster`, `OnOffCluster`, `FanControlCluster`, `HumidistatCluster`, and `RelativeHumidityMeasurementCluster`.
  * Coordinates On/Off state changes with Humidistat system state and Fan Control speeds.
* **`impl/LoggingDehumidifier.h` / `impl/LoggingDehumidifier.cpp`**:
  * Provides default logging delegates for state transitions.
* **`BUILD.gn`**:
  * GN target `:dehumidifier` for use across host and embedded builds.

### 2.2 Standalone Dehumidifier Application
Located at: `examples/dehumidifier-app/`

* **`common/DehumidifierManager.h` / `common/DehumidifierManager.cpp`**:
  * Manages application state and data model provider.
  * Registers Endpoint 0 (`RootNode`) and Endpoint 1 (`Dehumidifier`) with `CodeDrivenDataModelProvider`.
  * Implements `HumidistatTestEventTriggerHandler` for certification event triggers.
* **`common/BUILD.gn`**:
  * GN target `:dehumidifier-common`.
* **`posix/AppOptions.h` / `posix/AppOptions.cpp`**:
  * Parses command-line arguments: `--discriminator`, `--KVS`, `--port`, `--trace-to`, `--enable-key`, `--dac_provider`.
* **`posix/main.cpp`**:
  * Posix entrypoint initializing the Matter stack, registering `SimpleTestEventTriggerDelegate` with the enable key, starting `DehumidifierManager`, and driving the event loop.
* **`posix/BUILD.gn` & `examples/dehumidifier-app/BUILD.gn`**:
  * Builds the executable binary `chip-dehumidifier-app` (including Darwin code-signing action).
* **`esp32/`**:
  * Full ESP32 / ESP32-S3 build support with CMake configs, partition tables, and SDK defaults (`sdkconfig.defaults`, `sdkconfig.defaults.esp32s3`).

---

## 3. Build System Integration

The application is integrated into the Matter unified build tools:

* **`scripts/build/builders/host.py`**: Added `HostApp.DEHUMIDIFIER` targeting `dehumidifier-app/posix` producing `chip-dehumidifier-app`.
* **`scripts/build/builders/esp32.py`**: Added `Esp32App.DEHUMIDIFIER` targeting `examples/dehumidifier-app` producing `chip-dehumidifier-app`.
* **`scripts/build/build/targets.py`**: Added `TargetPart('dehumidifier', ...)` to host and ESP32 targets.

---

## 4. Key Platform & Testing Resolutions

1. **Darwin Platform mDNS Resolution**:
   * On macOS, Darwin applications advertise via native Bonjour (`DNSServiceRegister`).
   * The Python test controller was rebuilt with `-m platform` (`./scripts/build_python.sh -i out/venv -m platform -c no --enable_ipv4 true`) so discovery and commissioning utilize Darwin's native `DNSServiceBrowse`/`DNSServiceResolve` rather than raw minimal-mDNS multicast sockets (which trigger `EHOSTUNREACH` without scoped IPv6 routes).
2. **KVS Factory Reset on Darwin**:
   * On Darwin POSIX builds, relative KVS paths default to the user's `~/Documents/` folder. Using `--KVS /tmp/chip_kvs1` allows `run_python_test.py --factory-reset` to cleanly wipe persistent storage between consecutive test runs.

---

## 5. Certification Test Results

All four Humidistat certification test suites pass against the Darwin build:

```bash
for test in TC_HSTAT_2_1 TC_HSTAT_2_2 TC_HSTAT_2_3 TC_HSTAT_2_4; do
  PATH="$PWD/out/venv/bin:$PATH" out/venv/bin/python3 ./scripts/tests/run_python_test.py \
    --app out/darwin-arm64-dehumidifier-clang/chip-dehumidifier-app \
    --app-args "--discriminator 1234 --KVS /tmp/chip_kvs1" \
    --app-ready-pattern "===== APP STATUS: Starting event loop =====" \
    --script src/python_testing/${test}.py \
    --script-args "--endpoint 1 --storage-path admin_storage.json --commissioning-method on-network --discriminator 1234 --passcode 20202021 --PICS src/app/tests/suites/certification/ci-pics-values" \
    --factory-reset
done
```

| Test Suite | Description | Result |
| :--- | :--- | :--- |
| **`TC_HSTAT_2_1.py`** | Cluster & attribute discovery, read/write verification | **PASSED** |
| **`TC_HSTAT_2_2.py`** | Mode functionality, On/Off synchronization, SystemState | **PASSED** |
| **`TC_HSTAT_2_3.py`** | Setpoint functionality, min/max bounds, stepping | **PASSED** |
| **`TC_HSTAT_2_4.py`** | Optional functionality: Continuous mode, subscriptions, and test event triggers | **PASSED** |

---

## 6. How to Build & Run

### Building for Darwin / Host
```bash
scripts/run_in_build_env.sh "./scripts/build/build_examples.py --target darwin-arm64-dehumidifier-clang --quiet build"
```
Binary output: `out/darwin-arm64-dehumidifier-clang/chip-dehumidifier-app`

### Building for ESP32-S3
```bash
scripts/run_in_build_env.sh "./scripts/build/build_examples.py --target esp32-devkitc-dehumidifier --quiet build"
```
Binary output: `out/esp32-devkitc-dehumidifier/chip-dehumidifier-app.bin`

### Running the Application Standalone
```bash
./out/darwin-arm64-dehumidifier-clang/chip-dehumidifier-app --discriminator 1234 --KVS /tmp/chip_kvs1
```
