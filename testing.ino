#include "conf.settings.h"

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
#include "conf.pitches.h"
#include <esp_wifi.h>

#include "images/face1_lossy200.h"
#include "images/face2_lossy200.h"
#include "images/face3_lossy200.h"
#include <WiFi.h>
#include "conf.secret.h"

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
#define settingPages 20
#define loop1 loop

extern int nowSetPage;
extern int segments;
extern int set_value;
extern int set_angle;
extern int prev;
extern bool btPushed;
extern int setopvalue[20];

AnimatedGIF gif;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
Preferences settings;
TouchDrvFT6X36 touch;
RTC_PCF8563 rtc;
DateTime now;
TaskHandle_t Task2;
uint8_t snderADDR[] = {0xFC, 0xE8, 0xC0, 0xA5, 0xDD, 0x10};//fc:e8:c0:a5:dd:10
int setPageRad[20] = {0,18,36,54,72,90,108,126,144,162,180,198,216,234,252,270,288,306,324,342};

typedef struct struct_message {
  int speed;
  char nowtime[15];                       // 202501081541006 => 15
} struct_message;

struct_message enData;


// settings 
int tftBrightness=255;
int tftRotation=0;
bool tftSound=1;

int speed=0;
int nowGif=0;
int nowPage=0;

bool playok=1;
bool playtone=0;
bool isTouched=0;
bool gifWaitStarted=0;
bool caseNinit[pages]; //0:gif,1:speed,2:clock
bool setInit=0;
bool opInit=0;
bool ntpInit=0;
bool timeinit=0;
bool inSetting=0;
bool inOP=0;
bool hasGPS=0;
bool mute=0;
bool CW=0;

const int TONE_OUTPUT_PIN = 3;
const int TONE_PWM_CHANNEL = 0;
const char* ntpServer = "time.google.com";
const long  gmtOffset_sec = 28800; // UTC+8
const int   daylightOffset_sec = 0;

unsigned long waitTime;
unsigned long startGIFTime;

// nowtime
String nowtime="202501010000009"; // 202501081541000 => 15  the last digit is for checking status, 0=>OK, 9=>lost GPS

void rtcInit(){
  rtc.adjust(DateTime(nowtime.substring(0,4).toInt(),
                      nowtime.substring(4,6).toInt(),
                      nowtime.substring(6,8).toInt(),
                      nowtime.substring(8,10).toInt(),
                      nowtime.substring(10,12).toInt(),
                      nowtime.substring(12,14).toInt()
                      ));
  now=rtc.now();
  if(nowtime!="202501010000009")
    timeinit=1;
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&enData, incomingData, sizeof(enData));
  speed=enData.speed;
  nowtime=String(enData.nowtime);

  if(nowtime[14]=='0')
    hasGPS=1;
  else if(nowtime[14]=='9')
    hasGPS=0;

  // init RTC
  if(!timeinit)rtcInit();

  Serial.print("now speed:");
  Serial.println(enData.speed);
  Serial.print("now time:");
  Serial.println(nowtime);
}

// rotary encoder callback
void turnedCallBack(){
  playtone=1;
  playok=0;
}

void setup() {  // Initialize all components
  // memory section
  settings.begin("page-set",false);
  nowPage=settings.getInt("pgcnt",0);
  mute=settings.getBool("muted",0);
  tftBrightness=settings.getInt("brtns",255);
  setVaInit();

  // Serial
  Serial.begin(115200);

  // BLE
  

  // I2C
  Wire.begin(11, 12); 
  rtc.begin();
  rtcInit();
  rtc.start();
  touch.begin(Wire, FT3267_SLAVE_ADDRESS, TP_SDA, TP_SCL);
  touch.interruptTrigger();

  // TFT
  tft.begin();
  tft.setRotation(settings.getInt("tftrot",0));     // Adjust Rotation of your screen (0-3) // get rotsetting from EEPROM
  tft.fillScreen(TFT_BLACK);
  gif.begin(BIG_ENDIAN_PIXELS);
  randomSeed(analogRead(0));
  analogWrite(9, tftBrightness);

  // touch screen and rotary encoder
  pinMode(TP_IRQ, INPUT); // touch pin interrupt
  pinMode(41, INPUT); //encoder A
  pinMode(40, INPUT); //encoder B 
  attachInterrupt(41, turnedCallBack, RISING);
  attachInterrupt(42, settingCallBack, FALLING);

  // buzzer
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);


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
  WiFi.useStaticBuffers(true);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  // others
}

