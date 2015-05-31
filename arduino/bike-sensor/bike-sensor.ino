const int PIN_LED = 11;
const int PIN_PULSE = 5;
const int DEBOUNCE_TIME = 100;
const int LED_TIME = 100;

unsigned long last_rev = 0;
unsigned long led_off = 0;

void revolution() {
  unsigned long time = millis();
  long elapsed = time - last_rev;
  if (elapsed > DEBOUNCE_TIME) {
    digitalWrite(PIN_LED, HIGH);
    led_off = time + LED_TIME;
    last_rev = time;

    int rpm = 60000 / elapsed;
    Serial.println(rpm);
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PULSE, INPUT_PULLUP);

  Serial.begin(9600);

  attachInterrupt(0, revolution, FALLING);
}

void loop() {
  if (millis() > led_off) {
    digitalWrite(PIN_LED, LOW);
  }
}
