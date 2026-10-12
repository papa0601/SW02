// 서보 끝점 보정용 스케치 (MG90S 등)
//
// 시리얼 모니터: 57600 baud, 줄 끝 설정 "새 줄(Newline)"
//
// 명령어
//   1500   : 해당 µs 위치로 이동 (숫자 입력)
//   +      : 현재 step만큼 증가
//   -      : 현재 step만큼 감소
//   s5     : step을 5µs로 변경 (s10, s1 등)
//   min    : 현재 위치를 최소값(0° 쪽 끝)으로 기록
//   max    : 현재 위치를 최대값(180° 쪽 끝)으로 기록
//   p      : 기록한 값과 attach() 코드 출력
//
// 끝점에서 "지잉" 소리가 나거나 더 이상 안 움직이면 기계적 한계에 걸린 것.
// 그 상태로 오래 두지 말고, 소리가 사라지는 지점까지 되돌린 값을 기록할 것.

#include <Servo.h>

#define PIN_SERVO   10     // 서보 신호선(주황) 연결 핀
#define US_LIMIT_LO 400    // 보정 중 허용할 최소 펄스 (안전 한계)
#define US_LIMIT_HI 2600   // 보정 중 허용할 최대 펄스 (안전 한계)
#define MOVE_STEP   5      // 부드럽게 이동할 때 한 번에 움직이는 µs
#define MOVE_DELAY  5      // 이동 단계 사이 대기 (ms)

Servo myservo;
int cur = 1500;     // 현재 펄스 폭 (µs)
int step = 10;      // +/- 명령의 변화량 (µs)
int recMin = -1;    // 기록한 최소값
int recMax = -1;    // 기록한 최대값

void setup() {
  Serial.begin(57600);

  // 기본 attach(544~2400)로 하면 그 밖의 값이 잘리므로 범위를 넓게 잡음
  myservo.attach(PIN_SERVO, US_LIMIT_LO, US_LIMIT_HI);
  myservo.writeMicroseconds(cur);

  Serial.println("== Servo calibration ==");
  printHelp();
  printNow();
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "+") {
    moveTo(cur + step);
  } else if (cmd == "-") {
    moveTo(cur - step);
  } else if (cmd.startsWith("s")) {
    int v = cmd.substring(1).toInt();
    if (v > 0) {
      step = v;
      Serial.print("step = "); Serial.print(step); Serial.println(" us");
    } else {
      Serial.println("step 값이 올바르지 않음 (예: s5)");
    }
  } else if (cmd == "min") {
    recMin = cur;
    Serial.print("min 기록: "); Serial.print(recMin); Serial.println(" us");
  } else if (cmd == "max") {
    recMax = cur;
    Serial.print("max 기록: "); Serial.print(recMax); Serial.println(" us");
  } else if (cmd == "p") {
    printResult();
  } else if (isDigit(cmd[0])) {
    moveTo(cmd.toInt());
  } else {
    printHelp();
  }
}

// 목표 위치까지 조금씩 이동 (급격한 점프 방지)
void moveTo(int target) {
  target = constrain(target, US_LIMIT_LO, US_LIMIT_HI);
  while (cur != target) {
    if (cur < target) cur = min(cur + MOVE_STEP, target);
    else              cur = max(cur - MOVE_STEP, target);
    myservo.writeMicroseconds(cur);
    delay(MOVE_DELAY);
  }
  printNow();
}

void printNow() {
  Serial.print("now: "); Serial.print(cur); Serial.print(" us   (step ");
  Serial.print(step); Serial.println(")");
}

void printResult() {
  Serial.println("---- 결과 ----");
  Serial.print("min (0°)   : "); Serial.println(recMin);
  Serial.print("max (180°) : "); Serial.println(recMax);
  if (recMin > 0 && recMax > 0) {
    Serial.print("center(90°): "); Serial.println((recMin + recMax) / 2);
    Serial.println("사용 예:");
    Serial.print("  myservo.attach(PIN_SERVO, ");
    Serial.print(recMin); Serial.print(", "); Serial.print(recMax);
    Serial.println(");");
    Serial.println("  myservo.write(0);   // 0도");
    Serial.println("  myservo.write(180); // 180도");
  } else {
    Serial.println("min, max를 둘 다 기록해야 함");
  }
}

void printHelp() {
  Serial.println("명령: <숫자>=이동  +/-=step만큼  s<숫자>=step변경  min/max=기록  p=결과");
}
