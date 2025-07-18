# NRF24L01 Wiring Guide

This document provides the complete wiring instructions for connecting the **Hailege NRF24L01 + PA + LNA** radio modules to your STM32F103C8T6 (Blue Pill) boards.

## Required Components

### Per Board (you need 2 complete setups)
- 1x **STM32F103C8T6** (Blue Pill) development board
- 1x **Hailege NRF24L01 + PA + LNA** radio module with SMA antenna
- 1x **SMA antenna** (2.4GHz)
- **Jumper wires** (male-to-female recommended)
- **Breadboard** (optional, for organized connections)

## NRF24L01 Module Pinout

The NRF24L01 + PA + LNA module has **8 pins** arranged in a single row:

```
┌─────────────────────────┐
│ [SMA Antenna Connector] │
│                         │
│    NRF24L01 + PA + LNA  │
│                         │
└─┬─┬─┬─┬─┬─┬─┬─┬─────────┘
  1 2 3 4 5 6 7 8
```

### Pin Description
| Pin | Name | Function |
|-----|------|----------|
| 1 | **GND** | Ground |
| 2 | **VCC** | Power supply (3.3V) |
| 3 | **CE** | Chip Enable (TX/RX control) |
| 4 | **CSN** | Chip Select Not (SPI select) |
| 5 | **SCK** | SPI Clock |
| 6 | **MOSI** | SPI Master Out, Slave In |
| 7 | **MISO** | SPI Master In, Slave Out |
| 8 | **IRQ** | Interrupt Request (optional) |

## STM32F103C8T6 (Blue Pill) Pinout Reference

```
      ┌────────────────────┐
   B12│○                  ○│B11
   B13│○                  ○│B10
   B14│○                  ○│B1
   B15│○                  ○│B0
    A8│○                  ○│A7  ← MOSI
    A9│○                  ○│A6  ← MISO
   A10│○                  ○│A5  ← SCK
   A11│○                  ○│A4  ← Mode Button
   A12│○                  ○│A3  ← CSN
   A15│○                  ○│A2  ← CE
    B3│○                  ○│A1
    B4│○                  ○│A0
    B5│○ ← External LED   ○│C15 ← Debug LED
    B6│○ ← I2C SCL        ○│C14
    B7│○ ← I2C SDA        ○│C13 ← Onboard LED
      │    [USB]    [SWD]  │
      └────────────────────┘
```

## Wiring Connections

### Complete Wiring Table

| NRF24L01 Pin | Pin Name | STM32F103C8T6 Pin | Function |
|--------------|----------|-------------------|----------|
| 1 | **GND** | **GND** | Ground connection |
| 2 | **VCC** | **3.3V** | Power supply |
| 3 | **CE** | **PA2** | Chip Enable |
| 4 | **CSN** | **PA3** | Chip Select |
| 5 | **SCK** | **PA5** | SPI1 Clock |
| 6 | **MOSI** | **PA7** | SPI1 Master Out |
| 7 | **MISO** | **PA6** | SPI1 Master In |
| 8 | **IRQ** | **PA8** | Interrupt (optional) |

### Visual Wiring Diagram

```
NRF24L01 Module                    STM32F103C8T6 (Blue Pill)
┌─────────────────┐                ┌─────────────────────────┐
│                 │                │                         │
│  1 2 3 4 5 6 7 8 │                │                         │
│  │ │ │ │ │ │ │ │ │                │                         │
│  └─┼─┼─┼─┼─┼─┼─┼─┼────────────────┼─ GND                   │
│    └─┼─┼─┼─┼─┼─┼─┼────────────────┼─ 3.3V                  │
│      └─┼─┼─┼─┼─┼─┼────────────────┼─ PA2 (CE)              │
│        └─┼─┼─┼─┼─┼────────────────┼─ PA3 (CSN)             │
│          └─┼─┼─┼─┼────────────────┼─ PA5 (SCK)             │
│            └─┼─┼─┼────────────────┼─ PA7 (MOSI)            │
│              └─┼─┼────────────────┼─ PA6 (MISO)            │
│                └─┼────────────────┼─ PA8 (IRQ)             │
│                  │                │                         │
└─────────────────┘                │  PA4 ← Mode Button      │
                                   │  PB5 ← External LED     │
                                   │  PC13 ← Onboard LED     │
                                   │  PC15 ← Debug LED       │
                                   └─────────────────────────┘
```

## Step-by-Step Wiring Instructions

### ⚠️ **IMPORTANT: Power Off Both Boards Before Wiring**

### Step 1: Power Connections
1. Connect **NRF24L01 Pin 1 (GND)** → **Blue Pill GND**
2. Connect **NRF24L01 Pin 2 (VCC)** → **Blue Pill 3.3V**

