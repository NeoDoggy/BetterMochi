#include "fonts/sevenSeg.h"

#define maxRot 8
#define maxBrt 255
#define nowMaxSets 3

extern Preferences settings;
extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern bool playok;
extern int setPageMap[20];
extern int nowSetPage;
extern bool setInit;
extern bool inSetting;
extern bool mute;

int xcenter=120;
int ycenter=120;

String setopshows[20]={"Backlight","Rotation","Mute","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","Back"};
int    setopvalue[20]={255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int    maxopvalue[20]={255,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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

void setOPInit(){
  // tft.fillScreen(TFT_BLACK);
  sprite.createSprite(240, 240);
  sprite.fillScreen(TFT_BLACK);
  sprite.setTextDatum(4);
  sprite.setTextColor(TFT_WHITE);
  sprite.setFreeFont(&DSEG14_Classic_Regular_28);
  sprite.drawString(setopshows[setPageMap[nowSetPage]], 120, 120);
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

