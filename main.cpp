#include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
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
// int btnB = 7;
// Buzzer
int buzzer = 8; //8 ;// setting controls the digital IO foot buzzer
int RBG_count = 0;

// Time variable
unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
unsigned long soundMillis; // buzzer 관련 시간 변수
unsigned long previousSndMillis; // buzzer 관련 이전 시간
unsigned long btnMillis; // button 관련 시간 변수
unsigned long previousRGBMillis = 0;

long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int gRound = 0; // Default round 3
int detect_count = 0; // 일시적으로 차량이 검출된 것을 카운트한다.
boolean btnA_flag = false; // button A 동작 확인
boolean car_detectOK = false;
int btnA_push = 0;
// int x, minX;

// // notes in the melody:
// int melody[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3};
// // note durations: 4 = quarter note, 8 = eighth note, etc.:
// int noteDurations[] = {8, 8, 8, 8};


// 함수 선언
int distance_check(); // 현재 측정한 거리값을 반환한다
int calibration_dist(); //캘리브레이션 측정 후, 거리 값을 반환한다
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);
void start_sound(); // 스타트 버튼을 눌렀을 때 사운드 출력
void display_status(int); // 디스플레이 상단의 상태를 보여준다. 캘리브레이션 된 거리, 현재 차량을 측정한 거리, 현재 진행중인 라운드[라운드박스]
String display_time(unsigned long startMillis); // 경기가 시작되고 진행된 전체 시간을 보여준다.
void display_round(); // 현재 라운드의 시간 기록을 보여줍니다. 1라운드, 2라운드, 3라운드의 각 시간을 보여준다 1R 0:00:00, 2R 0:00:00, 3R 0:00:00
void check_btn();
void car_detect(short);
void display_message(int, String, String);

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // 센서 Trig 핀
  pinMode(echoPin, INPUT);  // 센서 Echo 핀
  pinMode(buzzer, OUTPUT) ;// set the digital IO pin mode, OUTPUT out of Wen
  pinMode(btnA, INPUT); // Pull_Down 방식 적용
  // pinMode(btnB, INPUT); // Pull_Down 방식 적용
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
  // x = display.width();
  // Status bar display
  // display_status(0);  
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
  while(!btnA_flag) { // A 버튼을 누르지 않았다면
    if(btnA_push<1) { 
      display_message(2, "Push A Btn"," To Check.");
    }
    check_btn();
  }
  Serial.print(F("gRound = "));
  Serial.println(gRound);   
  car_detect(3);
  if(gRound>3) {
    exit(0);
  }
  // long temp_dist = distance_check() + 2;
  
  if(gRound>=1) display_message(3,display_time(millis()),"NULL");



  // Serial.print(F("car_detectOK = "));
  // Serial.print(car_detectOK);
  // Serial.print(F("RBG_count = "));
  // Serial.print(RBG_count);
  // 차량이 검출된 상태에서 RGB LED 동작
  if(car_detectOK && RBG_count < 3) {
    unsigned long currentRGBMillis = millis();
    if(currentRGBMillis-previousRGBMillis < 20 ) {
      RGB_color(255, 0, 0); // Red 
      // Serial.println(F("RED -------------------"));
    }
    else if(currentRGBMillis-previousRGBMillis < 40 ) {
      RGB_color(0, 255, 0); // Green
      // Serial.println(F("Green -------------------"));
    }
    else if(currentRGBMillis-previousRGBMillis < 60 ) {
      RGB_color(0, 0, 255); // Blue
      // Serial.println(F("Blue -------------------"));
    }
    else {
      RGB_color(0, 0, 0); // OFF
      RBG_count++;
      previousRGBMillis = currentRGBMillis;
    }
  }
  else if(RBG_count>3) {
    car_detectOK = false;
  }


  // if(cal_dist >= temp_dist) {
  //   detect_count++;
  //   if(detect_count>3){
  //     Serial.println(F("Car Detected!"));
  //     char buf[20];
  //     snprintf(buf, sizeof(buf), "Distance %4d cm", int(temp_dist-2));
  //     Serial.println(F(buf));
  //     delay(100);
  //     display_status(int(temp_dist-2));
  //     display_round();
  //     for(int i = 0; i < 10; i++) 
  //     {    
  //       RGB_color(255, 0, 0); // Red
  //       delay(20);
  //       RGB_color(0, 255, 0); // Green
  //       delay(20);
  //       RGB_color(0, 0, 255); // Blue
  //       delay(20);     
  //       RGB_color(0, 0, 0); // OFF
  //       delay(20); 
  //     }
  //     detect_count=0;
  //   }
  // }


    // // 부저 테스트 
    // int noteDuration = 1000 / 2;
    // tone(buzzer, NOTE_B6, noteDuration);
    // // to distinguish the notes, set a minimum time between them.
    // // the note's duration + 30% seems to work well:
    // int pauseBetweenNotes = noteDuration * 1.30;
    // delay(pauseBetweenNotes);
    // // stop the tone playing:
    // noTone(buzzer);    
 
  // // 캘리브레이션
  // while (cal_count < 1) {
  //   cal_dist = calibration_dist();
  //   cal_count++;
  //   start_sound();
  //   display_status(0); 
  // }   
}
void car_detect(short x) {
  long temp_dist = distance_check() + 2;
  //  Serial.print(F("temp_dist = "));
  //  Serial.println(temp_dist);
  if(cal_dist >= temp_dist) {    
    detect_count++;
    // Serial.println(detect_count);
    if(detect_count>x) { // 차량 검출 확인
      gRound++;
      car_detectOK = true;
      display_status(int(temp_dist-2));
      display_round();
      RBG_count=0;
      detect_count=0;
    }
  }
  else detect_count=0;
}


