# Kelbaran: Ultra-Minimal STM32F103C8T6 Radio Communication Template

**Kelbaran** is an ultra-streamlined embedded template for **STM32F103C8T6 (Blue Pill)** microcontrollers, specifically designed for wireless radio communication between two devices. This minimal template provides:

- **FreeRTOS** real-time operating system
- **CMSIS** bare-metal targeting (no HAL)
- **Ultra-minimal footprint**: Only **19,892 bytes** Flash
- **Clean modular architecture** ready for radio integration
- **Template-ready** for copy-paste usage

This template provides the absolute minimum foundation needed for radio communication projects, with all unnecessary components removed.

---

## Current Features

### Core Components (Minimal Set)
- **LEDs**: Status indicators (onboard, external, debug) - perfect for TX/RX status
- **Mode Button**: Manual input for mode switching or radio control
- **Display Support**: OLED display framework (currently disabled, easily re-enabled)
- **I2C Protocol**: For display communication when needed
- **GPIO Driver**: Low-level pin control
- **Task System**: Minimal FreeRTOS task architecture

### Software Architecture
- **Ultra-minimal task design** with FreeRTOS
- **Hub pattern**: Sensor Hub → Controller → Actuator Hub
- **Template-friendly**: No conditional compilation (#ifdef guards)
- **Radio-focused**: All unused modules removed

---

## Project Structure

```
src/
├── actuators/          # Output devices (minimal)
│   ├── klbn_actuator_hub.c  # Hub with LED control only
│   ├── klbn_led.c           # LED status indicators
│   └── klbn_oled.c          # OLED support (disabled)
├── logic/              # Control logic (minimal)
│   └── klbn_controller.c    # Simplified radio-ready controller
├── core/               # System core
│   ├── klbn_board.c         # Basic board initialization
│   ├── klbn_clock.c         # System clocks
│   ├── klbn_exti_dispatcher.c
│   ├── klbn_taskmanager.c   # Minimal task management
│   └── libc_stubs.c
├── drivers/            # Essential drivers only
│   └── klbn_gpio.c          # GPIO control
├── protocols/          # Communication protocols
│   └── klbn_i2c.c           # I2C for display (when enabled)
├── sensors/            # Input devices (minimal)
│   ├── klbn_sensor_hub.c    # Basic sensor framework
│   └── klbn_mode_button.c   # Mode button control
├── utils/              # Essential utilities
│   ├── klbn_debug.c
│   ├── klbn_delay.c
│   └── klbn_font8x8.c       # For OLED when enabled
└── kelbaran.c          # Main entry point
```

---

## Task Architecture (Ultra-Minimal)

| Task Name | Responsibility | Priority | Communication |
|-----------|---------------|----------|---------------|
| **SensorTask** | Basic sensor framework + button | Medium | Sends data via queues |
| **ControllerTask** | Processes button events, radio logic | High | Receives from sensors |
| **ActuatorTask** | Controls LEDs (and display when enabled) | High | Receives commands via queues |

## Communication Flow (Ready for Radio)

```
[Mode Button] → [Sensor Hub] → [Controller] → [Actuator Hub] → [LEDs]
                                     ↓
                              [Radio Module] ← TODO: Add here
                                     ↓
                            [Packet TX/RX] ← TODO: Implement
```

---

## Hardware Requirements (Minimal)

| Component | Description | Pins Used | Status |
|-----------|-------------|-----------|---------|
| **MCU** | STM32F103C8T6 (Blue Pill) | - | ✅ Active |
| **LEDs** | Status indicators | PC13 (onboard), PB5 (external), PC15 (debug) | ✅ Active |
| **Mode Button** | Manual input | PA4 | ✅ Active |
| **OLED Display** | SSD1306 128x64 I2C (optional) | PB6 (SCL), PB7 (SDA) | 🔶 Framework ready |

### Available Pins for Radio Module

| Pin | Available For | Suggested Use |
|-----|---------------|---------------|
| **PA8** | GPIO/Timer | Radio IRQ/Interrupt |
| **PA2** | GPIO | Radio CE (Chip Enable) |
| **PA3** | GPIO | Radio CSN (Chip Select) |
| **PA5** | SPI1_SCK | Radio SPI Clock |
| **PA6** | SPI1_MISO | Radio SPI MISO |
| **PA7** | SPI1_MOSI | Radio SPI MOSI |
| **PB0-PB4** | GPIO | Additional radio control |

---

## Getting Started

### Prerequisites

```bash
# Fedora/RHEL
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib

# Ubuntu/Debian
sudo apt install gcc-arm-none-eabi

# Verify installation
arm-none-eabi-gcc --version
```

### Build and Flash

```bash
# Clone and build
git clone https://github.com/msoodb/kelbaran.git
cd kelbaran
make

# Flash to device (multiple methods available)
make flash                    # Default: ST-Link
make METHOD=openocd flash     # OpenOCD
make METHOD=dfu flash         # DFU mode
```

### Flash Methods

```bash
make METHOD=stlink flash      # ST-Link (default)
make METHOD=openocd flash     # OpenOCD
make METHOD=gdb flash         # GDB with OpenOCD
make METHOD=serial flash      # Serial bootloader
make METHOD=dfu flash         # DFU mode
```

---

## Using as Radio Communication Template

### 1. Copy the Template
```bash
cp -r kelbaran my_radio_project
cd my_radio_project
```

### 2. Add Radio Module
Choose your radio module and add the appropriate driver:

#### For nRF24L01 (Recommended)
```bash
# Add to protocols/
# - klbn_spi.c/h (SPI driver)
# - klbn_nrf24l01.c/h (nRF24L01 driver)

# Update pins in include/klbn_pins.h:
# #define KLBN_RADIO_CE_PORT GPIOA
# #define KLBN_RADIO_CE_PIN 2
# #define KLBN_RADIO_CSN_PORT GPIOA  
# #define KLBN_RADIO_CSN_PIN 3
```

#### For HC-12 (Simplest)
```bash
# Add to protocols/
# - klbn_uart.c/h (UART driver)
# - klbn_hc12.c/h (HC-12 driver)

# Update pins for UART2:
# PA2 (TX), PA3 (RX)
```

### 3. Enable OLED Display (Optional)
```c
// In klbn_actuator_hub.c, uncomment:
klbn_oled_init();      // Line 18
klbn_oled_apply(...);  // Line 26
```

### 4. Update Controller for Radio
Add radio communication logic to `klbn_controller.c`:
```c
// TODO sections are ready for:
// - Radio initialization
// - Packet transmission
// - Packet reception
// - Radio status display
```

---

## Template Benefits

### Ultra-Minimal Footprint
- **19,892 bytes** Flash (vs 25,340+ bytes in original)
- **17,852 bytes** RAM usage
- **Perfect for STM32F103C8T6** limited resources

### Radio-Focused Design
- **No unused code**: Motor, servo, ultrasonic, IR remote all removed
- **Clean pin assignments**: Maximum pins available for radio
- **Focused tasks**: Only essential functionality remains
- **Ready placeholders**: TODO sections mark where to add radio code

### Template-Ready Features
- **No configuration overhead**: No #ifdef guards or config files
- **Copy-paste friendly**: Clean code structure
- **Modular radio integration**: Clear places to add radio drivers
- **Multiple radio support**: Architecture supports different radio modules
- **Visual feedback ready**: LEDs perfect for TX/RX status indication

---

## Binary Size

**Current minimal template**: **19,892 bytes** (Flash) + **17,852 bytes** (RAM)

*Radio protocol implementation will add approximately 2-4KB depending on module choice*

---

## Radio Implementation Roadmap

### Phase 1: SPI Driver (for nRF24L01)
- [ ] Add `klbn_spi.c/h` in `protocols/`
- [ ] Add SPI pin definitions
- [ ] Test SPI communication

### Phase 2: Radio Driver
- [ ] Add `klbn_nrf24l01.c/h` (or HC-12/LoRa)
- [ ] Implement packet structure
- [ ] Add radio task to task manager

### Phase 3: Communication Protocol
- [ ] Design packet format
- [ ] Implement TX/RX functions
- [ ] Add acknowledgment system

### Phase 4: User Interface
- [ ] Enable OLED for radio status
- [ ] Add radio status to LED indicators
- [ ] Implement mode button radio control

---

## License

This project is licensed under the GNU General Public License v3. See the LICENSE file for details.

---

## Contributing

This ultra-minimal template is designed to be the perfect starting point for radio communication projects. The template philosophy is "less is more" - providing only what's essential for radio communication implementation.