#include "fonts/setfont.h"

bool deb = 0;
bool btPushed=0;

unsigned short grays[101] = { 0 };

unsigned short foreground = TFT_WHITE;
unsigned short background = TFT_BLACK;
String setOpt[20] = {"Backlight","Rotation","Sound","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","Back"};
int setPageMap[20]   = {0,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};

double set_rad = 0.01745;

float set_x[360];  //outer point
float set_y[360];
float set_px[360];  //ineer point
float set_py[360];
float set_lx[360];  //long line
float set_ly[360];
float shx[360];  //short line
float shy[360];
float set_tx[360];  //text
float set_ty[360];

int set_angle = 0;
int set_value = 0;
int chosenFont;
int chosenColor;
int set_r = 140;
int set_sx = -100;
int set_sy = 268-148;
int inc = 1;
int prev = 0;

extern Preferences settings;
extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern bool playok;
extern bool inOP;
extern bool setOpInit;

int nowSetPage = 0;

void settingCallBack() {
  btPushed=1;
}

void btpushcheck(){
  if (!inSetting&&!inOP) {
    playok = 0;
    inSetting = 1;
    opInit=0;
    inOP=0;
  }
  else if(setPageMap[nowSetPage]==19&&!inOP){
    playok = 1;
    inSetting = 0;
  }
  else if(inSetting){
    if(!inOP){
      opInit=0;
      inOP=1;
    }
    else{
      inOP=0;
      opInit=0;
    }
  }
  Serial.println("nowSetPage:"+String(setPageMap[nowSetPage]));
}


void settingInit() {
  initAll(pages);
  tft.fillScreen(TFT_BLACK);
  sprite.createSprite(240, 240);

  for (int i = 0; i < 360; i++) {
    set_x[i] = (set_r * cos(set_rad * i)) + set_sx;
    set_y[i] = (set_r * sin(set_rad * i)) + set_sy;
    set_px[i] = ((set_r - 14) * cos(set_rad * i)) + set_sx;
    set_py[i] = ((set_r - 14) * sin(set_rad * i)) + set_sy;

    set_lx[i] = ((set_r - 40) * cos(set_rad * i)) + set_sx;
    set_ly[i] = ((set_r - 40) * sin(set_rad * i)) + set_sy;

    shx[i] = ((set_r - 26) * cos(set_rad * i)) + set_sx;
    shy[i] = ((set_r - 26) * sin(set_rad * i)) + set_sy;

    set_tx[i] = ((set_r + 28) * cos(set_rad * i)) + set_sx;
    set_ty[i] = ((set_r + 28) * sin(set_rad * i)) + set_sy;
  }

  for (int i = 0; i < 101; i++)
    grays[i] = tft.color565(50 + (i * 2), 50 + (i * 2), 50 + (i * 2));
  set_angle=0;
  prev=0;
  nowSetPage=0;
  drawSetting();
}

int a = 0;

void drawSetting() {
  sprite.setTextDatum(3);
  sprite.fillSprite(background);

  for (int i = 0; i < 120; i++) {
    a = set_angle + (i * 3);
    if (a > 359)
      a = (set_angle + (i * 3)) - 360;
    int pos = set_ty[a];

    if (i % 6 == 0 && set_ty[a] > 0 && set_ty[a] < 536) {

      if (pos < set_sy) {
        chosenFont = map(pos, 0, set_sy, 0, 14);
        chosenColor = map(pos, 0, set_sy, 0, 100);
      }
      if (pos >= set_sy) {
        chosenFont = map(pos, set_sy, 536, 14, 0);
        chosenColor = map(pos, set_sy, 536, 100, 0);
      }

      sprite.setFreeFont(myfonts[chosenFont]);
      sprite.drawWedgeLine(set_x[a], set_y[a], set_lx[a], set_ly[a], 3, 3, grays[chosenColor], background);

      sprite.setTextColor(grays[chosenColor], background);
      sprite.drawString(setOpt[i/6], set_tx[a], set_ty[a]); // sprite.drawString(String((i / 6) * 5), set_tx[a], set_ty[a]);

    } else if (i % 3 == 0) {
      sprite.drawWedgeLine(set_x[a], set_y[a], shx[a], shy[a], 2, 2, grays[chosenColor], background);
    } else {
      sprite.drawWedgeLine(set_x[a], set_y[a], set_px[a], set_py[a], 2, 2, grays[chosenColor], background);
    }
  }
  sprite.setTextColor(grays[30], background);
  sprite.setFreeFont(myfonts[4]);
  sprite.setTextDatum(5);
  sprite.setTextColor(grays[50], background);
  sprite.setFreeFont(&DSEG14_Classic_Regular_28);
  sprite.drawString(String(set_value / 10.00), 240, 490);

  sprite.pushSprite(0, 0);
}

