#define LED_PIN 7

void setup() {
  Serial.begin(9600);
  Serial.println("ready");

  // 처음에는 LED가 꺼진 상태로 시작
  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {

  // 처음 1초 동안 LED ON
  digitalWrite(LED_PIN, LOW);
  Serial.println("1");
  delay(1000);

  // 다음 1초 동안 5회 깜빡임
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);  // OFF
    delay(100);
    digitalWrite(LED_PIN, LOW);   // ON
    delay(100);
  }

  // LED OFF
  digitalWrite(LED_PIN, HIGH);

  // 여기서 정지
  while (1) {
  }
}
