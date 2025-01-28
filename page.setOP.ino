#include "fonts/sevenSeg.h"

#define maxRot 8
#define maxBrt 255
#define nowMaxSets 4

extern Preferences settings;
extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern bool playok;
extern int setPageMap[20];
extern int nowSetPage;
extern bool setInit;
extern bool inSetting;
extern bool mute;
extern bool ntpInit;

int xcenter=120;
int ycenter=120;

String setopshows[20]={"Backlight","Rotation","Mute","Set Time","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","Back"};
int    setopvalue[20]={255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int    maxopvalue[20]={255,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int    dltopvalue[20]={5,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void setOPadd(){
  if(setopvalue[setPageMap[nowSetPage]]<maxopvalue[setPageMap[nowSetPage]]&&setPageMap[nowSetPage]<nowMaxSets)setopvalue[setPageMap[nowSetPage]]++;
  Serial.println(">120");
  setOPInit();
}

void setOPsub(){
  if(setopvalue[setPageMap[nowSetPage]]>0&&setPageMap[nowSetPage]<nowMaxSets)setopvalue[setPageMap[nowSetPage]]--;
  Serial.println("<=120");
  setOPInit();
}

void setOPCW(){
  if(setopvalue[setPageMap[nowSetPage]]+dltopvalue[setPageMap[nowSetPage]]<=maxopvalue[setPageMap[nowSetPage]]&&setPageMap[nowSetPage]<nowMaxSets)setopvalue[setPageMap[nowSetPage]]+=dltopvalue[setPageMap[nowSetPage]];
  setOPInit();
}

void setOPCCW(){
  if(setopvalue[setPageMap[nowSetPage]]-dltopvalue[setPageMap[nowSetPage]]>=0&&setPageMap[nowSetPage]<nowMaxSets)setopvalue[setPageMap[nowSetPage]]-=dltopvalue[setPageMap[nowSetPage]];
  setOPInit();
}

void setOPInit(){
  // tft.fillScreen(TFT_BLACK);
  sprite.createSprite(240, 240);
  sprite.fillScreen(TFT_BLACK);
  sprite.setTextDatum(4);
  sprite.setTextColor(TFT_WHITE);
  sprite.setFreeFont(&DSEG14_Classic_Regular_28);
  sprite.drawString(setopshows[setPageMap[nowSetPage]], 120, 120);
  if(setPageMap[nowSetPage]==3&&!setopvalue[3]){
    WiFi.begin(ssid, pass);
  } // start wifi to connect to ntp server // ssid and pass are configurable in "conf.secret.h" <= don't push this onto public domains
  
  // if mute page
  if(setPageMap[nowSetPage]==2){
    if(setopvalue[setPageMap[nowSetPage]]){
      sprite.setTextColor(0xF9EB);
      sprite.drawString("muted", 120, 160);
    }
    else{
      sprite.setTextColor(0x05AF);
      sprite.drawString("unmuted", 120, 160);
    }
    sprite.setTextColor(TFT_WHITE);
  }

  // if ntp page
  else if(setPageMap[nowSetPage]==3){
    setopvalue[setPageMap[nowSetPage]]=max(timeinit+0,setopvalue[setPageMap[nowSetPage]]);
    if(setopvalue[setPageMap[nowSetPage]]){
      sprite.setTextColor(0x05AF);
      sprite.drawString("setted", 120, 160);
    }
    else{
      sprite.setTextColor(0xF9EB);
      sprite.drawString("unsetted", 120, 160);
    }
    sprite.setTextColor(TFT_WHITE);
  }

  // hokkano
  else
    sprite.drawString(String(setopvalue[setPageMap[nowSetPage]]), 120, 160);

  
  runOP();
  sprite.pushSprite(0, 0);
}

void setVaInit(){
  setopvalue[0]=settings.getInt("brtns",255);
  setopvalue[1]=settings.getInt("tftrot",0);
  setopvalue[2]=settings.getBool("muted",0);
}

void runOP(){
  int tmp=setopvalue[setPageMap[nowSetPage]];

  if(setPageMap[nowSetPage]==0){
    settings.putInt("brtns", tmp);
    analogWrite(9, tmp);
  }
  else if(setPageMap[nowSetPage]==1){
    settings.putInt("tftrot", tmp);
    tft.setRotation(tmp);
  }
  else if(setPageMap[nowSetPage]==2){
    settings.putBool("muted", tmp);
    mute=tmp;
  }
}

