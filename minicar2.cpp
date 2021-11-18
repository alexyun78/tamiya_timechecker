#include <Arduino.h>
#include "U8glib-HAL.h" 
#include "pitches.h"

//Initialize display.
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

///////////////////////////////////
// PIN Sstup
///////////////////////////////////
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
int buzzer = 8; //8 ;// setting controls the digital IO foot buzzer

///////////////////////////////////
// Variable Sstup
///////////////////////////////////
unsigned long time_total;
unsigned long round_interval = 0;
unsigned long now_time;
String firstlab;
String secondlab;
String thirdlab;

long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int gRound = 0; // Default round 3
int detect_count = 0; // 일시적으로 차량이 검출된 것을 카운트한다.
boolean btnA_flag = false; // button A 동작 확인
boolean car_detectOK = false;
int btnA_push = 0;
int car_dist = 0;
int car_dist_show = 0;
//0-None, 1-BT1, 2-BT2, 3-BT3, 4-

// // notes in the melody:
// int melody[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3};
// // note durations: 4 = quarter note, 8 = eighth note, etc.:
// int noteDurations[] = {8, 8, 8, 8};


///////////////////////////////////
// Function Declaration
///////////////////////////////////
int distance_check(); // 현재 측정한 거리값을 반환한다
int calibration_dist(); //캘리브레이션 측정 후, 거리 값을 반환한다
void RGB_color(int, int, int);
void start_sound(); // 스타트 버튼을 눌렀을 때 사운드 출력
String display_time(unsigned long); // 경기가 시작되고 진행된 전체 시간을 보여준다.
void display_round(); // 현재 라운드의 시간 기록을 보여줍니다. 1라운드, 2라운드, 3라운드의 각 시간을 보여준다 1R 0:00:00, 2R 0:00:00, 3R 0:00:00
void check_btn();
void car_detect(short);
void car_detect2(short);
void display_message();
void check_btnB();
// void draw(void);

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // 센서 Trig 핀
  pinMode(echoPin, INPUT);  // 센서 Echo 핀
  pinMode(buzzer, OUTPUT) ;// set the digital IO pin mode, OUTPUT out of Wen
  pinMode(btnA, INPUT); // Pull_Down 방식 적용
  // pinMode(btnB, INPUT); // Pull_Down 방식 적용

  RGB_color(255, 0, 0); // Red
  delay(500);
  RGB_color(0, 255, 0); // Green
  delay(500);
  RGB_color(0, 0, 255); // Blue
  delay(500);
  RGB_color(0, 0, 0); // OFF
  Serial.begin(115200);    
//Set font.
// u8g.setFont(u8g_font_helvB12);
}

