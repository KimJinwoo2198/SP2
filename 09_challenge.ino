#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300
#define TIMEOUT ((INTERVAL / 2.0) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)
#define _EMA_ALPHA 0.5
#define MEDIAN_N 3

unsigned long last_sampling_time = 0;
float dist_ema = 0.0;
float samples[MEDIAN_N];
int sample_index = 0;
int sample_count = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH);
  Serial.begin(57600);
  last_sampling_time = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - last_sampling_time < INTERVAL) return;
  last_sampling_time = now;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  if (sample_count == 0) dist_ema = dist_raw;
  dist_ema = _EMA_ALPHA * dist_raw + (1.0 - _EMA_ALPHA) * dist_ema;

  samples[sample_index] = dist_raw;
  sample_index = (sample_index + 1) % MEDIAN_N;
  if (sample_count < MEDIAN_N) sample_count++;
  float dist_median = getMedian(samples, sample_count);

  Serial.print("Min:");
  Serial.print(_DIST_MIN);
  Serial.print(",raw:");
  Serial.print(dist_raw);
  Serial.print(",ema:");
  Serial.print(dist_ema);
  Serial.print(",median:");
  Serial.print(dist_median);
  Serial.print(",Max:");
  Serial.println(_DIST_MAX);

  digitalWrite(PIN_LED,
    (dist_raw < _DIST_MIN || dist_raw > _DIST_MAX) ? HIGH : LOW);
}

float getMedian(float data[], int size) {
  float sorted[MEDIAN_N];
  for (int i = 0; i < size; i++) sorted[i] = data[i];
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (sorted[i] > sorted[j]) {
        float temp = sorted[i];
        sorted[i] = sorted[j];
        sorted[j] = temp;
      }
    }
  }
  if (size % 2) return sorted[size / 2];
  return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
