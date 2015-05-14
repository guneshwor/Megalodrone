#include "sonar.h"

uint8_t trigger_pin = TRIGGER_PIN;
uint8_t echo_pin = ECHO_PIN;
unsigned int max_cm_distance = MAX_SENSOR_DISTANCE;

void initSonar(){
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

boolean pingTrigger(boolean interrupt) {
#if ONE_PIN_ENABLED == true
  *_triggerMode |= _triggerBit; // Set trigger pin to output.
#endif

  *_triggerOutput &= ~_triggerBit; // Set the trigger pin low, should already be low, but this will make sure it is.
  delayMicroseconds(4);            // Wait for pin to go low.
  *_triggerOutput |= _triggerBit;  // Set trigger pin high, this tells the sensor to send out a ping.
  delayMicroseconds(10);           // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
  *_triggerOutput &= ~_triggerBit; // Set trigger pin back to low.

#if ONE_PIN_ENABLED == true
  *_triggerMode &= ~_triggerBit; // Set trigger pin to input (when using one Arduino pin this is technically setting the echo pin to input as both are tied to the same Arduino pin).
#endif

  if (interrupt) return true;

#if URM37_ENABLED == true
  if (!(*_echoInput & _echoBit)) return false;            // Previuos ping hasn't finished, abort.
  _max_time = micros() + _maxEchoTime + MAX_SENSOR_DELAY; // Maximum time we'll wait for ping to start (most sensors are <450uS, the SRF06 can take up to 34,300uS!)
  while (*_echoInput & _echoBit)                          // Wait for ping to start.
  	if (micros() > _max_time) return false;             // Took too long to start, abort.
#else
  if (*_echoInput & _echoBit) return false;               // Previuos ping hasn't finished, abort.
  _max_time = micros() + _maxEchoTime + MAX_SENSOR_DELAY; // Maximum time we'll wait for ping to start (most sensors are <450uS, the SRF06 can take up to 34,300uS!)
  while (!(*_echoInput & _echoBit))                       // Wait for ping to start.
  	if (micros() > _max_time) return false;             // Took too long to start, abort.
#endif  
  _max_time = micros() + _maxEchoTime; // Ping started, set the timeout.
  return true;                         // Ping started successfully.
}


uint16_t ping(){
  if (!pingTrigger()) return NO_ECHO; // Trigger a ping, if it returns false, return NO_ECHO to the calling function.

#if URM37_ENABLED == true
  while (!(*_echoInput & _echoBit))               // Wait for the ping echo.
  	if (micros() > _max_time) return NO_ECHO;   // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
#else
  while (*_echoInput & _echoBit)                  // Wait for the ping echo.
  	if (micros() > _max_time) return NO_ECHO;   // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
#endif  
  return (micros() - (_max_time - _maxEchoTime) - PING_OVERHEAD); // Calculate ping time, include overhead.
}

uint16_t pingCm() {
  uint16_t echoTime = ping();         // Calls the ping method and returns with the ping echo distance in uS.
#if ROUNDING_ENABLED == false
  return (echoTime / US_ROUNDTRIP_CM);              // Call the ping method and returns the distance in centimeters (no rounding).
#else
  return NewPingConvert(echoTime, US_ROUNDTRIP_CM); // Convert uS to centimeters.
#endif
}

