# LoRa-Telemetry

[![Build Status](https://img.shields.io/badge/build-TBD-lightgrey)](#)
[![License](https://img.shields.io/badge/license-TBD-lightgrey)](#license)
[![STM32](https://img.shields.io/badge/MCU-STM32-blue)](#hardware-architecture)
[![LoRa](https://img.shields.io/badge/Wireless-LoRa-orange)](#hardware-architecture)

> **STM32-based long-range UAV telemetry system using SX1278 LoRa transceivers**

LoRa-Telemetry is an embedded UAV telemetry platform built around an **STM32F439ZI transmitter**, **STM32G491RE receiver**, and **SX1278 / Ai-Thinker RA-02 LoRa modules**.

The current implementation establishes the core wireless telemetry path:

```text
STM32F439ZI
     │
     │ SPI
     ▼
 SX1278 TX
     │
     │ LoRa RF (~433 MHz)
     ▼
 SX1278 RX
     │
     │ SPI
     ▼
STM32G491RE
     │
     │ UART
     ▼
PC / Terminal / Ground Station
```

The project has progressed through SX1278 register validation, basic LoRa transmission/reception, UART debugging, timestamp experiments using `HAL_GetTick()`, and JSON-based telemetry testing.

The broader project is intended to evolve toward **battery monitoring, communication-loss detection, emergency power management, and eventual MAVLink integration**.

> **Project status:** Active academic/research embedded-systems project. Features such as autonomous RTL/LAND, backup-power switching, production packet serialization, and a complete ground station are future development targets unless explicitly implemented in the current firmware.

---

## Table of Contents

- [Features](#features)
- [Hardware Architecture](#hardware-architecture)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Pinout and Wiring](#pinout-and-wiring)
- [Power Requirements](#power-requirements)
- [Repository Structure](#repository-structure)
- [Software Stack](#software-stack)
- [Firmware Configuration](#firmware-configuration)
- [Packet Structure and Telemetry](#packet-structure-and-telemetry)
- [JSON Telemetry Layout](#json-telemetry-layout)
- [Binary vs JSON](#binary-vs-json)
- [Build and Flash](#build-and-flash)
- [UART Monitoring](#uart-monitoring)
- [LoRa Initialization Flow](#lora-initialization-flow)
- [Hardware Debugging](#hardware-debugging)
- [Current Development Status](#current-development-status)
- [Future Development](#future-development)
- [Safety Considerations](#safety-considerations)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Features

### Currently Implemented / Demonstrated

- STM32F439ZI transmitter firmware
- STM32G491RE receiver firmware
- SX1278 / RA-02 interfacing
- SPI communication with SX1278
- NSS / chip-select control
- Hardware reset control
- DIO0 event/interrupt connection
- Custom STM32 HAL-based LoRa driver
- SX1278 register read/write operations
- SX1278 initialization
- 433 MHz initialization
- Basic LoRa transmission
- Basic LoRa reception
- Simple packet testing
- UART debugging
- Timestamp testing using `HAL_GetTick()`
- JSON telemetry testing
- Character-by-character UART reception
- Human-readable telemetry representation through `telemetry.json`

### Planned / Future

- Explicit binary packet serialization
- Packet framing
- CRC
- Sequence-number-based packet-loss detection
- Communication watchdog
- Battery voltage/current monitoring
- Emergency power management
- Emergency state machine
- MAVLink integration
- RTL / LAND decision logic
- Ground-station dashboard
- Long-range RF characterization

---

# Hardware Architecture

## System Overview

The telemetry system consists of two independent embedded nodes.

### Transmitter — UAV Side

The **STM32F439ZI** acts as the UAV-side telemetry controller.

Responsibilities include:

- Preparing telemetry information
- Building telemetry packets
- Communicating with the SX1278 over SPI
- Transmitting telemetry over LoRa
- Providing UART-based debugging and telemetry output

### Receiver — Ground Side

The **STM32G491RE** acts as the ground-side receiver.

Responsibilities include:

- Communicating with the SX1278 over SPI
- Receiving LoRa packets
- Processing received telemetry
- Forwarding telemetry/debug information over UART

---

## System Architecture

```text
                         UAV / TRANSMITTER
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│                  UAV Telemetry / Sensors                   │
│                           │                                 │
│                           ▼                                 │
│                  ┌─────────────────┐                        │
│                  │   STM32F439ZI   │                        │
│                  │   TRANSMITTER   │                        │
│                  └────────┬────────┘                        │
│                           │ SPI                             │
│                           ▼                                 │
│                  ┌─────────────────┐                        │
│                  │  SX1278 / RA-02  │                       │
│                  │       TX         │                       │
│                  └────────┬────────┘                        │
└───────────────────────────┼─────────────────────────────────┘
                            │
                            │ LoRa RF
                            │ ~433 MHz
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                         RECEIVER                            │
│                                                             │
│                  ┌─────────────────┐                        │
│                  │  SX1278 / RA-02  │                       │
│                  │       RX         │                       │
│                  └────────┬────────┘                        │
│                           │ SPI                             │
│                           ▼                                 │
│                  ┌─────────────────┐                        │
│                  │   STM32G491RE   │                        │
│                  │    RECEIVER     │                        │
│                  └────────┬────────┘                        │
│                           │ UART                            │
│                           ▼                                 │
│                  ┌─────────────────┐                        │
│                  │ PC / Terminal / │                        │
│                  │ Ground Station  │                        │
│                  └─────────────────┘                        │
└─────────────────────────────────────────────────────────────┘
```

---

# Hardware Components

| Component | Role |
|---|---|
| **STM32F439ZI** | UAV-side transmitter MCU |
| **STM32G491RE** | Ground-side receiver MCU |
| **SX1278 / Ai-Thinker RA-02 ×2** | LoRa transceivers |
| **ST-LINK** | Programming/debugging |
| **3.3 V supply** | SX1278 power |
| **USB-UART / onboard UART interface** | Serial monitoring |

### Communication Interfaces

| Interface | Purpose |
|---|---|
| SPI | MCU ↔ SX1278 communication |
| GPIO | NSS and RESET control |
| EXTI / GPIO | DIO0 LoRa event handling |
| UART | Debugging, telemetry and JSON testing |
| LoRa RF | Wireless TX/RX link |

---

# Pinout and Wiring

The known working/development pin mapping is shared between the two MCU targets.

> **Important:** The current `.ioc` files should be treated as authoritative if they differ from this documentation.

## SX1278 ↔ STM32 Pin Mapping

| SX1278 / RA-02 | STM32F439ZI | STM32G491RE | Function |
|---|---:|---:|---|
| **SCK** | PA5 | PA5 | SPI clock |
| **MISO** | PA6 | PA6 | SPI MISO |
| **MOSI** | PA7 | PA7 | SPI MOSI |
| **NSS / CS** | PA4 | PA4 | Chip select |
| **RESET** | PB0 | PB0 | Hardware reset |
| **DIO0** | PB1 | PB1 | LoRa interrupt/event |
| **VCC** | 3.3 V | 3.3 V | Module power |
| **GND** | GND | GND | Common ground |

### Wiring Diagram

```text
STM32                         SX1278 / RA-02
──────────────────────────────────────────────

PA5  ───────────────────────► SCK
PA6  ◄─────────────────────── MISO
PA7  ───────────────────────► MOSI
PA4  ───────────────────────► NSS / CS

PB0  ───────────────────────► RESET
PB1  ◄─────────────────────── DIO0

3.3V ───────────────────────► VCC
GND  ───────────────────────► GND
```

---

## UART Mapping

The UART mapping used during later development/testing was:

| STM32 Pin | Function |
|---|---|
| **PD8** | UART TX |
| **PD9** | UART RX |

The UART peripheral instance and alternate-function configuration should be verified from the current STM32CubeMX `.ioc` file before modifying firmware.

---

# Power Requirements

The SX1278 / RA-02 is a **3.3 V device**.

```text
SX1278 VCC  →  3.3 V
SX1278 GND  →  STM32 GND
```

### Important

- Do **not** connect the SX1278 directly to an inappropriate 5 V supply.
- Use a suitable regulated 3.3 V supply.
- Ensure a common ground between the STM32 and LoRa module.
- For a future custom PCB, power integrity should be considered carefully because LoRa transmission can create dynamic current demand.

---

# Repository Structure

```text
LoRa-Telemetry/
│
├── LoRa_Transmit_F439ZI/
│   ├── Core/
│   │   ├── Inc/
│   │   └── Src/
│   ├── Drivers/
│   ├── LoRa driver source/header files
│   ├── STM32CubeMX .ioc
│   └── project files
│
├── LoRa_Receiver_G491RE/
│   ├── Core/
│   │   ├── Inc/
│   │   └── Src/
│   ├── Drivers/
│   ├── LoRa driver source/header files
│   ├── STM32CubeMX .ioc
│   └── project files
│
├── docs/
│   ├── documentation
│   ├── hardware schematics
│   └── project material
│
├── telemetry.json
│
└── README.md
```

The repository currently separates the transmitter and receiver firmware into independent projects because the two nodes use different STM32 MCU families.

---

# Software Stack

## Development Tools

- **STM32CubeIDE**
- **STM32CubeMX**
- **STM32 HAL**
- **Embedded C**
- **Git / GitHub**

## Programming and Debugging

- ST-LINK
- STM32CubeProgrammer

## Serial Monitoring

- PuTTY
- Tera Term
- STM32CubeIDE serial tools

## Optional Hardware Debugging

- Logic analyzer
- Oscilloscope

---

# Firmware Configuration

Each MCU project contains its own STM32CubeMX `.ioc` configuration.

The `.ioc` file defines:

- MCU selection
- Clock configuration
- GPIO
- SPI
- UART
- EXTI / NVIC
- Pin assignments
- Peripheral configuration

## Recommended Peripheral Configuration

### SPI

The SX1278 communicates with the MCU using SPI.

```text
STM32 ↔ SX1278

SCK
MISO
MOSI
NSS
```

SPI is used for:

- Register reads
- Register writes
- SX1278 configuration
- FIFO access
- TX operations
- RX operations

### GPIO

GPIO is used for:

- NSS / CS
- SX1278 RESET

### DIO0 / EXTI

DIO0 is connected to a GPIO interrupt-capable pin.

Depending on the configured SX1278 interrupt mapping, DIO0 can be used to indicate events such as:

- `TxDone`
- `RxDone`

### UART

UART is used for:

- Debug output
- Telemetry display
- Test input
- JSON testing
- Timestamp output

---

# Clock Configuration

Clock configuration is MCU-specific and should be taken from the corresponding `.ioc` project.

> **Do not copy the transmitter `.ioc` configuration into the receiver project or vice versa.**

The STM32F439ZI and STM32G491RE have different clock architectures, peripheral implementations, alternate functions, and startup configurations.

---

# Custom LoRa Driver

The project uses a custom STM32 LoRa driver built around STM32 HAL SPI rather than the Arduino LoRa library.

Conceptually:

```text
STM32 HAL SPI
      │
      ▼
SX1278 Register Access
      │
      ├── Configuration
      ├── Mode Control
      ├── FIFO
      ├── TX
      └── RX
```

Functions used during development include:

```c
LoRa_Reset();

LoRa_ReadRegister();

LoRa_WriteRegister();

LoRa_Init(433000000);

LoRa_Send();

LoRa_Receive();
```

---

# SX1278 Initialization

The basic initialization flow is:

```text
MCU Reset
    │
    ▼
HAL Initialization
    │
    ▼
System Clock
    │
    ▼
GPIO Initialization
    │
    ▼
SPI Initialization
    │
    ▼
UART Initialization
    │
    ▼
LoRa_Reset()
    │
    ▼
Read SX1278 Version
    │
    ▼
LoRa_Init(433000000)
    │
    ▼
Configure LoRa
    │
    ▼
Ready for TX / RX
```

The project development configuration uses approximately **433 MHz**:

```c
LoRa_Init(433000000);
```

> Other LoRa parameters such as spreading factor, bandwidth, coding rate, preamble length and TX power should not be assumed from this README. Verify the current firmware configuration.

---

# Packet Structure and Telemetry

The telemetry layer is designed around structured data.

A representative telemetry structure discussed during development is:

```c
typedef struct
{
    uint32_t timestamp;

    float altitude;
    float latitude;
    float longitude;

    float battery_voltage;
    float battery_current;

    uint8_t battery_percentage;
    uint8_t status;

} TelemetryPacket;
```

> **Important:** This is a representative telemetry design. The exact structure currently implemented in firmware should be confirmed from the source code before treating this definition as the production wire format.

---

## Binary Telemetry Flow

The intended architecture is:

```text
Telemetry Data
      │
      ▼
C Telemetry Structure
      │
      ├───────────────────┐
      ▼                   ▼
Binary Encoding          JSON
      │                   │
      ▼                   ▼
   LoRa RF             UART / PC
```

Binary telemetry is preferred for the RF link because LoRa has limited bandwidth and packet payload efficiency is important.

---

# JSON Telemetry Layout

The repository contains:

```text
telemetry.json
```

The current file is a **human-readable telemetry example/test representation**, rather than a formal JSON Schema.

The current repository example contains fields such as:

```json
{
  "timestamp": "2026-06-13T10:30:00Z",
  "uav_id": "UAV_001",
  "accel_x": 1.23,
  "accel_y": -0.56,
  "accel_z": 9.81,
  "gyro_x": 0.15,
  "gyro_y": -0.08,
  "gyro_z": 0.22,
  "latitude": 21.1702,
  "longitude": 72.8311,
  "altitude": 120.5,
  "temperature": 29.4,
  "battery": 87
}
```

This representation provides a convenient way to visualize telemetry information on a PC or during UART-based testing.

### Field Reference

| Field | Example | Description |
|---|---:|---|
| `timestamp` | `"2026-06-13T10:30:00Z"` | Telemetry timestamp |
| `uav_id` | `"UAV_001"` | UAV identifier |
| `accel_x` | `1.23` | X-axis acceleration |
| `accel_y` | `-0.56` | Y-axis acceleration |
| `accel_z` | `9.81` | Z-axis acceleration |
| `gyro_x` | `0.15` | X-axis angular velocity |
| `gyro_y` | `-0.08` | Y-axis angular velocity |
| `gyro_z` | `0.22` | Z-axis angular velocity |
| `latitude` | `21.1702` | Latitude |
| `longitude` | `72.8311` | Longitude |
| `altitude` | `120.5` | Altitude |
| `temperature` | `29.4` | Temperature |
| `battery` | `87` | Battery percentage/state |

> The committed `telemetry.json` should take precedence if this README and the file diverge.

---

# UART JSON Reception

UART was also tested using character-by-character reception.

A simplified pattern is:

```c
char ch;

HAL_UART_Receive(
    &huart3,
    (uint8_t *)&ch,
    1,
    HAL_MAX_DELAY
);
```

Received characters can be accumulated into a buffer.

During development testing, the closing brace:

```text
}
```

was used as a simple end-of-message condition.

For example:

```json
{
    "battery": 78
}
```

Once `}` is received, the accumulated characters can be treated as one JSON message.

### Production Recommendation

Using `}` alone as a framing delimiter is suitable only for a controlled test.

A production UART framing protocol should use explicit framing such as:

```text
START | LENGTH | PAYLOAD | CRC
```

This avoids ambiguity caused by:

- Nested JSON objects
- Multiple consecutive messages
- Corrupted streams
- Missing characters
- Buffer overflow
- Resynchronization problems

---

# Binary vs JSON

## JSON

JSON is useful for:

- Human-readable telemetry
- Debugging
- PC applications
- Ground-station APIs
- Logging
- Development

## Binary

Binary packets are preferred for the LoRa RF link because they provide:

- Smaller packet sizes
- Lower bandwidth usage
- Deterministic payload sizes
- Efficient MCU processing
- Better suitability for constrained wireless links

Therefore, the intended design is:

```text
              Telemetry
                  │
                  ▼
          C Telemetry Structure
             │          │
             │          │
             ▼          ▼
      Binary Encode    JSON
             │          │
             ▼          ▼
           LoRa      UART / PC
```

---

# Struct Serialization

A production implementation should **not blindly transmit a C struct using `sizeof()`** as the RF protocol.

For example:

```c
LoRa_Send(
    (uint8_t *)&packet,
    sizeof(packet)
);
```

can introduce compatibility problems caused by:

- Compiler padding
- Memory alignment
- Endianness
- Compiler differences
- Floating-point representation
- Future structure changes

A dedicated serializer/deserializer is preferable.

### Recommended API

```c
int Telemetry_Encode(
    const TelemetryPacket *packet,
    uint8_t *buffer,
    uint16_t buffer_size
);
```

and:

```c
int Telemetry_Decode(
    const uint8_t *buffer,
    uint16_t length,
    TelemetryPacket *packet
);
```

This keeps the RF protocol independent from compiler-specific memory layout.

---

# Recommended Future Packet Format

A robust future packet format can follow:

```text
┌────────┬─────────┬────────┬────────┬───────────────┬─────┐
│ START  │ VERSION │ TYPE   │ LENGTH │ PAYLOAD       │ CRC │
├────────┼─────────┼────────┼────────┼───────────────┼─────┤
│ 1 byte │ 1 byte  │ 1 byte │ 2 byte │ N bytes       │ 2/4 │
└────────┴─────────┴────────┴────────┴───────────────┴─────┘
```

Potential fields:

- Start-of-frame
- Protocol version
- Message type
- Payload length
- Sequence number
- Timestamp
- Telemetry payload
- CRC

A future telemetry structure could be:

```c
typedef struct
{
    uint8_t  version;
    uint8_t  message_type;
    uint16_t sequence;
    uint32_t timestamp;

    float altitude;
    float latitude;
    float longitude;

    float battery_voltage;
    float battery_current;

    uint8_t battery_percentage;
    uint8_t status;

    uint16_t crc;

} TelemetryPacket;
```

> This is a recommended future protocol design and is **not a claim that this exact packet format is currently implemented**.

---

# Build and Flash

## Prerequisites

Install:

1. STM32CubeIDE
2. STM32CubeMX if independent `.ioc` configuration is required
3. STM32CubeProgrammer
4. ST-LINK drivers
5. A suitable USB/UART terminal if serial monitoring is required

---

## Option 1 — Build and Flash with STM32CubeIDE

### Transmitter

1. Connect the **STM32F439ZI** development board through ST-LINK.
2. Open STM32CubeIDE.
3. Import/open:

```text
LoRa_Transmit_F439ZI/
```

4. Verify that the F439ZI `.ioc` file is selected.
5. Build the project:

```text
Project → Build Project
```

6. Start a debug/flash session:

```text
Run → Debug
```

or:

```text
Run → Run
```

7. Select the connected ST-LINK target if prompted.
8. Program the STM32F439ZI.
9. Reset the board.
10. Open the configured UART terminal.

---

### Receiver

Repeat the same procedure for:

```text
LoRa_Receiver_G491RE/
```

but ensure that the **STM32G491RE `.ioc` configuration** is used.

> Never replace the F439ZI `.ioc` with the G491RE `.ioc`, or vice versa.

---

# Flashing with STM32CubeProgrammer

STM32CubeProgrammer can also be used independently of STM32CubeIDE.

### Procedure

1. Connect the target MCU through ST-LINK.
2. Open STM32CubeProgrammer.
3. Select the appropriate ST-LINK connection.
4. Connect to the MCU.
5. Select the generated firmware binary/HEX file.
6. Program the device.
7. Verify the programming operation.
8. Reset the target.
9. Start the UART terminal if required.

Repeat for both boards.

---

# Running the System

The recommended startup sequence is:

### 1. Power the Receiver

```text
STM32G491RE
      │
      ▼
SX1278 RX
```

### 2. Power the Transmitter

```text
STM32F439ZI
      │
      ▼
SX1278 TX
```

### 3. Verify SX1278 Communication

The SX1278 version register is:

```text
Register: 0x42
Expected value: 0x12
```

A successful read of `0x12` provides strong evidence that the MCU is communicating correctly with the SX1278 over SPI.

### 4. Perform Basic RF Test

Start with a simple message:

```text
HELLO
```

### 5. Verify UART Output

Confirm that the receiver reports the received packet through UART.

### 6. Move to Structured Telemetry

Only after the basic RF link works reliably should structured telemetry be introduced.

---

# UART Monitoring

A serial terminal such as **Tera Term** or **PuTTY** can be used to inspect UART output.

Typical uses include:

- Debug messages
- Received LoRa payloads
- Telemetry values
- JSON testing
- Timestamp output
- Communication diagnostics

### Tera Term Local Echo

During UART testing, remember:

> **Terminal display ≠ MCU reception**

Local echo only determines whether characters typed into the terminal are displayed locally.

To prove that the STM32 actually received data, the firmware should process the received bytes and generate an MCU-side response or diagnostic output.

---

# Timestamp and Latency Testing

The project uses:

```c
HAL_GetTick()
```

for millisecond-scale timing experiments.

A conceptual transmitter timestamp is:

```c
uint32_t txTime = HAL_GetTick();
```

and the receiver can obtain:

```c
uint32_t rxTime = HAL_GetTick();
```

Then:

```text
elapsed = rxTime - txTime
```

can be used as an approximate elapsed-time measurement.

### Important Limitation

This does **not** represent pure RF propagation delay.

The measured interval can include:

- Packet preparation
- SPI FIFO writes
- SX1278 processing
- RF transmission
- SX1278 reception
- SPI reads
- Software processing
- UART processing
- Timestamp placement

For a more rigorous latency measurement:

```text
T0 = timestamp at precisely defined TX event
T1 = timestamp at precisely defined RX event

Latency = T1 - T0
```

---

# Hardware Debugging

Always debug from the physical layer upward.

## Step 1 — Power

```text
[ ] SX1278 powered at 3.3 V
[ ] Common ground
[ ] No inappropriate 5 V connection
```

## Step 2 — SPI

```text
[ ] PA5 = SCK
[ ] PA6 = MISO
[ ] PA7 = MOSI
[ ] PA4 = NSS
```

## Step 3 — Control Pins

```text
[ ] PB0 = RESET
[ ] PB1 = DIO0
```

## Step 4 — Verify SX1278 Version

Read:

```text
Register = 0x42
```

Expected:

```text
0x12
```

## Step 5 — Basic RF Test

Transmit:

```text
HELLO
```

## Step 6 — Structured Telemetry

Only after the basic RF path is working.

---

# Debugging Decision Tree

```text
                 LoRa not working
                       │
                       ▼
             Can read register 0x42?
                  /          \
                NO            YES
                │              │
                ▼              ▼
          Check power      Check LoRa
          and SPI          configuration
                              │
                              ▼
                         Can TX send?
                          /       \
                        NO         YES
                        │           │
                        ▼           ▼
                    Check FIFO   Can RX receive?
                                  /      \
                                NO        YES
                                │          │
                                ▼          ▼
                           Check DIO0    Basic link OK
                           and RX mode
```

---

# Current Development Status

| Capability | Status |
|---|---|
| STM32F439ZI transmitter | Implemented / demonstrated |
| STM32G491RE receiver | Implemented / demonstrated |
| SX1278 interfacing | Implemented / demonstrated |
| SPI communication | Implemented / demonstrated |
| NSS control | Implemented / demonstrated |
| SX1278 reset | Implemented / demonstrated |
| DIO0 connection | Implemented / demonstrated |
| Custom LoRa driver | Implemented / demonstrated |
| SX1278 register access | Implemented / demonstrated |
| SX1278 initialization | Implemented / demonstrated |
| 433 MHz initialization | Implemented / demonstrated |
| Basic LoRa TX | Implemented / demonstrated |
| Basic LoRa RX | Implemented / demonstrated |
| UART debugging | Implemented / demonstrated |
| Timestamp testing | Implemented / demonstrated |
| JSON telemetry testing | Implemented / demonstrated |
| `telemetry.json` representation | Present |
| Production binary serialization | Future |
| CRC | Future |
| Sequence numbers | Future |
| Packet-loss detection | Future |
| Complete battery measurement hardware | Future |
| Automatic backup-power switching | Future |
| MAVLink RTL | Future |
| MAVLink LAND | Future |
| Autonomous emergency decision engine | Future |
| Production ground station | Future |
| Long-range characterization | Future |

---

# Future Development

The recommended development order is:

## Phase 1 — Freeze Existing LoRa Driver

Document:

- SX1278 registers
- Frequency
- Modulation configuration
- FIFO behavior
- Interrupt configuration

## Phase 2 — Define Packet Protocol

Define:

- Header
- Protocol version
- Message type
- Length
- Sequence number
- Timestamp
- Payload
- CRC

## Phase 3 — Serialization

Implement explicit:

```text
Telemetry_Encode()
Telemetry_Decode()
```

## Phase 4 — Reliability

Add:

- Sequence numbers
- CRC
- Packet-loss tracking
- Communication timeout
- Watchdog logic

## Phase 5 — RF Characterization

Measure:

- Range
- RSSI
- SNR
- Latency
- Packet loss

## Phase 6 — Battery Monitoring

Add:

- Battery voltage
- Battery current
- Battery percentage/state
- Power-state detection

## Phase 7 — Emergency State Machine

Potential states:

```text
NORMAL
WARNING
CRITICAL
LINK_LOST
POWER_FAULT
```

## Phase 8 — Backup Power

Design and validate emergency supply switching.

## Phase 9 — MAVLink

Integrate with the flight controller only after the telemetry and decision layers are reliable.

## Phase 10 — Ground Station

Build a dedicated monitoring interface for:

- Telemetry
- Battery state
- Link status
- RSSI
- SNR
- Alerts
- Logging

---

# Battery and Emergency Management

Battery monitoring is a major objective of the broader project.

Potential telemetry parameters include:

```text
Battery Voltage
Battery Current
Battery Percentage
Power Consumption
Battery Health / Status
```

Possible battery states:

```text
NORMAL
WARNING
CRITICAL
```

Potential emergency conditions include:

```text
Low Battery
Critical Battery
Communication Failure
Primary Power Failure
```

A future communication watchdog can monitor the time since the last valid telemetry packet:

```text
Last valid packet
        │
        ▼
    Reset timer
        │
        ▼
Wait for next packet
        │
        ├──────────────► Packet received
        │                    │
        │                    ▼
        │               Reset timer
        │
        └──────────────► Timeout
                             │
                             ▼
                         LINK_LOST
```

---

# MAVLink Integration

A major future objective is integration with **MAVLink**.

The intended high-level flow is:

```text
Telemetry
    │
    ▼
Condition Monitoring
    │
    ▼
Emergency Decision
    │
    ├───────────────┐
    ▼               ▼
   RTL             LAND
    │               │
    └───────┬───────┘
            ▼
      Flight Controller
```

Potential triggers include:

- Critical battery
- Link loss
- Power failure

MAVLink functionality should only be introduced after the telemetry and decision layers have been thoroughly validated.

---

# Safety Considerations

This project is intended for embedded/UAV research and development.

Automatic flight-control actions such as **RTL** and **LAND** are safety-critical.

Before using any emergency-control functionality on an actual aircraft:

1. Validate telemetry integrity.
2. Validate battery measurements.
3. Use threshold hysteresis.
4. Avoid decisions based on a single noisy sample.
5. Detect stale telemetry packets.
6. Validate MAVLink messages.
7. Provide manual override.
8. Test without propellers.
9. Perform controlled ground tests.
10. Test failure and recovery scenarios.

The system should never transition directly from:

```text
LoRa packet received
        │
        ▼
   AUTOMATIC RTL
```

without intermediate validation, filtering, persistence checks, telemetry freshness checks, timeout logic, and safety overrides.

---

# What This Project Is Not Yet

The current project should **not** be described as:

- A production-certified UAV telemetry system
- A certified aviation safety system
- A complete autonomous flight-control system
- A complete MAVLink autopilot
- A completed backup-power PCB
- A commercial LoRa network
- A fully validated autonomous emergency system

It is an:

> **Academic/research embedded-systems project under active development.**

---

# Quick Reference

| Category | Detail |
|---|---|
| Project | LoRa-Telemetry |
| Application | UAV telemetry |
| TX MCU | STM32F439ZI |
| RX MCU | STM32G491RE |
| LoRa IC | SX1278 |
| LoRa Module | Ai-Thinker RA-02 |
| Frequency | ~433 MHz |
| MCU ↔ LoRa | SPI |
| SCK | PA5 |
| MISO | PA6 |
| MOSI | PA7 |
| NSS | PA4 |
| RESET | PB0 |
| DIO0 | PB1 |
| UART TX | PD8 |
| UART RX | PD9 |
| UART Purpose | Debug / telemetry / JSON |
| Driver | Custom STM32 HAL-based LoRa driver |
| Timestamp | `HAL_GetTick()` |
| JSON | `telemetry.json` |
| Serial Tools | PuTTY / Tera Term |
| IDE | STM32CubeIDE |
| Configuration | STM32CubeMX `.ioc` |
| Programmer | STM32CubeProgrammer / ST-LINK |
| TX Directory | `LoRa_Transmit_F439ZI` |
| RX Directory | `LoRa_Receiver_G491RE` |
| Documentation | `docs` |
| Future | Battery monitoring, emergency power, MAVLink |
| Status | Active academic development |

---

# License

No explicit open-source license is currently specified in the repository.

Until a license is added, the repository contents should be treated as **all rights reserved** by default.

If this project is intended to be open source, add an appropriate license file such as:

```text
LICENSE
```

and update the badge and this section accordingly.

---

# Acknowledgments

This project builds upon the following technologies and ecosystems:

- **STMicroelectronics STM32** microcontrollers
- **STM32CubeIDE**
- **STM32CubeMX**
- **STM32 HAL**
- **Semtech SX1278 LoRa technology**
- **Ai-Thinker RA-02**
- **ST-LINK / STM32CubeProgrammer**
- **GitHub**

---

## Project Repository

**LoRa-Telemetry**

https://github.com/Pixel2pioneer/LoRa-Telemetry

---

## Development Philosophy

The project follows a bottom-up embedded development approach:

```text
Hardware
   │
   ▼
SPI Communication
   │
   ▼
SX1278 Register Validation
   │
   ▼
Basic LoRa TX/RX
   │
   ▼
Structured Telemetry
   │
   ▼
Reliable Packet Protocol
   │
   ▼
Battery / Link Monitoring
   │
   ▼
Emergency Decision Logic
   │
   ▼
MAVLink Integration
   │
   ▼
Ground Station
