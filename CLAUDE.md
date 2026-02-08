# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino-based robotic hand controller that drives 8 servo motors (5 finger + 3 center/wrist) via serial commands using a custom binary protocol.

## Build & Upload

This is an Arduino project — build and upload via the **Arduino IDE**:
- Open `geared_hand_servo/geared_hand_servo.ino` in Arduino IDE
- Select the appropriate board and serial port
- Build: Sketch → Verify/Compile (Ctrl+R)
- Upload: Sketch → Upload (Ctrl+U)

**Dependencies:** Only the built-in `Servo.h` library (no external libraries required).

**Serial monitor:** 115200 baud for testing commands.

## Architecture

```
Serial (115200) → Packet Parser (hand_comm.h) → PacketCB() → Servo Control → PWM (pins 4-11)
```

**Two source files:**
- `geared_hand_servo.ino` — Main sketch: servo initialization, packet callback, smooth motion control loop
- `hand_comm.h` — Communication protocol: state machine parser for incoming serial bytes

**Servo pin mapping:**
- Pins 7–11: 5 finger servos (controlled via packet commands with smooth motion)
- Pins 4–6: 3 center/wrist servos (direct attach in setup)

## Communication Protocol

Custom binary format parsed by a state machine in `hand_comm.h`:

- **PACKET_SINGLE (5 bytes):** `F[ID][DEG]` — Control one servo (e.g., `F1090` = servo 1 to 90°)
- **PACKET_ALL (22 bytes):** `F[R/L][ID1][DEG1]...[ID5][DEG5]` — Control all 5 finger servos at once

The parser uses states: `WAIT_PACKET` → `WAIT_TYPE` → `WAIT_ID` → `WAIT_DEG` → callback invocation.

## Motion Control

The main loop implements smooth servo movement:
- Servos move in 10-degree increments per 20ms cycle
- Maximum 3 servos move simultaneously per cycle (load limiting to prevent power issues)
- PWM range: 544–2500 microseconds (custom calibration via `attach(pin, 544, 2500)`)
