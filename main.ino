/*
 * ATtiny85-Servo-Heartbeat-Controller
 * 
 * Author: Farhang Naderi
 * Email: farhang.naderi@uri.edu
 * License: MIT License
 * Year: 2024
 * 
 * Description:
 * This code runs on an ATtiny85 microcontroller to control the power to a servo rail based on
 * heartbeat signals received via I2C communication. If a heartbeat signal is not received within
 * a specified timeout period, the servo rail is powered down, and the ATtiny85 enters a low power
 * sleep mode. When a heartbeat signal is received, the servo rail is powered on, and the ATtiny85
 * remains active.
 */

#include <TinyWireS.h>
#include <avr/sleep.h>

const int powerControlPin = 0; // Pin to control power to the servo rail
const int heartbeatTimeout = 2000; // Timeout period in milliseconds
volatile bool heartbeatReceived = false;
unsigned long lastHeartbeatTime = 0;

void setup() {
  // Initialize the power control pin as an output
  pinMode(powerControlPin, OUTPUT);
  digitalWrite(powerControlPin, HIGH); // Initially, power on the servo rail

  // Initialize I2C communication
  TinyWireS.begin(0x08); // Set I2C address for the ATtiny85
  TinyWireS.onReceive(receiveEvent); // Register the receive event handler

  // Set PB1 as output
  DDRB |= (1 << PB1);
}

void loop() {
  TinyWireS_stop_check(); // Call this in your loop for proper TinyWireS functionality

  // Check if a heartbeat has been received within the timeout period
  if (millis() - lastHeartbeatTime > heartbeatTimeout) {
    // No heartbeat received, power down the servo rail
    digitalWrite(powerControlPin, LOW);
    
    // Set PB1 high
    PORTB |= (1 << PB1);
    
    // Put the ATtiny85 into sleep mode to save power
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
  } else {
    // Heartbeat received, ensure power to the servo rail is on
    digitalWrite(powerControlPin, HIGH);
    
    // Set PB1 low
    PORTB &= ~(1 << PB1);
  }

  // Sleep for a short period to save power
  delay(100);
}

// I2C receive event handler
void receiveEvent(uint8_t howMany) {
  while (TinyWireS.available()) {
    char c = TinyWireS.read();
    if (c == 'H') { // Assuming 'H' is the heartbeat signal
      heartbeatReceived = true;
      lastHeartbeatTime = millis();
    }
  }
}