// 1KHZ loop: CPU1
void loop0(void * pvParameters ){
  while(1){
    if(btPushed){
      btPushed=0;
      btpushcheck();
      if(!mute){
        if(inSetting){
          int tmptone[]={NOTE_C6,NOTE_E6,NOTE_G6,NOTE_C7};
          playSong(tmptone,20);
        }
        else{
          int tmptone[]={NOTE_C7,NOTE_G6,NOTE_E6,NOTE_C6};
          playSong(tmptone,20);
        }
      }
    }
    if(digitalRead(TP_IRQ)==LOW&&inOP){
      int16_t x[2], y[2];
      uint8_t touched = touch.getPoint(x, y, 2);
      // for (int i = 0; i < touched; ++i) {
      //   tftBrightness=x[i];
      //   analogWrite(9, x[i]);
      // }
      if(x[0]>120)setOPadd();
      else setOPsub();
    }
    if(playtone){
      if(digitalRead(41)==digitalRead(40)){ // clockwize
        Serial.println("CW");
        CW=1;
        if(!inSetting)
          ++nowPage%=pages;
        else if(inSetting&&!inOP)
          ++nowSetPage%=settingPages;
        if(!mute){
          ledcWriteTone(TONE_PWM_CHANNEL,4000);
          delay(20);
          ledcWrite(TONE_PWM_CHANNEL, 0); // Stop tone 
        }
      }
      else{
        Serial.println("CCW");
        CW=0;
        if(!inSetting){
          if(nowPage-1<0)nowPage=pages-1;
          else nowPage--;
        }
        else if(inSetting&&!inOP){
          if(nowSetPage-1<0)nowSetPage=settingPages-1;
          else nowSetPage--;
        }
        if(!mute){
          ledcWriteTone(TONE_PWM_CHANNEL,4000);
          delay(20);
          ledcWrite(TONE_PWM_CHANNEL, 0); // Stop tone 
        }
      }
      playtone=0;
      settings.putInt("pgcnt", nowPage);
    }
    if (WiFi.status() == WL_CONNECTED){ // if wifi connected
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if(!timeinit){
        printLocalTime();
        ntpRtcInit();
      }
    }
    delay(1);
  }
}

// main loop: CPU0
void loop1(){
  if(!inSetting){
    switch(nowPage){
      case 0:{
        initWithout(nowPage,pages);
        if(!caseNinit[0]){
          waitTime=0;
          startGIFTime=0;
          gifWaitStarted=1;
          playok=1;
        }
        caseNinit[0]=1;
        if(!gifWaitStarted){
          gifWaitStarted=1;
          startGIFTime=millis();
          waitTime=100*random(100,3000);
        }
        if(millis()-waitTime>startGIFTime){
          playGIF(random(0,3));
          gifWaitStarted=0;
        }
        break;
      }
      case 1:{
        initWithout(nowPage,pages);
        if(!caseNinit[1]){
          guageInit();
          playok=1;
        }
        caseNinit[1]=1;
        if(playok){
          drawGuage();
        }
        break;
      }
      case 2:{
        initWithout(nowPage,pages);
        if(!caseNinit[2]){
          clockInit();
          playok=1;
        }
        caseNinit[2]=1;
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
  else if(!inOP&&inSetting){
    // Serial.println("nowINSetting");
    if(!setInit){
      setInit=1;
      settingInit();
    }
    set_value = map(set_angle, 0, 359, 1000, 0);
    if(prev!=nowSetPage){
      set_angle=setPageRad[prev];
      drawSetting();
      if(CW){
        while(set_angle!=setPageRad[nowSetPage]){
          ++set_angle%=360;
          drawSetting();
          // Serial.println("nowAngle=>"+String(set_angle));
          // delay(1);
        }
        prev=nowSetPage;
      }
      else{
        while(set_angle!=setPageRad[nowSetPage]){
          if(set_angle-1<0)set_angle=359;
          else set_angle--;
          drawSetting();
          // delay(1);
        }
        prev=nowSetPage;
      }
    }
    
  }
  else if(inOP){
    if(!opInit){
      setOPInit();
      opInit=1;
      setInit=0;        
    }
  }
}