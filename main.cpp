#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display(OLED_RESET);

#define XPOS 0
#define YPOS 1
#define DELTAY 2

// Tamiya time checker with HC-SR04
// OLED 추가 예정
// 버튼을 이용한 캘리브레이션 적용
// 차량 검출 효과를 위한 LED 적용
// 부저를 활용한 소리 효과
// 버튼 적용 캘리브레이션 / 모드
// 트랙 1 ~ 5

// PIN, 변수 선언
int trigPin = 3;
int echoPin = 2;
unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int cal_count = 0;
int buzzer = 8 ;// setting controls the digital IO foot buzzer
// notes in the melody:
int melody[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {8, 8, 8, 8};

int start_melody[] = {NOTE_B5, NOTE_B5, NOTE_B5, NOTE_B6};
int start_noteDurations[] = {2, 2, 2, 1};

// 함수 선언
long distance_check();
long calibration_dist();
void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h);

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // 센서 Trig 핀
  pinMode(echoPin, INPUT);  // 센서 Echo 핀
  pinMode (buzzer, OUTPUT) ;// set the digital IO pin mode, OUTPUT out of Wen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();   
  Serial.begin(115200);  
  // draw a bitmap icon and 'animate' movement
}

void loop() {
  long temp_dist = distance_check() + 2;
  if(cal_dist > temp_dist) {
    Serial.println("Car Detected!");
    char buf[20];
    snprintf(buf, sizeof(buf), "Distance %4d cm", int(temp_dist));
    Serial.println(buf);
    delay(100);
    // 부저 테스트
    for (int i = 0; i <4; i++) // Wen a frequency sound
    {
    int noteDuration = 1000 / noteDurations[i];
    tone(buzzer, melody[i], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzer);    
    }    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Average is = ");
    display.println(temp_dist);
    display.display();    
  }
  // 캘리브레이션
  while (cal_count < 1) {
    cal_dist = calibration_dist();
    cal_count++;
  }
}

// 거리를 측정해서 거리값을 반환한다
long distance_check() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, HIGH);  // 센서에 Trig 신호 입력
  delayMicroseconds(10);        // 10us 정도 유지
  digitalWrite(trigPin, LOW);   // Trig 신호 off

  long duration = pulseIn(echoPin, HIGH);    // Echo pin: HIGH->Low 간격을 측정
  long distance = duration / 29 / 2;         // 거리(cm)로 변환 
  return distance;
}

// 거리를 측정해서 값이 연속해서 3번 일치하면 캘리브레이션 완료된 것으로 적용
long calibration_dist() {
  currentMillis = millis();
  long dist_check = distance_check();
  Serial.println("### Start calibration ###");
  while(millis()-currentMillis < 4000) {
    if(dist_check == distance_check()) {
      Serial.println("### Calibration confirm between speed checker and track distance. ###");
      delay(1000);
    }
    else {
      currentMillis = millis();
      dist_check = distance_check();
      Serial.println("### On calibrating...###");
      delay(1000);
    }
  }
  Serial.print("### Calibration success ");
  Serial.print(int(dist_check));
  Serial.println(" cm" );
  for (int i = 0; i <4; i++) // Wen a frequency sound
  {
  int noteDuration = 1000 / start_noteDurations[i];
  tone(buzzer, start_melody[i], noteDuration);
  delay(1000);
  // stop the tone playing:
  noTone(buzzer);    
  }   
  return dist_check;
}


