# Geared Hand

Arduino-based robotic hand controller that drives 8 servo motors (5 finger) via a custom serial command protocol.

## Hardware

- **Finger servos:** 5 servos on pins 7–11 (PWM range 544–2500 μs)
- **Communication:** Serial at 115200 baud

## Serial Protocol

Two command formats are supported:

| Format | Length | Example | Description |</br>
|------|--------|--------|---------|-------------|</br>
| `F[R/L][ID][DEG]×5` | 22 bytes | `FR00901091209230934094` | Set all 5 finger servos |

- `R`/`L` designates right or left hand
- `ID` is the servo index (0–4)
- `DEG` is a 3-digit angle (000–180)

## Build

Open `geared_hand_servo/geared_hand_servo.ino` in the Arduino IDE, select your board and port, then upload.

## Author

Karl Kwon
(mrthinks@gmail.com)