### Step 2: SPI Connections
3. Connect **NRF24L01 Pin 5 (SCK)** → **Blue Pill PA5**
4. Connect **NRF24L01 Pin 6 (MOSI)** → **Blue Pill PA7**
5. Connect **NRF24L01 Pin 7 (MISO)** → **Blue Pill PA6**

### Step 3: Control Connections
6. Connect **NRF24L01 Pin 3 (CE)** → **Blue Pill PA2**
7. Connect **NRF24L01 Pin 4 (CSN)** → **Blue Pill PA3**
8. Connect **NRF24L01 Pin 8 (IRQ)** → **Blue Pill PA8** (optional)

### Step 4: Mode Button (Optional)
9. Connect a **push button** between **Blue Pill PA4** and **GND**

### Step 5: External LED (Optional)
10. Connect an **LED + resistor (220Ω)** between **Blue Pill PB5** and **GND**

## Testing Setup

### Board 1 (Transmitter/Receiver)
- Complete wiring as described above
- Flash with compiled firmware: `make flash`
- Power on the board
- **LED Status**: 
  - **Slow blink (500ms)**: RX mode (listening)
  - **Fast blink (100ms)**: TX mode (ready to transmit)

### Board 2 (Transmitter/Receiver)
- Complete wiring as described above
- Flash with **same firmware**: `make flash`
- Power on the board
- **LED Status**: Same as Board 1

## Operation Test

### Test Procedure
1. **Power on both boards**
2. **Observe LED patterns**:
   - Both boards should start in **RX mode** (slow blink)
3. **Press mode button** on Board 1:
   - Board 1 switches to **TX mode** (fast blink)
   - Board 1 sends test message: "Hello from board! Count: 1"
4. **Check Board 2**:
   - **Debug LED (PC15)** should pulse briefly when message received
5. **Press mode button** on Board 2:
   - Board 2 switches to **TX mode** (fast blink)
   - Board 2 sends test message: "Hello from board! Count: 1"
6. **Check Board 1**:
   - **Debug LED (PC15)** should pulse briefly when message received

### Expected Results
- ✅ **LED patterns change** when pressing mode button
- ✅ **Debug LED pulses** on receiving board when message is sent
- ✅ **Two-way communication** works in both directions
- ✅ **Range**: Should work up to 1100m (line of sight with PA+LNA)

## Troubleshooting

### Common Issues

#### No LED Activity
- **Check power connections**: Ensure 3.3V and GND are connected
- **Check firmware**: Verify successful flash with `make flash`
- **Check antenna**: Ensure SMA antenna is properly connected

#### LEDs Blink Very Fast (Error Mode)
- **Radio initialization failed**
- **Check wiring**: Verify all SPI connections (PA5, PA6, PA7)
- **Check power**: Ensure stable 3.3V supply
- **Check connections**: Verify CE (PA2) and CSN (PA3) connections

#### Mode Button Not Working
- **Check button wiring**: PA4 to GND through button
- **Check debouncing**: Wait 100ms between button presses
- **Check LED pattern**: Should change from slow to fast blink

#### No Communication Between Boards
- **Check antenna connections**: Both boards need antennas
- **Check distance**: Start with boards close together (1-2 meters)
- **Check interference**: Avoid WiFi routers, microwaves
- **Check wiring**: Ensure identical wiring on both boards

## Advanced Features

### Adding OLED Display (Optional)
If you want to see radio status on a display:

```
SSD1306 OLED Display → Blue Pill
VCC → 3.3V
GND → GND
SCL → PB6
SDA → PB7
```

Enable in code by uncommenting lines in `src/actuators/klbn_actuator_hub.c`:
```c
klbn_oled_init();      // Line 18
klbn_oled_apply(...);  // Line 26
```

### Extended Range Testing
- **Indoor range**: ~100-200m
- **Outdoor range**: ~500-1100m (line of sight)
- **Factors affecting range**: Obstacles, interference, antenna orientation

## Safety Notes

- **Use 3.3V only**: NRF24L01 is **NOT** 5V tolerant
- **Check connections**: Double-check wiring before powering on
- **Antenna required**: Never operate without antenna (can damage PA)
- **Power supply**: Ensure stable 3.3V supply (Blue Pill regulator is adequate)

## Success Indicators

✅ **Both boards power on** and show LED activity  
✅ **Mode button switches** between fast/slow blink patterns  
✅ **Debug LED pulses** when messages are received  
✅ **Two-way communication** works in both directions  
✅ **Range testing** successful at expected distances  

---

**Happy radio communication!** 📡
