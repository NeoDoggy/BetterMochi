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
#define pages 2
#define loop1 loop

AnimatedGIF gif;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
Preferences settings;
TouchDrvFT6X36 touch;
RTC_PCF8563 rtc;
TaskHandle_t Task2;
uint8_t snderADDR[] = {0xFC, 0xE8, 0xC0, 0xA5, 0xDD, 0x10};//fc:e8:c0:a5:dd:10

typedef struct struct_message {
  int speed;
} struct_message;

struct_message myData;

extern int segments;

int tftBrightness=255;
int speed=0;
int nowGif=0;
int nowPage=0;
bool playok=1;
bool playtone=0;
bool isTouched=0;
const int TONE_OUTPUT_PIN = 3;
const int TONE_PWM_CHANNEL = 0;

unsigned long waitTime;
unsigned long startGIFTime;
bool gifWaitStarted=0;

bool case0init=0;
bool case1init=0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  speed=myData.speed;
  Serial.print("now speed:");
  Serial.println(myData.speed);
}

void loop0(void * pvParameters ){
  while(1){
    if(digitalRead(TP_IRQ)==LOW){
      int16_t x[2], y[2];
      uint8_t touched = touch.getPoint(x, y, 2);
      for (int i = 0; i < touched; ++i) {
        Serial.print("X[");
        Serial.print(i);
        Serial.print("]:");
        Serial.print(x[i]);
        Serial.print(" ");
        Serial.print(" Y[");
        Serial.print(i);
        Serial.print("]:");
        Serial.print(y[i]);
        Serial.print(" ");
        tftBrightness=x[i];
        analogWrite(9, x[i]);
      }
      Serial.println();
    }
    if(playtone){
      settings.putInt("pgcnt", nowPage);
      if(digitalRead(41)==digitalRead(40)){ // clockwize
        Serial.println("CW");
        for (int i=1; i<20; i++) {
          ledcWriteTone(TONE_PWM_CHANNEL, i * 100);
          delay(10);    
        }
      }
      else{
        Serial.println("CCW");
        for (int i=19; i>0; i--) {
          ledcWriteTone(TONE_PWM_CHANNEL, i * 100);
          delay(10);    
        }
      }
      ledcWrite(TONE_PWM_CHANNEL, 0); // Stop tone 
      playtone=0;
    }
    delay(1);
  }
}


void turnedCallBack(){
  if(digitalRead(41)==digitalRead(40)){ // clockwize
    nowPage=(nowPage+1)%pages;
  }
  else{
    if(nowPage-1<0)nowPage=pages;
    nowPage=(nowPage-1)%pages;
  }
  playtone=1;
  playok=0;
  Serial.println(nowPage);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(11, 12); 
  rtc.begin();
  rtc.start();
  tft.begin();
  tft.setRotation(0);     // Adjust Rotation of your screen (0-3)
  tft.fillScreen(TFT_BLACK);
  gif.begin(BIG_ENDIAN_PIXELS);
  randomSeed(analogRead(0));
  pinMode(TP_IRQ, INPUT); // touch pin interrupt
  pinMode(41, INPUT); //encoder A
  pinMode(40, INPUT); //encoder B 
  attachInterrupt(41, turnedCallBack, RISING);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
  settings.begin("page-set",false);
  nowPage=settings.getInt("pgcnt",0);
  touch.begin(Wire, FT3267_SLAVE_ADDRESS, TP_SDA, TP_SCL);
  touch.interruptTrigger();
  xTaskCreatePinnedToCore(
                    loop0,    /* Task function. */
                    "loop0",  /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop1(){
  switch(nowPage){
    case 0:{
      case1init=0;
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
    default:
      break;
  }
  
}