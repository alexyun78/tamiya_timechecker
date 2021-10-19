#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

// 1. 부팅시 Logo 출력
// 2. 상단에 측정 기록된 거리 출력. 초기 출력 Distance 00 Track Round 3, Time Over 30, Best Record 00:00:00 Best Average 00:00:00
// 3. A 버튼 클릭 안내문 Press the A button to correct the distance.
// 4. Calibration 종료 후, Distance 09 출력
// 5. 화면에 Game Start, Button A
// 6. 부저 사운드와 함께 3, 2, 1, Go 출력
// 7. 처음 차량 인지하면서 기록 시간 시작
// 8. 총 3회 측정애서 게임 종료 함
// 9. 게임중에는 버튼 입력 불가. 단, 한 트랙 측정 중 1분 초과시 자동 종료
// 10. 게임 종료 후 다시 A버튼 누르면 게임 시작
// 11. Game start 화면이나 종료된 화면에서 B 버튼 누르면 설정 화면 진입
// 12. 1. Calibrate 2. Track 1~5(3), Time over 20~90(30), Best track time(기본), Best Average time 선택


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
// 버튼을 이용한 캘리브레이션 적용, 
// A버튼 (스타트버튼)
//    첫번째 누르면 캘리브레이션 후 Ready 모드
//    두번째 누르면 시작을 알리는 부저음
//    3초간 누르면 캘리브레이션 진행 후 Ready 모드 
// 차량 검출 효과를 위한 LED 적용
// 부저를 활용한 소리 효과
// 버튼 적용 캘리브레이션 / 모드
// 트랙 1 ~ 5
// Common Cathode RGB LED(-)

// PIN, 변수 선언
// HC-SR 04
int trigPin = 3;
int echoPin = 2;
// RGB LED
int red_light_pin= 9;
int green_light_pin = 10;
int blue_light_pin = 11;
// Button
int btnA = 6;
int btnB = 7;
// Buzzer
int buzzer = 8 ;// setting controls the digital IO foot buzzer

// Time variable
unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
unsigned long soundMillis; // buzzer 관련 시간 변수
unsigned long previousSndMillis; // buzzer 관련 이전 시간
unsigned long btnMillis; // button 관련 시간 변수
int inverval = 1000;

long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int cal_count = 0;
int gRound = 3; // Default round 3
int tover = 30; // Default time over 30 sec.
int detect_count = 0;

// notes in the melody:
int melody[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {8, 8, 8, 8};

int start_melody[] = {NOTE_B5, NOTE_B5, NOTE_B5, NOTE_B6};
int start_noteDurations[] = {2, 2, 2, 1};

// 함수 선언
long distance_check(); //측정된 거리값을 반환한다
long calibration_dist(); //캘리브레이션 된 거리 값을 반환한다
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);
void start_sound();
void display_status(int);
void display_time(unsigned long startMillis);
void display_round();

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // 센서 Trig 핀
  pinMode(echoPin, INPUT);  // 센서 Echo 핀
  pinMode(buzzer, OUTPUT) ;// set the digital IO pin mode, OUTPUT out of Wen
  pinMode(btnA, INPUT); // Pull_Down 방식 적용
  pinMode(btnB, INPUT); // Pull_Down 방식 적용
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();   
  Serial.begin(115200);  
  RGB_color(255, 0, 0); // Red
  delay(1000);
  RGB_color(0, 255, 0); // Green
  delay(1000);
  RGB_color(0, 0, 255); // Blue
  delay(1000);  
  // Status bar display
  display_status(0);  
  // display.clearDisplay();
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(0,0);
  // char buf[20];
  // snprintf(buf, sizeof(buf), "D %2d  R %d  T %2d ", int(cal_dist), gRound, tover);  
  // display.println(buf);  
  // display.display();    
}

