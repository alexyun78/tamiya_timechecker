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
// int btnB = 7;
// Buzzer
int buzzer = 12; //8 ;// setting controls the digital IO foot buzzer

///////////////////////////////////
// Variable Sstup
///////////////////////////////////
unsigned long previousRGBMillis = 0;
unsigned long time_total;
unsigned long time_round;
unsigned long firstlab, secondlab, thiredlab;

int RBG_count = 0;
long cal_dist = 0; // 측정기와 트랙간의 거리를 기록해서 저장
int gRound = 0; // Default round 3
int detect_count = 0; // 일시적으로 차량이 검출된 것을 카운트한다.
boolean btnA_flag = false; // button A 동작 확인
boolean car_detectOK = false;
int btnA_push = 0;
int car_dist = 0;
//0-None, 1-BT1, 2-BT2, 3-BT3, 4-
short status = 0; 

// // notes in the melody:
// int melody[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3};
// // note durations: 4 = quarter note, 8 = eighth note, etc.:
// int noteDurations[] = {8, 8, 8, 8};


///////////////////////////////////
// Function Declaration
///////////////////////////////////
int distance_check(); // 현재 측정한 거리값을 반환한다
int calibration_dist(); //캘리브레이션 측정 후, 거리 값을 반환한다
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);
void start_sound(); // 스타트 버튼을 눌렀을 때 사운드 출력
void display_status(int); // 디스플레이 상단의 상태를 보여준다. 캘리브레이션 된 거리, 현재 차량을 측정한 거리, 현재 진행중인 라운드[라운드박스]
String display_time(unsigned long startMillis); // 경기가 시작되고 진행된 전체 시간을 보여준다.
void display_round(); // 현재 라운드의 시간 기록을 보여줍니다. 1라운드, 2라운드, 3라운드의 각 시간을 보여준다 1R 0:00:00, 2R 0:00:00, 3R 0:00:00
void check_btn();
void car_detect(short);
void display_message();
void display_clear();
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
  Serial.begin(115200);    
//Set font.
// u8g.setFont(u8g_font_helvB12);
}

void loop() 
{    
  while(!btnA_flag) { // A 버튼을 누르지 않았다면
      if(btnA_push<=1) { 
        display_message();
      }
      check_btn();
    }
  // Serial.print(F("gRound = "));
  // Serial.println(gRound);   
  car_detect(3);
  if(gRound>=4) {
      thiredlab = millis() - time_round;
      Serial.print("firstlab = ");
      Serial.println(firstlab);
      Serial.print("secondlab = ");
      Serial.println(secondlab);
      Serial.print("thiredlab = ");
      Serial.println(thiredlab);        
      exit(0);
  }
    // long car_detect = distance_check() + 2;

  if(gRound == 1) {
      if(car_detectOK) time_total = time_round = millis();
      // display_message(3,display_time(millis()-time_total),display_time(millis()-time_round));
  }
  else if(gRound == 2) {
      if(car_detectOK) {
      firstlab = millis() - time_round;
      time_round = millis();
      }
      // display_message(3,display_time(millis()-time_total),display_time(millis()-time_round));
  }
  else if(gRound == 3) {
      if(car_detectOK) {
      secondlab = millis() - time_round;
      time_round = millis();
      }
      // display_message(3,display_time(millis()-time_total),display_time(millis()-time_round));
  }

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
///////////////////////////////////
// Display Function
///////////////////////////////////
  // u8g.firstPage();
  // do {
  // draw();
  // } while (u8g.nextPage());
  // //Delay before repeating the loop.
  // delay(50);
}

// void draw(void) {
//     //Write text. (x, y, text)
//     u8g.setFont(u8g_font_helvB12);
//     u8g.drawStr(20, 20, "Hello World.");
//     u8g.drawBox(10,12,20,30);
//     u8g.drawRBox(100,0,8,15,1);
//     u8g.setFont(u8g_font_gdb17);
//     u8g.drawStr(20, 50, "Hello World.");
// }

