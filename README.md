# ATtiny85-Servo-Heartbeat-Controller

This project provides code for an ATtiny85 microcontroller to control the power to a servo rail based on heartbeat signals received via I2C communication. If a heartbeat signal is not received within a specified timeout period, the servo rail is powered down, and the ATtiny85 enters a low power sleep mode. When a heartbeat signal is received, the servo rail is powered on, and the ATtiny85 remains active.

## Features

- Monitors heartbeat signals via I2C communication.
- Powers down the servo rail when no heartbeat signal is received.
- Puts the ATtiny85 into sleep mode to save power.
- Powers on the servo rail and keeps the ATtiny85 active when a heartbeat signal is received.

## Hardware Requirements

- ATtiny85 microcontroller
- Servo rail
- I2C master device (e.g., Raspberry Pi)
- Supporting electronic components (e.g., resistors, capacitors)

## Pin Configuration

- `powerControlPin (Pin 0)`: Controls power to the servo rail.
- `PB1 (Pin 6)`: Additional control signal (set high when no heartbeat, low when heartbeat received).

## Dependencies

- TinyWireS library for I2C communication
- avr/sleep.h for power management

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/ATtiny85-Servo-Heartbeat-Controller.git
    ```

2. Include the required libraries in your project:
    - [TinyWireS](https://github.com/rambo/TinyWire)

3. Upload the code to your ATtiny85 microcontroller.

## Usage

1. Connect your hardware according to the pin configuration.
2. Upload the provided code to the ATtiny85.
3. Send a heartbeat signal ('H') from your I2C master device (e.g., Raspberry Pi) to the ATtiny85 periodically.
4. The servo rail will power down if no heartbeat is received within the specified timeout period and the ATtiny85 will enter sleep mode.
5. The servo rail will power on when a heartbeat signal is received and the ATtiny85 will remain active.

## Code Explanation

```cpp
#include <TinyWireS.h>
#include <avr/sleep.h>

const int powerControlPin = 0; // Pin to control power to the servo rail
const int heartbeatTimeout = 1000; // Timeout period in milliseconds
volatile bool heartbeatReceived = false;
unsigned long lastHeartbeatTime = 0;

void setup() {
  pinMode(powerControlPin, OUTPUT);
  digitalWrite(powerControlPin, HIGH); // Initially, power on the servo rail

  TinyWireS.begin(0x08); // Set I2C address for the ATtiny85
  TinyWireS.onReceive(receiveEvent); // Register the receive event handler

  DDRB |= (1 << PB1); // Set PB1 as output
}

void loop() {
  TinyWireS_stop_check(); // Call this in your loop for proper TinyWireS functionality

  if (millis() - lastHeartbeatTime > heartbeatTimeout) {
    digitalWrite(powerControlPin, LOW); // No heartbeat received, power down the servo rail
    PORTB |= (1 << PB1); // Set PB1 high

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Enter sleep mode
    sleep_enable();
    sleep_mode();
  } else {
    digitalWrite(powerControlPin, HIGH); // Heartbeat received, power on the servo rail
    PORTB &= ~(1 << PB1); // Set PB1 low
  }

  delay(100); // Sleep for a short period to save power
}

void receiveEvent(uint8_t howMany) {
  while (TinyWireS.available()) {
    char c = TinyWireS.read();
    if (c == 'H') { // Assuming 'H' is the heartbeat signal
      heartbeatReceived = true;
      lastHeartbeatTime = millis();
    }
  }
}
```
## License

This project is licensed under the MIT License. See the LICENSE file for details.
Contributing

Contributions are welcome! Please open an issue or submit a pull request for any changes or improvements.
## Contact
Author: Farhang Naderi
Email: farhang.naderi@uri.edu
