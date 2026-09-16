#define LED_PIN 7
int count;
int toggle;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial){
    // Wait until serial begins
    ;
  }
  Serial.println("Hello World!");
  count = 0;
  toggle = 0;
  digitalWrite(LED_PIN, toggle);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(++count);
  toggle = flip_toggle(toggle);
  digitalWrite(LED_PIN, toggle);
  delay(1000);
}

int flip_toggle(int val) {
  return (val == 0)? 1 : 0;
}