void loop() {
  long temp_dist = distance_check() + 2;
  // display_status();
  display_time(millis());
  if(cal_dist >= temp_dist) {
    detect_count++;
    if(detect_count>3){
      Serial.println("Car Detected!");
      char buf[20];
      snprintf(buf, sizeof(buf), "Distance %4d cm", int(temp_dist-2));
      Serial.println(buf);
      delay(100);
      display_status(int(temp_dist-2));
      display_round();
      // display.clearDisplay();
      // display.setTextSize(1.8);
      // display.setTextColor(WHITE);
      // display.setCursor(0,0);
      // display.print("Average is = ");
      // display.println(temp_dist);
      // display.print("Average is = ");
      // display.println(temp_dist);
      // display.print("Average is = ");
      // display.println(temp_dist);
      // display.print("Average is = ");
      // display.println(temp_dist);                 
      // display.display();
      for(int i = 0; i < 10; i++) 
      {    
        RGB_color(255, 0, 0); // Red
        delay(20);
        RGB_color(0, 255, 0); // Green
        delay(20);
        RGB_color(0, 0, 255); // Blue
        delay(20);     
        RGB_color(0, 0, 0); // Blue
        delay(20); 
      }
      detect_count=0;
    }

    // // 부저 테스트 
    // int noteDuration = 1000 / 2;
    // tone(buzzer, NOTE_B6, noteDuration);
    // // to distinguish the notes, set a minimum time between them.
    // // the note's duration + 30% seems to work well:
    // int pauseBetweenNotes = noteDuration * 1.30;
    // delay(pauseBetweenNotes);
    // // stop the tone playing:
    // noTone(buzzer);    
 

  }
  // 캘리브레이션
  while (cal_count < 1) {
    cal_dist = calibration_dist();
    cal_count++;
    start_sound();
    display_status(0); 
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
  Serial.print("Checked distance is ==>");
  Serial.println(distance);
  return distance;
}

// 거리를 측정해서 값이 연속해서 3번 일치하면 캘리브레이션 완료된 것으로 적용
long calibration_dist() {
  currentMillis = millis();
  long dist_check = distance_check();
  Serial.println("### Start calibration ###");
  while(millis()-currentMillis < 4000) {
    if(dist_check == distance_check() && dist_check > 3) {
      Serial.print("### Calibration confirm between speed checker and track distance. ==>");
      Serial.println(dist_check);
      delay(1000);
    }
    else {
      currentMillis = millis();
      dist_check = distance_check();
      Serial.print("### On calibrating... ==> ");
      Serial.println(dist_check);
      delay(1000);
    }
  }
  Serial.print("### Calibration success ");
  Serial.print(int(dist_check));
  Serial.println(" cm" );
  return dist_check;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void start_sound() {
  unsigned long currentSndMillis = millis();
  for (int i = 0; i <4; i++) // Wen a frequency sound
  {
    previousSndMillis = currentSndMillis;
    int noteDuration = 1000 / start_noteDurations[i];
    tone(buzzer, start_melody[i], noteDuration);
    while (currentSndMillis - previousSndMillis <= inverval) {          
      currentSndMillis = millis();  
    }
    noTone(buzzer); 
  }     
}

void display_status(int measure) {
  // Status bar display
  // display.clearDisplay();
  display.fillRect(0,0,128,15,BLACK);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  char buf[20];
  snprintf(buf, sizeof(buf), "%02dcm[%02d]", int(cal_dist), measure);  
  display.println(buf);
  display.fillRoundRect(100,0,8,15,1,WHITE);  
  display.fillRoundRect(110,0,8,15,1,WHITE);  
  display.fillRoundRect(120,0,8,15,1,WHITE);  
  display.display(); 
}

void check_btn() {
  // Serial.print("Button A = ");
  // Serial.println(digitalRead(btnA));
  // delay(500);
  // Serial.print("Button B = ");
  // Serial.println(digitalRead(btnB));
  // delay(500);  
}

void display_time(unsigned long startMillis) {
  unsigned long min = startMillis/60000;
  unsigned long sec = startMillis/1000%60;
  unsigned long mils = startMillis%1000;
  mils = mils/10;
  // display.clearDisplay();
  display.fillRect(0,16,128,24,BLACK);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,16);
  char buf[20];
  snprintf(buf, sizeof(buf), "%1d:%02d:%02d", int(min), int(sec), int(mils));  
  display.println(buf);  
  display.display(); 
}

void display_round(){
// display.clearDisplay();
  display.fillRect(0,40,128,28,BLACK);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,40);
  // char buf[20];
  // snprintf(buf, sizeof(buf), "%02d:%02d:%02d", int(min), int(sec), int(mils));  
  // display.println(buf);  
  display.println("1St Round");
  display.display(); 
}