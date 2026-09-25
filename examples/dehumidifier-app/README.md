# Matter Dehumidifier Example Application

This example demonstrates the implementation of a Matter Dehumidifier device. The posix implementation runs on both macOS (Darwin) and Linux.

---

## Prerequisites

Before building, ensure that submodules are updated and the Matter build environment is activated.

From the repository root:

```bash
source scripts/activate.sh
```

*(Alternatively, any command can be executed inside the build environment without manual shell activation by prefixing with `scripts/run_in_build_env.sh`).*

---

## Building on macOS (Darwin)

### Method 1: Using `build_examples.py` (Recommended)

Matter provides `build_examples.py` to configure and build targets consistently.

#### Apple Silicon (M1/M2/M3/M4):

```bash
./scripts/build/build_examples.py --target darwin-arm64-dehumidifier build
```

The compiled binary will be located at:
```
./out/darwin-arm64-dehumidifier/chip-dehumidifier-app
```

#### Intel Mac (x64):

```bash
./scripts/build/build_examples.py --target darwin-x64-dehumidifier build
```

The compiled binary will be located at:
```
./out/darwin-x64-dehumidifier/chip-dehumidifier-app
```

---

### Method 2: Building Directly with GN and Ninja

You can also run GN and Ninja directly using the POSIX build root:

```bash
gn gen --root=examples/dehumidifier-app/posix out/dehumidifier-app
ninja -C out/dehumidifier-app
```

The compiled binary will be located at:
```
./out/dehumidifier-app/chip-dehumidifier-app
```

---

## Alternative: Dehumidifier via `all-devices-app`

The code-driven `all-devices-app` also supports the Dehumidifier device type and can be built for macOS in two ways:

1. **Single-device subset target**:
   ```bash
   ./scripts/build/build_examples.py --target darwin-arm64-all-devices-dehumidifier build
   # Binary output: ./out/darwin-arm64-all-devices-dehumidifier/example-device-app
   ```

2. **Dynamic multi-device app**:
   ```bash
   ./scripts/build/build_examples.py --target darwin-arm64-all-devices build
   # Run with dehumidifier endpoint:
   ./out/darwin-arm64-all-devices/all-devices-app --device dehumidifier
   ```

---

## Running the Application

To run the built standalone dehumidifier app:

```bash
./out/darwin-arm64-dehumidifier/chip-dehumidifier-app
```

### Common Command-Line Options

| Option | Description |
|---|---|
| `--discriminator <value>` | 12-bit commissioning discriminator (e.g. `3840`) |
| `--passcode <value>` | 27-bit commissioning passcode / PIN (e.g. `20202021`) |
| `--secured-device-port <port>` | UDP port to bind for Matter secure messaging (default: 5540) |
| `--KVS <filepath>` | Path to key-value store file for persistent storage |
| `--trace-to <dest>` | Tracing destination (e.g., `json:log`, `perfetto`) |
| `--wifi` | Enable Wi-Fi management |
| `--ble-controller <id>` | Specify BLE controller ID |
| `--help` | Show complete list of supported options |

---

## Commissioning with `chip-tool`

To commission the dehumidifier using `chip-tool` over IP (on the same machine or local network):

```bash
chip-tool pairing onnetwork <node-id> 20202021
```