void car_detect(short x) {
     car_dist = distance_check() + 2;
    //  Serial.print(F("car_dist = "));
    //  Serial.println(car_dist);
    if(cal_dist >= car_dist) {    
        detect_count++;
        // Serial.println(detect_count);
        if(detect_count > x) { // 차량 검출 확인
          gRound++;
          car_detectOK = true;
          car_dist = car_dist - 2;
          status++;
          // display_status(car_dist-2);
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
  unsigned long currentMillis; // millis() 경과한 시간을 밀리 초로 반환한다.
  currentMillis = millis();
  int dist_check = distance_check();
  // Serial.println(F("### Start calibration ###"));
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
      u8g.drawStr(0, 60, "In Progress...");     
      } while(u8g.nextPage());   
      check_count++;
      // Serial.print(F("### Calibration confirm between speed checker and track distance. ==>"));
      Serial.println(dist_check);
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
  u8g.firstPage();
  do { 
  u8g.setFont(u8g_font_helvB12);
  u8g.drawStr(60, 15, String(dist_check).c_str());
  // u8g.setFont(u8g_font_helvB10);
  u8g.drawStr(0, 35, "Calibration");  
  u8g.drawStr(0, 60, "Completed !!!");     
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
    int noteDuration = 1000 / start_noteDurations[i];
    tone(buzzer, start_melody[i], noteDuration);  
    // if(i==3) display_message(2," Let's GO"," START !!!");
    // else display_message(3,"   "+(String)(3-i)," Ready ");
    while (currentSndMillis - previousSndMillis <= 1000) {          
      car_detect(4); // 출발 중
      // Serial.print(F("car_detectOK = "));
      // Serial.println(car_detectOK);       
      // Serial.print(F("gRound = "));
      // Serial.println(gRound); 
      if(car_detectOK) return;
      currentSndMillis = millis();  
    }
    noTone(buzzer); 
  }     
}

void display_status(int measure) {
  // Status bar display
  // u8g.setColorIndex(0);
  // u8g.drawBox(0,0,128,15);
  // display.setTextSize(2);
  // display.setTextColor(WHITE);
  // u8g.drawStr(0,0);
  char buf[10];
  snprintf(buf, sizeof(buf), "%02dcm[%02d]", int(cal_dist), measure);  
  // display.println(buf);
  switch(gRound){
    case 1:
      // display.fillRoundRect(100,0,8,15,1,WHITE);
      // Serial.print(F("gRound = 1 = "));
      // Serial.println(gRound);
      break;
    case 2:
      // display.fillRoundRect(100,0,8,15,1,WHITE);
      // display.fillRoundRect(110,0,8,15,1,WHITE);  
      // Serial.print(F("gRound = 2 = "));
      // Serial.println(gRound);
      break;
    case 3:
      // display.fillRoundRect(100,0,8,15,1,WHITE);
      // display.fillRoundRect(110,0,8,15,1,WHITE);      
      // display.fillRoundRect(120,0,8,15,1,WHITE); 
      // Serial.print(F("gRound = 3 = "));
      // Serial.println(gRound);      
      break;  
  }
  // display.display();
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
  Serial.println(btnA_push);  
  if(btnA_push==1) {       
        // cal_dist = calibration_dist();
        // display_clear();      
        // u8g.firstPage();
        // do {
        // // display_message(2,"  Measure", "Completed!");
        // display_status(0);
        // } while (u8g.nextPage());    
        // delay(3000);
        // do {
        // // display_message(2,"Push A Btn", " TO START!");
        // } while (u8g.nextPage());    
    } 
  else if(btnA_push==2) {
      start_sound();
      btnA_flag = true;
      // display.clearDisplay();
      // display.display();
    }  
}

String display_time(unsigned long startMillis) {
  int min = startMillis/60000;
  int sec = startMillis/1000%60;
  int mils = startMillis%1000;
  mils = mils/10;
  // display.clearDisplay();
  // u8g.drawBox(0,16,128,24,BLACK);
  // display.setTextSize(3);
  // display.setTextColor(WHITE);
  // u8g.drawStr(0,16);
  char buf[10];
  snprintf(buf, sizeof(buf), "%1d:%02d:%02d", min, sec, mils); 
  // Serial.println(buf);
  return buf;
  // display.println(buf);  
  // display.display(); 
}

void display_round(){
// display.clearDisplay();
  if(gRound==1) {
    // u8g.drawBox(0,40,128,28);
    // u8g.drawStr(0,40);
    // char buf[20];
    // snprintf(buf, sizeof(buf), "%02d:%02d:%02d", int(min), int(sec), int(mils));  
    // display.println(buf);  
    // display.println("1St Round");
    // display.display(); 
  }
}

void display_message() 
{
  int size = 2;
  int msg1 = 1;
  int msg2 = 2;
    switch(btnA_push) {
      case 0: // 처음 부팅 화면
        Serial.println("btnA_push 0");
        u8g.firstPage();
        do {
          u8g.setFont(u8g_font_helvB10);
          u8g.drawStr(0, 35, "Press The Button");  
          u8g.drawStr(0, 60, "To Calibrate."); 
        } while(u8g.nextPage());
        break;
      case 1: // 버튼 1번 눌렀을 때
        Serial.println("btnA_push 1");
        cal_dist = calibration_dist(); 
        u8g.firstPage();
        do { 
        u8g.setFont(u8g_font_helvB12);
        u8g.drawStr(60, 15, String(cal_dist).c_str());
        u8g.setFont(u8g_font_helvB10);
        u8g.drawStr(0, 35, "Press The Button"); 
        u8g.setFont(u8g_font_helvB12); 
        u8g.drawStr(0, 60, "To START !!!");     
        } while(u8g.nextPage());                  
        break;
      case 2:
        Serial.println("btnA_push 2");
        if(size == 2) u8g.setFont(u8g_font_helvB12);
        else if(size == 3) u8g.setFont(u8g_font_gdb17);
        if(msg1 != 0) {
            u8g.drawBox(0,16,128,24);
            u8g.drawStr(0, 16, "test1");
            // u8g.drawBox(0,16,128,24,BLACK);        
            // u8g.drawStr(0,16);  
            // display.print(msg1);
        }
        if(msg2 != 0) {
            u8g.drawBox(0,40,128,24);
            u8g.drawStr(0, 40, "test2");      
            // u8g.drawBox(0,40,128,24,BLACK);
            // u8g.drawStr(0,40);
            // display.print(msg2);       
        }
        break;
    } 
  
}


void display_clear() {
  u8g.firstPage();
  do {
  } while (u8g.nextPage());   
}