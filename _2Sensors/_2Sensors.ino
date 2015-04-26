// ---------------------------------------------------------------------------
// This example code was used to successfully communicate with 15 ultrasonic sensors. You can adjust
// the number of sensors in your project by changing SONAR_NUM and the number of NewPing objects in the
// "sonar" array. You also need to change the pins for each sensor for the NewPing objects. Each sensor
// is pinged at 33ms intervals. So, one cycle of all sensors takes 495ms (33 * 15 = 495ms). The results
// are sent to the "oneSensorCycle" function which currently just displays the distance data. Your project
// would normally process the sensor results in this function (for example, decide if a robot needs to
// turn and call the turn function). Keep in mind this example is event-driven. Your complete sketch needs
// to be written so there's no "delay" commands and the loop() cycles at faster than a 33ms rate. If other
// processes take longer than 33ms, you'll need to increase PING_INTERVAL so it doesn't get behind.
// ---------------------------------------------------------------------------
#include <NewPing.h>
#include <avr/io.h>

#define DEBUG

#define SONAR_NUM     2 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

static uint16_t setTHR = 1100;    //Set default Throttle
static uint8_t ARMED = 0;

uint16_t pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
uint16_t cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.
uint16_t previousMillis = 0;

// Sonar Alt PID Variables
int16_t setpoint = 50;  // set to 50cm for now
int16_t  SonarAlt;
int32_t  errorSonar, deltaSonar;
int16_t  PTermSonar, ITermSonar, DTermSonar;
int32_t  errorAltitudeISonar = 0;
int32_t  lastSonarAlt;
int16_t  SonarPidOut;
uint8_t land = 0;
int8_t PSonar = 2;
int8_t ISonar = 1;
int8_t DSonar = 0;


NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(4, 4, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping. //Altitude Sonar
  NewPing(5, 5, MAX_DISTANCE)  //Front Sonar
};

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}

// Sonar Alt PID
uint16_t sonarPid(uint16_t sonarAlt) {
  goArm();
  if (ARMED) {
    static int hover_count = 0;
    uint16_t currentMillis = millis();

    if (currentMillis - previousMillis > 200) { // 200 ms
      hover_count++;
    }
    if (land == 0) {
      // P
      errorSonar = setpoint - sonarAlt;
      errorSonar = constrain(errorSonar, -250, 250); // limit
      PTermSonar = errorSonar * PSonar * 2 + 1000;

      // I
      errorAltitudeISonar += errorSonar;
      errorAltitudeISonar = constrain(errorAltitudeISonar, -30000, 30000); // WindUp
      ITermSonar = (int32_t)ISonar * errorAltitudeISonar / 100;

      // D
      deltaSonar = SonarAlt - lastSonarAlt;
      lastSonarAlt = SonarAlt;
      DTermSonar = (int32_t)deltaSonar * DSonar;            // 32 bits is needed for calculation

      // calcs
      SonarPidOut = PTermSonar + ITermSonar - DTermSonar;
      setTHR = SonarPidOut;

      if (hover_count > 700)  land = 1;      //200*25=5000ms
    }
    else {
      setTHR--;
      if (SonarAlt <= 10)
        ARMED = 0;
    }
  }
  return constrain(setTHR, 1100, 1800);
}

void goArm(){
  ARMED = 1;
}

void genPwm(uint8_t pin, uint16_t pulseWidth) {
  OCR1A =pulseWidth<<3;
}

void initPins(){
   pinMode(9, OUTPUT); // set pin 9 to output
     
   TCCR1A |= (1<<WGM11); 
   TCCR1A &= ~(1<<WGM10); 
   TCCR1B |= (1<<WGM13);  // phase correct mode.. to know how to set which register please see the datasheets of the MCU's
   
   TCCR1B &= ~(1<<CS11); // no prescaler
   ICR1 |= 0x3FFF; // set timers to count to 16383 (hex 3FFF)
   TCCR1A |= _BV(COM1A1); // connect pin 9 to timer 1 channel A
    
   OCR1A = setTHR<<3;// this wloud give us a half throttle signal // X<<1 = X*2, X<<2 = X*4 and X<<3 = X*8.... 
  
}

void setup() {
  initPins();
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {
  static uint16_t throttle;
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) {
        //oneSensorCycle(); 
        throttle = sonarPid(cm[0]); 
        
      } // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // Other code that *DOESN'T* analyze ping results can go here.
  genPwm(9, throttle);Serial.println(throttle); 
}

