// ---------------------------------------------------------------------------
// NewPing Library - v1.6 pre-release - 06/17/2014
//
// AUTHOR/LICENSE:
// Created by Tim Eckel - teckel@leethost.com
// Copyright 2014 License: GNU GPL v3 http://www.gnu.org/licenses/gpl.html
//
// LINKS:
// Project home: http://code.google.com/p/arduino-new-ping/
// Blog: http://arduino.cc/forum/index.php/topic,106043.0.html
//
// DISCLAIMER:
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.


#ifndef NEWPING_H_
#define NEWPING_H_

#include <avr/pgmspace.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     8   // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
	#include <pins_arduino.h>
#endif

#if defined(__AVR__)
	#include <avr/io.h>
	#include <avr/interrupt.h>
#endif

// Shoudln't need to changed these values unless you have a specific need to do so.
#define MAX_SENSOR_DISTANCE 500 // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to travel this distance and back. Default=500
#define US_ROUNDTRIP_CM 57      // Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space. Default=57
#define US_ROUNDTRIP_IN 146     // Microseconds (uS) it takes sound to travel round-trip 1 inch (2 inches total), uses integer to save compiled code space. Defalult=146
#define ONE_PIN_ENABLED true    // Set to "false" to disable one pin mode which saves around 14-26 bytes of binary size. Default=true
#define ROUNDING_ENABLED false  // Set to "true" to enable distance rounding which also adds 64 bytes to binary size. Default=false
#define URM37_ENABLED false     // Set to "true" to enable support for the URM37 sensor in PWM mode. Default=false
#define TIMER_ENABLED true      // Set to "false" to disable the timer ISR (if getting "__vector_7" compile errors set this to false). Default=true

// Probably shoudln't change these values unless you really know what you're doing.
#define NO_ECHO 0               // Value returned if there's no ping echo within the specified MAX_SENSOR_DISTANCE or max_cm_distance. Default=0
#define MAX_SENSOR_DELAY 5800   // Maximum uS it takes for sensor to start the ping. Default=5800
#define ECHO_TIMER_FREQ 24      // Frequency to check for a ping echo (every 24uS is about 0.4cm accuracy). Default=24
#define PING_MEDIAN_DELAY 29000 // Microsecond delay between pings in the ping_median method. Default=29000
#define PING_OVERHEAD 5         // Ping overhead in microseconds (uS). Default=5
#define PING_TIMER_OVERHEAD 13  // Ping timer overhead in microseconds (uS). Default=13
#if URM37_ENABLED == true
	#undef  US_ROUNDTRIP_CM
	#undef  US_ROUNDTRIP_IN
	#define US_ROUNDTRIP_CM 50      // Every 50uS PWM signal is low indicates 1cm distance. Default=50
	#define US_ROUNDTRIP_IN 127     // If 50uS is 1cm, 1 inch would be 127uS (50 x 2.54 = 127). Default=127
#endif

// Conversion from uS to distance (round result to nearest cm or inch).
#define NewPingConvert(echoTime, conversionFactor) (max(((unsigned int)echoTime + conversionFactor / 2) / conversionFactor, (echoTime ? 1 : 0)))

// Detect non-AVR microcontrollers (Teensy 3.x, Arduino DUE, etc.) and don't use port registers or timer interrupts as required.
#if defined (__arm__) && defined (TEENSYDUINO)
	#undef  PING_OVERHEAD
	#define PING_OVERHEAD 1
	#undef  PING_TIMER_OVERHEAD
	#define PING_TIMER_OVERHEAD 1
#elif !defined (__AVR__)
	#undef  PING_OVERHEAD
	#define PING_OVERHEAD 1
	#undef  PING_TIMER_OVERHEAD
	#define PING_TIMER_OVERHEAD 1
	#undef  TIMER_ENABLED
	#define TIMER_ENABLED false
#endif

// Disable the timer interrupts when using ATmega128 and all ATtiny microcontrollers.
#if defined (__AVR_ATmega128__) || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny261__) || defined(__AVR_ATtiny461__) || defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny43U__)
	#undef  TIMER_ENABLED
	#define TIMER_ENABLED false
#endif

// Define timers when using ATmega8 microcontrollers.
#if defined (__AVR_ATmega8__)
	#define OCR2A OCR2
	#define TIMSK2 TIMSK
	#define OCIE2A OCIE2
#endif

//class NewPing {
//	public:
		//NewPing(uint8_t trigger_pin, uint8_t echo_pin, unsigned int max_cm_distance = MAX_SENSOR_DISTANCE);
                
		unsigned int ping();
		unsigned long ping_cm();
		unsigned long ping_in();
		unsigned long ping_median(uint8_t it = 5);
//		void ping_interrupt(void (*userFunc)(void));
//		unsigned int get_interrupt();
		unsigned int convert_cm(unsigned int echoTime);
		unsigned int convert_in(unsigned int echoTime);
#if TIMER_ENABLED == true
		void ping_timer(void (*userFunc)(void));
		boolean check_timer();
		static unsigned long ping_result;
		static void timer_us(unsigned int frequency, void (*userFunc)(void));
		static void timer_ms(unsigned long frequency, void (*userFunc)(void));
		static void timer_stop();
#endif
//	private:
		boolean ping_trigger(boolean interrupt = false);
//		void ping_interrupt2();
#if TIMER_ENABLED == true
		boolean ping_trigger_timer(unsigned int trigger_delay);
		boolean ping_wait_timer();
		static void timer_setup();
		static void timer_ms_cntdwn();
#endif
//		uint8_t _echoPinInt;
#ifndef __AVR__
		uint8_t _triggerPin;
#endif
		static uint8_t _triggerBit;
		static uint8_t _echoBit;
		static volatile uint8_t *_triggerOutput;
		static volatile uint8_t *_echoInput;
		static volatile uint8_t *_triggerMode;
		static unsigned int _maxEchoTime;
		static unsigned long _max_time;
//};
void init_sonar();

#endif