// 거리를 측정해서 거리값을 반환한다
int distance_check() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, HIGH);  // 센서에 Trig 신호 입력
  delayMicroseconds(10);        // 10us 정도 유지
  digitalWrite(trigPin, LOW);   // Trig 신호 off

  int duration = pulseIn(echoPin, HIGH);    // Echo pin: HIGH->Low 간격을 측정
  int distance = duration / 29 / 2;         // 거리(cm)로 변환 
  // Serial.print(F("Checked distance is ==>"));
  // Serial.println(distance);
  return distance;
}

// 거리를 측정해서 값이 연속해서 3번 일치하면 캘리브레이션 완료된 것으로 적용
int calibration_dist() {
  currentMillis = millis();
  int dist_check = distance_check();
  display_message(2, " On doing", " Measure");
  // Serial.println(F("### Start calibration ###"));
  int check_count =0;
  while(millis()-currentMillis < 4000) {
    int temp_dist = distance_check();
    if(dist_check == temp_dist && dist_check > 3) {
      display.setTextSize(2);
      display.setCursor(0+check_count*30,0);
      display.print(dist_check);
      check_count++;
      // Serial.print(F("### Calibration confirm between speed checker and track distance. ==>"));
      // Serial.println(dist_check);
      delay(1000);
    }
    else {
      currentMillis = millis();
      dist_check = temp_dist;
      // Serial.print(F("### On calibrating... ==> "));
      check_count = 0;
      display.fillRect(0,0,128,15,BLACK);
      // Serial.println(dist_check);
      delay(1000);
    }
    display.display();
  }
  // Serial.print(F("### Calibration success "));
  // Serial.print(int(dist_check));
  // Serial.println(F(" cm" ));
  delay(2000);
  return dist_check;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

// 스타트 버튼을 누르면 진행되는 사운드 출력
// 사운드 출력 중, 차량 검출하면 바로 게임 시작
// 사운드 출력 완료후, 차량 검출 대기
void start_sound() {
  unsigned long currentSndMillis = millis();
  int start_melody[] = {NOTE_B5, NOTE_B5, NOTE_B5, NOTE_B6};
  int start_noteDurations[] = {2, 2, 2, 1};
  for (int i = 0; i <4; i++) // Wen a frequency sound
  {
    previousSndMillis = currentSndMillis;
    int noteDuration = 1000 / start_noteDurations[i];
    tone(buzzer, start_melody[i], noteDuration);  
    if(i==3) display_message(2," Let's GO"," START !!!");
    else display_message(3,"   "+(String)(3-i)," Ready ");
    while (currentSndMillis - previousSndMillis <= 1000) {          
      car_detect(4); // 출발 중
      Serial.print(F("car_detectOK = "));
      Serial.println(car_detectOK);       
      Serial.print(F("gRound = "));
      Serial.println(gRound); 
      if(car_detectOK) return;
      currentSndMillis = millis();  
    }
    noTone(buzzer); 
  }     
}

void display_status(int measure) {
  // Status bar display
  display.fillRect(0,0,128,15,BLACK);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  char buf[10];
  snprintf(buf, sizeof(buf), "%02dcm[%02d]", int(cal_dist), measure);  
  display.println(buf);
  switch(gRound){
    case 1:
      display.fillRoundRect(100,0,8,15,1,WHITE);
      // Serial.print(F("gRound = 1 = "));
      // Serial.println(gRound);
      break;
    case 2:
      display.fillRoundRect(100,0,8,15,1,WHITE);
      display.fillRoundRect(110,0,8,15,1,WHITE);  
      // Serial.print(F("gRound = 2 = "));
      // Serial.println(gRound);
      break;
    case 3:
      display.fillRoundRect(100,0,8,15,1,WHITE);
      display.fillRoundRect(110,0,8,15,1,WHITE);      
      display.fillRoundRect(120,0,8,15,1,WHITE); 
      // Serial.print(F("gRound = 3 = "));
      // Serial.println(gRound);      
      break;  
  }
  display.display();
}

void check_btn() {
  btnMillis = millis();
  unsigned long previousbtnMillis = btnMillis;
  while(btnMillis-previousbtnMillis < 200) {
    btnMillis = millis();
    if(digitalRead(btnA)==0) {      
      previousbtnMillis = btnMillis;
    }
  }
  btnA_push++;
  // Serial.println(btnA_push);  
  if(btnA_push==1) {        
    cal_dist = calibration_dist();
    display_message(2,"  Measure", "Completed!");
    display_status(0);
    delay(3000);
    display_message(2,"Push A Btn", " TO START!");  } 
  else if(btnA_push==2) {        
    start_sound();
    btnA_flag = true;
    // display.clearDisplay();
    // display.display();
  }  
}

String display_time(unsigned long startMillis) {
  unsigned long min = startMillis/60000;
  unsigned long sec = startMillis/1000%60;
  unsigned long mils = startMillis%1000;
  mils = mils/10;
  // display.clearDisplay();
  // display.fillRect(0,16,128,24,BLACK);
  // display.setTextSize(3);
  // display.setTextColor(WHITE);
  // display.setCursor(0,16);
  char buf[10];
  snprintf(buf, sizeof(buf), "%1d:%02d:%02d", int(min), int(sec), int(mils)); 
  // Serial.println(buf);
  return buf;
  // display.println(buf);  
  // display.display(); 
}

void display_round(){
// display.clearDisplay();
  if(gRound==1) {
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
}

void display_message(int size, String msg1, String msg2) {
  display.setTextSize(size);
  display.setTextColor(WHITE);
  if(msg1 != "NULL") {
    display.fillRect(0,16,128,24,BLACK);        
    display.setCursor(0,16);  
    display.print(msg1);
  }
  if(msg2 != "NULL") {
    display.fillRect(0,40,128,24,BLACK);
    display.setCursor(0,40);
    display.print(msg2);       
  }
  display.display(); 
}