// 핀
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// 설정값
#define SND_VEL 346.0    
#define INTERVAL 25       
#define PULSE_DURATION 10 
#define _DIST_MIN 100
#define _DIST_MAX 300

#define TIMEOUT ((INTERVAL / 2) * 1000.0) 
#define SCALE (0.001 * 0.5 * SND_VEL) 

#define _EMA_ALPHA 0.5    // EMA 비교용, 새 값 가중치
#define N 3               // 3, 10, 30 바꿔가며 캡처

float dist_med;
float dist_ema = 0;

float history[N];
float medians[N];

int sample_count = 0;

unsigned long last_sampling_time;   // ms

void setup() {
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  // EMA는 비교용, raw로 따로 돌림 (median 입력 아님)
  if (sample_count == 0) {
    dist_ema = dist_raw;
  } else {
    dist_ema = _EMA_ALPHA * dist_raw + (1.0 - _EMA_ALPHA) * dist_ema;
  }

  // 최신값 앞에 넣고 한 칸씩 밀기. + timeout 0도 그냥 넣음
  for (int i = N - 1; i > 0; i--) {
    history[i] = history[i - 1];
  }
  history[0] = dist_raw;

  if (sample_count < N) {
    sample_count++;
  }

  // 정렬용 복사
  for (int i = 0; i < sample_count; i++) {
    medians[i] = history[i];
  }

  // 오름차순, N 작아서 그냥 교환정렬
  for (int i = 0; i < sample_count - 1; i++) {
    for (int j = i + 1; j < sample_count; j++) {
      if (medians[i] > medians[j]) {
        float temp = medians[i];
        medians[i] = medians[j];
        medians[j] = temp;
      }
    }
  }

  // 대충 중위수 구하기
  if (sample_count % 2 == 1) {
    dist_med = medians[sample_count / 2];
  } else {
    dist_med = (medians[sample_count / 2 - 1]
              + medians[sample_count / 2]) / 2.0;
  }

  // 플로터
  Serial.print("Min:");     Serial.print(_DIST_MIN);
  Serial.print(",raw:");    Serial.print(dist_raw);
  Serial.print(",ema:");    Serial.print(dist_ema);
  Serial.print(",median:"); Serial.print(dist_med);
  Serial.print(",Max:");    Serial.print(_DIST_MAX);
  Serial.println("");

  // median 기준 LED
  if ((dist_med < _DIST_MIN) || (dist_med > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // 끔 (active low)
  else
    digitalWrite(PIN_LED, 0);       // 켬

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;

}