void loop() 
{    
  while(!btnA_flag) { // A 버튼을 누르지 않았다면
      if(btnA_push<=1) { 
        display_message();
        check_btn();
      }
      else if(btnA_push==2) {
        start_sound();
        btnA_flag = true;
      }
    }
  if(round_interval == 0) {
    car_detect2(3);
  }
  if(gRound>=4) {
    char buf[20]; 
    char buf1[20]; 
    char buf2[20]; 
    // Serial.print(F("firstlab = "));
    // Serial.print(firstlab);
    // Serial.print(F("\tsecondlab = "));
    // Serial.print(secondlab);
    // Serial.print(F("\tthirdlab = "));
    // Serial.println(thirdlab);
    snprintf(buf, sizeof(buf), "1St ==> %s", firstlab.c_str());
    snprintf(buf1, sizeof(buf1), "2nd ==> %s", secondlab.c_str()); 
    snprintf(buf2, sizeof(buf2), "3rd ==> %s", thirdlab.c_str()); 
    // firstlab = "1St    " + firstlab;
    // secondlab = "2nd    " + secondlab;
    // thirdlab = "3rd    " + thirdlab;
    u8g.firstPage();
    do {
    u8g.setFont(u8g_font_helvB12);
    u8g.drawStr(0, 13, buf);      
    u8g.drawStr(0, 35, buf1);
    u8g.drawStr(0, 55, buf2);   
    } while(u8g.nextPage());                
    delay(15);
    check_btnB();
  }
  if(gRound == 1) {    
    if(car_detectOK) {
      time_total = millis();
      car_detectOK = false;
    }
    // Serial.print(F("cal_dist = "));
    // Serial.print(cal_dist);
    // Serial.print(F("\tcar_dist = "));
    // Serial.print(car_dist);      
    // Serial.print(F("\tdetect_count = "));
    // Serial.print(detect_count);
    // Serial.print(F("\tgRound = "));
    // Serial.println(gRound);    
    unsigned long tmp_time = millis(); 
    String totaltime = display_time(tmp_time - time_total);
    firstlab = totaltime;
    char buf[20]; 
    if(((tmp_time - time_total) < round_interval) && round_interval) {
      char buf1[20];
      char buf2[20];
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show)); 
      snprintf(buf1, sizeof(buf1), "Total   %s", totaltime.c_str());
      snprintf(buf2, sizeof(buf2), "1st R   %s", firstlab.c_str());
      u8g.firstPage();
      do { 
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(0, 35, buf1);
      u8g.drawStr(0, 55, buf2);   
      } while(u8g.nextPage());
    } else if(round_interval) {
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show));
      u8g.firstPage();
      do {
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(15, 35, "First Round");
      u8g.drawStr(15, 55, "In Progress");   
      } while(u8g.nextPage());      
      round_interval = 0; 
      }
  }
  else if(gRound == 2) {
    // unsigned long now_time;
    unsigned long cal_time=0;
    if(car_detectOK) {
      now_time = millis();
      car_detectOK = false;
    }
    unsigned long tmp_time = millis();       
    String totaltime = display_time((tmp_time - time_total));
    // Serial.print("tmp_time  ");
    // Serial.println(tmp_time);
    // Serial.print("time_total  ");
    // Serial.print(time_total);
    Serial.print("now_time  ");
    Serial.println(now_time);
    cal_time = tmp_time - now_time;
    secondlab = display_time(cal_time);
    char buf[20];
    if(((tmp_time - now_time) < round_interval) && round_interval) {
      char buf1[20];
      char buf2[20];
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show));
      snprintf(buf1, sizeof(buf1), "Total   %s", totaltime.c_str());
      snprintf(buf2, sizeof(buf2), "2nd R   %s", secondlab.c_str());
      u8g.firstPage();
      do { 
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(0, 35, buf1);
      u8g.drawStr(0, 55, buf2);   
      } while(u8g.nextPage());
    } else if(round_interval) {
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show));
      u8g.firstPage();
      do {
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(0, 35, "Second Round");
      u8g.drawStr(15, 55, "In Progress");   
      } while(u8g.nextPage());      
      round_interval = 0; 
      }
  }
  else if(gRound == 3) {
    // unsigned long now_time;
    unsigned long cal_time=0;
    if(car_detectOK) {
      now_time = millis();
      car_detectOK = false;
    }
    unsigned long tmp_time = millis();        
    String totaltime = display_time((tmp_time - time_total));  
    Serial.print("now_time  ");
    Serial.println(now_time);    
    cal_time = tmp_time - now_time;  
    thirdlab = display_time(cal_time);
    char buf[20];
    if(((tmp_time - now_time) < round_interval) && round_interval) {
      char buf1[20];
      char buf2[20];
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show));
      snprintf(buf1, sizeof(buf1), "Total   %s", totaltime.c_str());
      snprintf(buf2, sizeof(buf2), "3rd R   %s", thirdlab.c_str());
      u8g.firstPage();
      do { 
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(0, 35, buf1);
      u8g.drawStr(0, 55, buf2);   
      } while(u8g.nextPage());
    } else if(round_interval) {
      snprintf(buf, sizeof(buf), "H:%02d Now:%02dcm", int(cal_dist), int(car_dist_show));
      u8g.firstPage();
      do {
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 13, buf);      
      u8g.drawStr(15, 35, "Last Round");
      u8g.drawStr(15, 55, "In Progress");   
      } while(u8g.nextPage());      
      round_interval = 0; 
      }
  }
}

