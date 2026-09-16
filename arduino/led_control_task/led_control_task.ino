#define LED_PIN 7

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) {
    ;
  }
  Serial.println("[Log] Serial Started");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("[Log] Entered loop() function");
  Serial.println("[Log] Turning on LED for 1 sec");
  digitalWrite(LED_PIN, 0);
  delay(1000);
  Serial.println("[Log] Starting Flicker for 1 sec (cycle: 200ms)");
  int i;
  for (i=0; i<5; i++) {
    digitalWrite(LED_PIN, 1);
    delay(100);
    digitalWrite(LED_PIN, 0);
    delay(100);
  }
  Serial.println("[Log] Done. Entering infinite loop");
  digitalWrite(LED_PIN, 1);
  while(1) {}
}
