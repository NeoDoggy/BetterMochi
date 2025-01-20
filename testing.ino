#include "settings.h"

#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <TFT_eSPI.h>             
#include <AnimatedGIF.h>
#include <Arduino.h>
#include <Preferences.h>
#include "TouchDrvFT6X36.hpp"
#include <Wire.h>
#include "RTClib.h"
#include <Time.h>

#include "images/face1_lossy200.h"
#include "images/face2_lossy200.h"
#include "images/face3_lossy200.h"

#define TP_IRQ 14
#define TP_SCL 12
#define TP_SDA 11
#define RTC_SCL 12
#define RTC_SDA 11
#define GPSRX 1 //GND 5V G2 G1  B R W Y
#define GPSTX 2


#define threshold 60
#define gifs 3
#define pages 3
#define loop1 loop

AnimatedGIF gif;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
Preferences settings;
TouchDrvFT6X36 touch;
RTC_PCF8563 rtc;
DateTime now;
TaskHandle_t Task2;
uint8_t snderADDR[] = {0xFC, 0xE8, 0xC0, 0xA5, 0xDD, 0x10};//fc:e8:c0:a5:dd:10

typedef struct struct_message {
  int speed;
  char nowtime[15];                       // 202501081541006 => 15
} struct_message;

struct_message enData;

extern int segments;

int tftBrightness=255;
int speed=0;
int nowGif=0;
int nowPage=0;

bool playok=1;
bool playtone=0;
bool isTouched=0;
bool gifWaitStarted=0;
bool case0init=0;
bool case1init=0;
bool case2init=0;
bool timeinit=0;

const int TONE_OUTPUT_PIN = 3;
const int TONE_PWM_CHANNEL = 0;

unsigned long waitTime;
unsigned long startGIFTime;

// nowtime
String nowtime="202501010000003"; // 202501081541006 => 15

void rtcInit(){
  rtc.adjust(DateTime(nowtime.substring(0,4).toInt(),
                      nowtime.substring(4,6).toInt(),
                      nowtime.substring(6,8).toInt(),
                      nowtime.substring(8,10).toInt(),
                      nowtime.substring(10,12).toInt(),
                      nowtime.substring(12,14).toInt()
                      ));
  now=rtc.now();
  if(nowtime!="202501010000003")
    timeinit=1;
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&enData, incomingData, sizeof(enData));
  speed=enData.speed;
  nowtime=String(enData.nowtime);

  // init RTC
  if(!timeinit)rtcInit();

  Serial.print("now speed:");
  Serial.println(enData.speed);
  Serial.print("now time:");
  Serial.println(nowtime);
}


void loop0(void * pvParameters ){
  while(1){
    if(digitalRead(TP_IRQ)==LOW){
      int16_t x[2], y[2];
      uint8_t touched = touch.getPoint(x, y, 2);
      for (int i = 0; i < touched; ++i) {
        Serial.print("X[");Serial.print(i);Serial.print("]:");Serial.print(x[i]);Serial.print(" ");Serial.print(" Y[");Serial.print(i);Serial.print("]:");Serial.print(y[i]);Serial.print(" ");
        tftBrightness=x[i];
        analogWrite(9, x[i]);
      }
      Serial.println();
    }
    if(playtone){
      if(digitalRead(41)==digitalRead(40)){ // clockwize
        Serial.println("CW");
        ++nowPage%=pages;
        for (int i=1; i<20; i++) {
          ledcWriteTone(TONE_PWM_CHANNEL, i * 100);
          delay(10);    
        }
      }
      else{
        Serial.println("CCW");
        if(nowPage-1<0)nowPage=pages-1;
        else nowPage--;
        for (int i=19; i>0; i--) {
          ledcWriteTone(TONE_PWM_CHANNEL, i * 100);
          delay(10);    
        }
      }
      ledcWrite(TONE_PWM_CHANNEL, 0); // Stop tone 
      playtone=0;
      settings.putInt("pgcnt", nowPage);
    }
    delay(1);
  }
}

// rotary encoder callback
void turnedCallBack(){
  playtone=1;
  playok=0;
  // Serial.println(nowPage);
}

void setup() {
  // Serial
  Serial.begin(115200);

  // I2C
  Wire.begin(11, 12); 
  rtc.begin();
  rtcInit();
  rtc.start();
  touch.begin(Wire, FT3267_SLAVE_ADDRESS, TP_SDA, TP_SCL);
  touch.interruptTrigger();

  // TFT
  tft.begin();
  tft.setRotation(0);     // Adjust Rotation of your screen (0-3)
  tft.fillScreen(TFT_BLACK);
  gif.begin(BIG_ENDIAN_PIXELS);
  randomSeed(analogRead(0));

  // touch screen and rotary encoder
  pinMode(TP_IRQ, INPUT); // touch pin interrupt
  pinMode(41, INPUT); //encoder A
  pinMode(40, INPUT); //encoder B 
  attachInterrupt(41, turnedCallBack, RISING);

  // buzzer
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);

  // memory section
  settings.begin("page-set",false);
  nowPage=settings.getInt("pgcnt",0);

  // loop1 init
  xTaskCreatePinnedToCore(
                    loop0,       /* Task function. */
                    "loop0",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */

  // esp-now
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // others
}

void loop1(){
  switch(nowPage){
    case 0:{
      case1init=0;
      case2init=0;
      if(!case0init){
        waitTime=0;
        startGIFTime=0;
        gifWaitStarted=1;
        playok=1;
      }
      case0init=1;
      if(!gifWaitStarted){
        gifWaitStarted=1;
        startGIFTime=millis();
        waitTime=100*random(30,100);
      }
      if(millis()-waitTime>startGIFTime){
        playGIF(random(0,3));
        gifWaitStarted=0;
      }
      break;
    }
    case 1:{
      case0init=0;
      case2init=0;
      if(!case1init){
        guageInit();
        playok=1;
      }
      case1init=1;
      if(playok){
        drawGuage();
      }
      break;
    }
    case 2:{
      case0init=0;
      case1init=0;
      if(!case2init){
        clockInit();
        playok=1;
      }
      case2init=1;
      if(playok){
        now=rtc.now();
        drawClock();
      }
      break;
    }
    default:
      break;
  }
  
}