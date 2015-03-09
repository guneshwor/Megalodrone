#include "NewPing.h"


void init_sonar();
uint8_t trigger_pin = TRIGGER_PIN;
uint8_t echo_pin = ECHO_PIN;
unsigned int max_cm_distance = MAX_SENSOR_DISTANCE;

void init_sonar(){
  _triggerBit = digitalPinToBitMask(trigger_pin); // Get the port register bitmask for the trigger pin.
  _echoBit = digitalPinToBitMask(echo_pin);       // Get the port register bitmask for the echo pin.

  _triggerOutput = portOutputRegister(digitalPinToPort(trigger_pin)); // Get the output port register for the trigger pin.
  _echoInput = portInputRegister(digitalPinToPort(echo_pin));         // Get the input port register for the echo pin.

  _triggerMode = (uint8_t *) portModeRegister(digitalPinToPort(trigger_pin)); // Get the port mode register for the trigger pin.

#if ROUNDING_ENABLED == false
  _maxEchoTime = min(max_cm_distance + 1, MAX_SENSOR_DISTANCE + 1) * US_ROUNDTRIP_CM; // Calculate the maximum distance in uS (no rounding).
#else
  _maxEchoTime = min(max_cm_distance, MAX_SENSOR_DISTANCE) * US_ROUNDTRIP_CM + (US_ROUNDTRIP_CM / 2); // Calculate the maximum distance in uS.
#endif

#ifndef __AVR__
  _triggerPin = trigger_pin;    // Assign the trigger pin.
  pinMode(echo_pin, INPUT);     // Set echo pin to input (on Teensy 3.x (ARM), pins default to disabled, at least one pinMode() is needed for GPIO mode).
  piinMode(trigger_pin, OUTPUT); // Set trigger pin to output (on Teensy 3.x (ARM), pins default to disabled, at least one pinMode() is needed for GPIO mode).
#endif

#if ONE_PIN_ENABLED != true
  *_triggerMode |= _triggerBit; // Set trigger pin to output.
#endif
}


