#define INTERVAL 500
unsigned long last_sampling_time = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("A");
  delay(INTERVAL);
}

void not_loop() {
  if (millis() < (last_sampling_time + INTERVAL)) {
    return;
  } else {
    last_sampling_time += INTERVAL;
    Serial.println("A");
  }
}
