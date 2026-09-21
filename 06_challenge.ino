const int LED_PIN = 7;
const int LED_ON = LOW;
const int LED_OFF = HIGH;
const unsigned long TRIANGLE_PERIOD = 1000000UL;
const unsigned long DUTY_INTERVAL = 5000UL;
const unsigned long SEGMENT_DURATION = 3000000UL;
const unsigned long TOTAL_DURATION = 9000000UL;

const unsigned int PWM_PERIODS[] = {10000, 1000, 100};

unsigned int pwmPeriod = PWM_PERIODS[0];
int pwmDuty = 0;
unsigned long startTime = 0;
bool finished = false;

void set_period(int period) {
  pwmPeriod = constrain(period, 100, 10000);
}

void set_duty(int duty) {
  pwmDuty = constrain(duty, 0, 100);
}

void pwm(unsigned long elapsed) {
  unsigned long onTime = (unsigned long)pwmPeriod * pwmDuty / 100;
  unsigned long phase = elapsed % pwmPeriod;
  bool ledOn = phase < onTime;
  unsigned long waitTime = ledOn ? onTime - phase : pwmPeriod - phase;
  unsigned long nextDuty = DUTY_INTERVAL - elapsed % DUTY_INTERVAL;

  if (waitTime > nextDuty) {
    waitTime = nextDuty;
  }

  digitalWrite(LED_PIN, ledOn ? LED_ON : LED_OFF);
  delayMicroseconds((unsigned int)waitTime);
}

void trianglePWM(unsigned long elapsed) {
  int step = elapsed / DUTY_INTERVAL;
  set_duty(step <= 100 ? step : 200 - step);
  pwm(elapsed);
}

void setup() {
  digitalWrite(LED_PIN, LED_OFF);
  pinMode(LED_PIN, OUTPUT);
  set_period(PWM_PERIODS[0]);
  startTime = micros();
}

void loop() {
  if (finished) {
    return;
  }

  unsigned long elapsed = micros() - startTime;

  if (elapsed >= TOTAL_DURATION) {
    set_duty(0);
    digitalWrite(LED_PIN, LED_OFF);
    finished = true;
    return;
  }

  int periodIndex = elapsed / SEGMENT_DURATION;
  set_period(PWM_PERIODS[periodIndex]);
  trianglePWM(elapsed % TRIANGLE_PERIOD);
}