void car_detect(short x) {
  car_dist = distance_check() + 2;
  Serial.print(F("cal_dist = "));
  Serial.print(cal_dist);  
  Serial.print(F("\tcar_dist = "));
  Serial.println(car_dist);
  if(cal_dist > car_dist) {
    // Serial.print(F("cal_dist = "));
    // Serial.print(cal_dist);
    // Serial.print(F("\tcar_dist = "));
    // Serial.print(car_dist);           
    // detect_count++;
    // Serial.print(F("\tdetect_count = "));
    // Serial.println(detect_count);  
    if(detect_count > x) { // 차량 검출 확인
      // Serial.print(F("======================== Car Detected 1111111 "));           
      car_detectOK = true;
      car_dist_show = car_dist - 2;
      detect_count=0;
    }
  }
  else detect_count=0;
}

void car_detect2(short x) {  
  car_dist = distance_check() + 2;
  // Serial.print(F("cal_dist = "));
  // Serial.print(cal_dist);
  // Serial.print(F("\tcar_dist = "));
  // Serial.print(car_dist);      
  // Serial.print(F("\tdetect_count = "));
  // Serial.print(detect_count);
  // Serial.print(F("\tgRound = "));
  // Serial.print(gRound);
  Serial.print(F("\tfirstlab = "));
  Serial.print(firstlab);
  Serial.print(F("\tsecondlab = "));
  Serial.print(secondlab);
  Serial.print(F("\tthirdlab = "));
  Serial.println(thirdlab);           
  if(cal_dist > car_dist) {
    detect_count++;
    // Serial.println(F("======================== Car Detected 2 ")); 
    if(detect_count > x) { // 차량 검출 확인
      // Serial.println(F("======================== Car Detected 222222222 "));  
      gRound++;       
      car_detectOK = true;
      car_dist_show = car_dist - 2;
      detect_count=0;
      round_interval = 5000;
      if(gRound==1) RGB_color(0, 0, 255); // Blue
      else if(gRound==2) RGB_color(0, 255, 0); // Green
      else RGB_color(255, 0, 0); // Red 
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
  return distance;
}

// 거리를 측정해서 값이 연속해서 3번 일치하면 캘리브레이션 완료된 것으로 적용
int calibration_dist() {
  unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
  currentMillis = millis();
  int dist_check = distance_check();
  int check_count =0;
  String cal_dist_sum = "";
  while(millis()-currentMillis < 4000) {
    car_dist = distance_check();   
    if(dist_check == car_dist && dist_check > 3) {
      cal_dist_sum += String(dist_check) + "    ";
      u8g.firstPage();
      do { 
      u8g.setFont(u8g_font_helvB12);
      u8g.drawStr(0, 15, cal_dist_sum.c_str());
      u8g.setFont(u8g_font_helvB10);
      u8g.drawStr(0, 35, "Calibration");  
      u8g.drawStr(0, 55, "In Progress...");     
      } while(u8g.nextPage());   
      check_count++;
      // Serial.println(dist_check);
      delay(1000);
    }
    else {
      currentMillis = millis();
      dist_check = car_dist;
      check_count = 0;
      cal_dist_sum = "";
      delay(1000);
    }
  }
  delay(500);
  char buf[20];
  u8g.firstPage();
  do { 
  u8g.setFont(u8g_font_helvB12);
  snprintf(buf, sizeof(buf), "Height:  %02d cm", int(dist_check));
  // u8g.drawStr(0, 15, "Height is " + String(dist_check).c_str() + "cm");
  u8g.drawStr(5, 12, buf);
  // u8g.setFont(u8g_font_helvB10);
  u8g.drawStr(0, 35, "Calibration");  
  u8g.drawStr(0, 55, "Completed !!!");     
  } while(u8g.nextPage());     
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
  unsigned long previousSndMillis; // buzzer 관련 이전 시간
  unsigned long currentSndMillis = millis();
  int start_melody[] = {NOTE_B5, NOTE_B5, NOTE_B5, NOTE_B6};
  int start_noteDurations[] = {2, 2, 2, 1};
  for (int i = 0; i <4; i++) // Wen a frequency sound
  {
    previousSndMillis = currentSndMillis; 
    u8g.firstPage();
    do { 
    u8g.setFont(u8g_font_helvB12);
    u8g.drawStr(40, 15, "READY");
    u8g.setFont(u8g_font_helvB12);
    u8g.drawStr(50, 55, String(3-i).c_str());    
    } while(u8g.nextPage());    
    int noteDuration = 1000 / start_noteDurations[i];
    tone(buzzer, start_melody[i], noteDuration);      
    while (currentSndMillis - previousSndMillis <= 1000) {               
      car_detect(3); // 출발 중
      // Serial.print(F("===========> cal_dist = "));
      // Serial.print(cal_dist);  
      // Serial.print(F("\tcar_dist = "));
      // Serial.print(car_dist);       
      // Serial.print(F("\tdetect_count = "));
      // Serial.println(detect_count);        
      if(car_detectOK) {
        gRound++;
        round_interval = 5000;
        return;
      }
      currentSndMillis = millis();  
    }
    noTone(buzzer); 
  }
  char buf[20];
  snprintf(buf, sizeof(buf), "Height:  %02d cm", int(cal_dist));  
  u8g.firstPage();
  do { 
  u8g.setFont(u8g_font_helvB12);
  u8g.drawStr(5, 12, buf);
  u8g.drawStr(30, 35, "Let's go"); 
  u8g.drawStr(25, 55, "START !!!");
  } while(u8g.nextPage());
  Serial.println(F("===========> START !!!!!!!!!!!!!!!!!! "));
}

void check_btn() {
  unsigned long btnMillis = millis();
  unsigned long previousbtnMillis = btnMillis;
  while(btnMillis-previousbtnMillis < 200) {
    btnMillis = millis();
    if(digitalRead(btnA)==0) {      
      previousbtnMillis = btnMillis;
    }
  }
  btnA_push++;
}

void check_btnB() {
  unsigned long btnMillis = millis();
  unsigned long previousbtnMillis = btnMillis;
  while(btnMillis-previousbtnMillis < 200) {
    btnMillis = millis();
    if(digitalRead(btnB)==0) {      
      previousbtnMillis = btnMillis;
    }
  }
  // Reset All data
  btnA_push = 0;
  gRound = 0;
  btnA_flag = false;
  car_dist = 0;
  car_detectOK = false;
  round_interval = 0;  
}

String display_time(unsigned long startMillis) {
  Serial.print("startMillis \t");
  Serial.println(startMillis);
  unsigned int min = startMillis/60000;
  unsigned int sec = (startMillis/1000)%60;
  unsigned int mils = (startMillis%1000)/10;
  // mils = mils/10;
  char buf5[20];
  snprintf(buf5, sizeof(buf5), "%1d:%02d:%02d", min, sec, mils); 
  return buf5;
}

void display_message() 
{
    switch(btnA_push) {
      case 0: // 처음 부팅 화면
        u8g.firstPage();
        do {
          u8g.setFont(u8g_font_helvB10);
          u8g.drawStr(0, 35, "Press The Button");  
          u8g.drawStr(0, 55, "To Calibrate."); 
        } while(u8g.nextPage());
        break;
      case 1: // 버튼 1번 눌렀을 때
        char buf[20];
        cal_dist = calibration_dist(); 
        snprintf(buf, sizeof(buf), "Height:  %02d cm", int(cal_dist));
        u8g.firstPage();
        do { 
        u8g.setFont(u8g_font_helvB12);
        u8g.drawStr(5, 12, buf);
        u8g.setFont(u8g_font_helvB10);
        u8g.drawStr(0, 35, "Press The Button"); 
        u8g.setFont(u8g_font_helvB12); 
        u8g.drawStr(0, 55, "To START !!!");     
        } while(u8g.nextPage());                  
        break;
    } 
}