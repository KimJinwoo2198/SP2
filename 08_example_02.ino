// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)

#define _DIST_MIN 100.0   // minimum distance (unit: mm)
#define _DIST_MID 200.0   // maximum LED brightness distance (unit: mm)
#define _DIST_MAX 300.0   // maximum distance (unit: mm)

#define TIMEOUT ((INTERVAL / 2.0) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)       // coefficient to convert duration to distance

unsigned long last_sampling_time = 0;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // Active LOW LED
  // 255 = OFF
  // 0   = maximum brightness
  analogWrite(PIN_LED, 255);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;
  int led_value;

  // sampling every 25 ms
  unsigned long current_time = millis();

  if (current_time - last_sampling_time < INTERVAL) {
    return;
  }

  last_sampling_time = current_time;

  // measure distance
  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  /*
    LED brightness control

    distance     analogWrite
    ------------------------
    100 mm       255 (OFF)
    150 mm       ~128 (50%)
    200 mm       0   (MAX)
    250 mm       ~128 (50%)
    300 mm       255 (OFF)

    Active LOW:
    analogWrite(PIN_LED, 0)   -> brightest
    analogWrite(PIN_LED, 255) -> OFF
  */

  if ((distance == 0.0) || (distance < _DIST_MIN) || (distance > _DIST_MAX)) {

    // outside measurement range -> LED OFF
    led_value = 255;

  }
  else if (distance <= _DIST_MID) {

    // 100 mm -> 255
    // 200 mm -> 0
    float ratio = (distance - _DIST_MIN)
                  / (_DIST_MID - _DIST_MIN);

    led_value = (int)(255.0 * (1.0 - ratio));

  }
  else {

    // 200 mm -> 0
    // 300 mm -> 255
    float ratio = (distance - _DIST_MID)
                  / (_DIST_MAX - _DIST_MID);

    led_value = (int)(255.0 * ratio);
  }

  // control LED brightness
  analogWrite(PIN_LED, led_value);

  // output values to serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",distance:");
  Serial.print(distance);

  Serial.print(",LED:");
  Serial.print(led_value);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println();
}


// get a distance reading from USS.
// return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  unsigned long duration = pulseIn(ECHO, HIGH, TIMEOUT);

  return duration * SCALE;
}
