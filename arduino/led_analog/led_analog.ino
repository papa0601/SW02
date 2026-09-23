#define LED_PIN 9
int brightness = 0;
int flucktuate = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(LED_PIN, brightness);

  brightness += flucktuate;

  if (brightness < 0) {
    brightness = 0;
    flucktuate = -flucktuate;
  } else if (brightness > 255) {
    brightness = 255;
    flucktuate = -flucktuate;
  }
  delay(50);
}
