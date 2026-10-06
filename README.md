# LoRa-Telemetry

A structured LoRa telemetry system built using **STM32F439ZI**, **STM32G491RE**, and **SX1278 LoRa transceivers**.

The project develops an end-to-end telemetry pipeline from low-level SX1278 register communication and basic LoRa packet transfer to **timestamped, structure-based telemetry packets** and **UART-based JSON reception and validation**.

![STM32](https://img.shields.io/badge/STM32-F439ZI%20%7C%20G491RE-blue)
![LoRa](https://img.shields.io/badge/LoRa-SX1278-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## System Architecture

```text
                    TRANSMITTER
              ┌─────────────────────┐
              │     STM32F439ZI     │
              │                     │
              │ Telemetry Packet    │
              │ Generation          │
              └──────────┬──────────┘
                         │ SPI
                         ▼
                   ┌───────────┐
                   │  SX1278   │
                   │   LoRa    │
                   └─────┬─────┘
                         │
                      LoRa RF
                         │
                   ┌─────▼─────┐
                   │  SX1278   │
                   │   LoRa    │
                   └─────┬─────┘
                         │ SPI
                         ▼
              ┌─────────────────────┐
              │     STM32G491RE     │
              │                     │
              │ Packet Reception &  │
              │ Telemetry Parsing   │
              └──────────┬──────────┘
                         │ UART
                         ▼
                  ┌──────────────┐
                  │ JSON Output  │
                  │ & Validation │
                  └──────────────┘
```

### Data Flow

**STM32F439ZI → SPI → SX1278 → LoRa RF → SX1278 → SPI → STM32G491RE → UART → JSON**

The system separates the **wireless transport layer** from the **telemetry representation layer**, allowing compact structured data to be transmitted over LoRa and exposed in a human-readable format through UART.

---

## What Was Built

The project was developed incrementally, validating each stage of the communication stack before moving to the next.

### 1. SX1278 Register Communication

The first stage established reliable **MCU ↔ SX1278 communication over SPI**.

- Implemented SPI-based register access
- Configured SX1278 control registers
- Verified communication with the LoRa module
- Validated the SX1278 device through its version register

This established the low-level hardware interface required for all subsequent LoRa functionality.

### 2. Basic LoRa Transmission & Reception

After validating the hardware interface, the system was extended to perform actual wireless communication.

- Configured SX1278 for LoRa operation
- Implemented packet transmission
- Implemented packet reception
- Established the basic **STM32 → LoRa → STM32** communication path
- Used the SX1278 event interface for TX/RX handling

### 3. Timestamp Transmission & Reception

The telemetry pipeline was then extended with timestamp information.

This allows transmitted telemetry to retain timing information across the wireless link and provides a foundation for measuring and reasoning about telemetry events.

### 4. Structure-Based Telemetry Packets

Instead of transmitting isolated values, telemetry was organized into a dedicated **structure-based packet**.

This provides:

- A defined telemetry data model
- Consistent field ordering
- Easier packet construction on the transmitter
- Predictable packet interpretation on the receiver
- A cleaner foundation for extending telemetry fields

The structure-based packet represents the core data exchanged between the two embedded systems.

### 5. UART-Based JSON Telemetry

The receiver was subsequently extended to expose the received telemetry through UART in a structured JSON representation.

This provides an interface between the embedded telemetry system and higher-level software such as:

- Serial monitoring tools
- Ground-station applications
- Data logging systems
- Python or other host-side processing

The UART stage also includes telemetry reception and validation.

---

## Engineering Highlights

- **Dual-MCU architecture** using STM32F439ZI and STM32G491RE
- Low-level **SPI communication** with SX1278
- SX1278 register configuration and device validation
- **433 MHz LoRa** communication
- Wireless packet TX/RX
- DIO0-based LoRa event handling
- Timestamp-aware telemetry
- Structure-based binary telemetry packets
- UART-based telemetry reception
- Structured JSON telemetry representation
- STM32 HAL-based embedded firmware
- Incremental hardware and protocol validation

---

## Hardware

| Component | Role |
|---|---|
| STM32F439ZI | Telemetry transmitter |
| STM32G491RE | Telemetry receiver |
| SX1278 ×2 | LoRa transceivers |
| SPI | MCU ↔ SX1278 communication |
| UART | Receiver telemetry interface |
| ST-Link | Programming & debugging |

### SX1278 Pinout

| SX1278 | STM32 | Function |
|---|---|---|
| SCK | PA5 | SPI Clock |
| MISO | PA6 | SPI Data |
| MOSI | PA7 | SPI Data |
| NSS | PA4 | Chip Select |
| RESET | PB0 | Module Reset |
| DIO0 | PB1 | TX/RX Event |
| VCC | 3.3V | Power |
| GND | GND | Ground |

> The project's `.ioc` configuration should be treated as the final authority if pin assignments differ from this documentation.

---

## Telemetry Format

The telemetry packet contains information such as timestamp, vehicle identification, inertial measurements, GPS position, altitude, temperature, and battery level.

Example JSON representation:

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

The example is available in [`telemetry.json`](telemetry.json).

| Field | Description |
|---|---|
| `timestamp` | Telemetry timestamp |
| `uav_id` | Vehicle identifier |
| `accel_x/y/z` | Accelerometer measurements |
| `gyro_x/y/z` | Gyroscope measurements |
| `latitude` | GPS latitude |
| `longitude` | GPS longitude |
| `altitude` | Altitude |
| `temperature` | Temperature |
| `battery` | Battery level |

> `telemetry.json` represents the reference telemetry format; it is not a formal JSON Schema.

---

## Key Engineering Challenges

### Reliable SX1278 Interface

Before implementing wireless communication, the MCU must reliably communicate with the SX1278.

**Approach:** Implemented SPI-based register communication and validated the module before proceeding to higher-level LoRa functionality.

### Building the Communication Stack Incrementally

Rather than implementing the complete telemetry pipeline at once, the project was developed in stages:

```text
Register Communication
        ↓
Basic LoRa TX/RX
        ↓
Timestamp Transfer
        ↓
Structured Telemetry Packet
        ↓
UART JSON Reception & Validation
```

Each stage builds upon the previous validated layer, simplifying hardware debugging and isolating communication issues.

### Bridging Embedded Data and Host Software

Raw embedded telemetry is not convenient for external applications.

**Approach:** The receiver exposes telemetry through UART using a structured JSON representation, creating a simple interface between the embedded system and host-side software.

---

## Firmware & Tools

### Development Tools

- STM32CubeIDE / STM32CubeMX
- STM32 HAL
- STM32CubeProgrammer
- Keil MDK
- ST-Link

### MCU Peripherals

| Peripheral | Purpose |
|---|---|
| SPI | SX1278 communication |
| UART | Telemetry output |
| GPIO | NSS, RESET and DIO0 |
| HAL timing | Delays and timestamps |

---

## Repository Structure

```text
LoRa-Telemetry/
├── LoRa_Transmit_F439ZI/     # STM32F439ZI transmitter firmware
├── LoRa_Receiver_G491RE/     # STM32G491RE receiver firmware
├── docs/                     # Project documentation
├── telemetry.json            # Example telemetry format
└── README.md
```

---

## Build & Flash

### Transmitter

1. Open `LoRa_Transmit_F439ZI` in STM32CubeIDE.
2. Verify the `.ioc` configuration.
3. Build the firmware.
4. Connect the STM32F439ZI through ST-Link.
5. Flash using STM32CubeIDE or STM32CubeProgrammer.

### Receiver

1. Open `LoRa_Receiver_G491RE`.
2. Verify SPI, UART and GPIO configuration.
3. Build the firmware.
4. Connect the STM32G491RE through ST-Link.
5. Flash the receiver firmware.
6. Connect the receiver UART to a serial terminal.

---

## Project Outcome

The project demonstrates a complete embedded telemetry pipeline:

**Low-Level Hardware Communication → Wireless Data Link → Structured Telemetry → Host-Readable JSON**

The implementation covers multiple layers of embedded development, including **STM32 peripheral configuration, SPI communication, LoRa transceiver control, packet design, wireless TX/RX, timestamp handling, UART communication, and telemetry validation**.

---

## License

This project is licensed under the **MIT License**.

See [`LICENSE`](LICENSE) for details.

---

## Developer

### Amrutha Pradeep

**ECE Final Year — SVNIT Surat**

📧 **amruthapradeep2004@gmail.com**

---

## Acknowledgments

- **STMicroelectronics** — STM32 platform and HAL
- **Semtech** — SX1278 LoRa technology
- **STM32CubeIDE / STM32CubeMX** — Development and configuration tools
