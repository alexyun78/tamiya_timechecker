#include <Arduino.h>
// Tamiya time checker with HC-SR04
// OLED 추가 예정
// 버튼을 이용한 캘리브레이션 적용
// 차량 검출 효과를 위한 LED 적용
// 부저를 활용한 소리 효과
// 버튼 적용 캘리브레이션 / 모드
// 트랙 1 ~ 5


int trigPin = 3;
int echoPin = 2;
unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int cal_count = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // 센서 Trig 핀
  pinMode(echoPin, INPUT);  // 센서 Echo 핀
  Serial.begin(115200);  
}

long distance_check() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, HIGH);  // 센서에 Trig 신호 입력
  delayMicroseconds(10);        // 10us 정도 유지
  digitalWrite(trigPin, LOW);   // Trig 신호 off

  long duration = pulseIn(echoPin, HIGH);    // Echo pin: HIGH->Low 간격을 측정
  long distance = duration / 29 / 2;         // 거리(cm)로 변환 
  return distance;
}

void calibration_time() {
  currentMillis = millis();
  cal_dist = distance_check();
  Serial.println("####### Start calibration #######");
  while(millis()-currentMillis < 4000) {
    if(cal_dist == distance_check()) {
      Serial.println("####### Calibration confirm between speed checker and track distance. #######");
      delay(1000);
    }
    else {
      currentMillis = millis();
      cal_dist = distance_check();
      Serial.println("####### On calibrating... #######");
      delay(1000);
    }
  }
  Serial.print("####### Calibration success ");
  Serial.print(int(cal_dist));
  Serial.println(" cm" );
}

void loop() {
  if(cal_dist > distance_check()+2) {
    Serial.println("Car Detected!");
    char buf[20];
    snprintf(buf, sizeof(buf), "Distance %4d cm", int(distance_check()));
    Serial.println(buf);
    delay(100);
  }
  // 캘리브레이션
  while (cal_count < 1) {
    calibration_time();
    cal_count++;
  }
}