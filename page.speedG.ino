#include "fonts/NotoSansBold15.h"
#include "fonts/NotoSansBold36.h"
#include "fonts/Final_Frontier_28.h"

extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern RTC_PCF8563 rtc;
extern int speed;
extern int tftBrightness;
extern bool hasGPS;

#define small NotoSansBold15
#define final Final_Frontier_28
#define big NotoSansBold36 

//......................................colors
#define backColor 0x0000 //0x0026
#define gaugeColor 0x0000 //055D
#define dataColor 0x0311
#define purple 0xF9EB  //EA16
#define needleColor 0xF9EB //F811

//............................dont edit this
int cx=120;
int cy=120;
int r=120;
int ir=116;
int n=0;
int angle=0;

float x[360]; //outer points of Speed gaouges
float y[360];
float px[360]; //ineer point of Speed gaouges
float py[360];
float lx[360]; //text of Speed gaouges
float ly[360];
float nx[360]; //needle low of Speed gaouges
float ny[360];

float x2[360]; //outer points of Speed gaouges
float y2[360];
float px2[360]; //ineer point of Speed gaouges
float py2[360];


double rad=0.01745;
unsigned short color1;
unsigned short color2;
float sA;
float rA;
int blinkPeriod=500;
unsigned long currentTimeL=0;
unsigned long currentTimeR=0;
int brightnesses[5]={40,80,120,150,240};
int selectedBrightness=3;
int deb1=0;
int deb2=0;
int debB=0;



//........................................................colors
unsigned short blockColor[4]={0x0312,0x0290,0x01EC,0x016A};
unsigned short dirColor[2]={0x0312,TFT_ORANGE};
unsigned short lightColor[3]={0x01EC,0x0FA8,0xB79F};

// .........important variables
bool leftPointer=0; 
bool rightPointer=0;
float speedAngle=0; //...speed variable 0-240
int segments=0; 

void guageInit(){
  tft.fillScreen(backColor);
  sprite.createSprite(240,240);
  sprite.setSwapBytes(true);
  sprite.setTextDatum(4);
  sprite.setTextColor(TFT_WHITE,backColor);
  sprite.setTextDatum(4);
  int a=120;
  for(int i=0;i<360;i++)
  {
    x[i]=((r-10)*cos(rad*a))+cx;
    y[i]=((r-10)*sin(rad*a))+cy;
    px[i]=((r-14)*cos(rad*a))+cx;
    py[i]=((r-14)*sin(rad*a))+cy;
    lx[i]=((r-24)*cos(rad*a))+cx;
    ly[i]=((r-24)*sin(rad*a))+cy;
    nx[i]=((r-36)*cos(rad*a))+cx;
    ny[i]=((r-36)*sin(rad*a))+cy;

    x2[i]=((r-50)*cos(rad*a))+cx;
    y2[i]=((r-50)*sin(rad*a))+cy;
    px2[i]=((r-62)*cos(rad*a))+cx;
    py2[i]=((r-62)*sin(rad*a))+cy;
    a++;
    if(a==360)
    a=0;
  }
}

void drawGuage()
{
  speedAngle=speed;
  sprite.fillSprite(backColor);
  sprite.fillSmoothCircle(cx, cy, r+2, backColor);
  sprite.loadFont(small);
   
 
  sprite.drawSmoothArc(cx, cy, r, ir, 30, 330, gaugeColor, backColor);
  sprite.drawSmoothArc(cx, cy, r-5, r-6, 30, 330, TFT_WHITE, backColor);
  sprite.drawSmoothArc(cx, cy, r-9, r-8, 270, 330, purple, backColor);
  sprite.drawSmoothArc(cx, cy, r-40, ir-39, 30, 330, gaugeColor, backColor);

  sprite.drawSmoothArc(cx, cy, r-50, r-49, 30, 330, 0xCE59, backColor);
  sprite.drawSmoothArc(cx, cy, r-62, r-61, 30, 330, 0xCE59, backColor);

  // battery icon
  // sprite.drawRect(110,170,20,14,TFT_RED);
  // sprite.fillRect(113,167,4,3,TFT_RED);
  // sprite.fillRect(124,167,4,3,TFT_RED);
  // sprite.drawLine(114,174,118,174,TFT_RED);
  // sprite.drawLine(122,174,126,174,TFT_RED);
  // sprite.drawLine(124,176,124,172,TFT_RED);

  // dirction pointers
  // sprite.fillTriangle(90,95,100,86,100,104,dirColor[leftPointer]);   
  // sprite.fillRect(100,90,10,10,dirColor[leftPointer]);
  // sprite.fillTriangle(140,86,140,104,150,95,dirColor[rightPointer]);
  // sprite.fillRect(130,90,10,10,dirColor[rightPointer]);
   

  
  
  //.....................................................draw GAUGES
     for(int i=0;i<26;i++){
     if(i<20) {color1=gaugeColor; color2=TFT_WHITE;} else {color1=purple; color2=purple;}

      for(int i=0;i<segments;i++)
      sprite.drawSmoothArc(cx, cy, r-58, r-54, 35+(i*25), 35+(i*25)+15, 0x4FFF, backColor);

     for(int i=0;i<13;i++)
     sprite.drawWedgeLine(x2[i*25],y2[i*25],px2[i*25],py2[i*25],1,1,0xCE59);

  if(i%2==0) {
  sprite.drawWedgeLine(x[i*12],y[i*12],px[i*12],py[i*12],3,1,color1);
  
  sprite.setTextColor(color2,backColor);
  sprite.drawString(String(i*10),lx[i*12],ly[i*12]);
  }else
  {sprite.drawWedgeLine(x[i*12],y[i*12],px[i*12],py[i*12],1,1,0xCE59);
  
  }
  }

  // ........................................needles draw
   sA=speedAngle*1.2;
  

    sprite.drawWedgeLine(px[(int)sA],py[(int)sA],nx[(int)sA],ny[(int)sA],3,3,needleColor);
  


  //.....................................drawing  TEXT
  
  sprite.setTextColor(TFT_WHITE,backColor);
  sprite.drawString("KM/H",cx,cy+34);
  sprite.unloadFont();
  // sprite.loadFont(big);
  sprite.setFreeFont(&DSEG14_Classic_Regular_28);
  sprite.drawString(String((int)speedAngle),cx,cy);  //sprite.drawString(String((int)speedAngle),cx,cy+12);
  sprite.unloadFont();
  sprite.setTextColor(TFT_RED,backColor);
  sprite.drawString("",120,194);
  if(hasGPS)
    sprite.setTextColor(TFT_WHITE,blockColor[3]);
  else
    sprite.setTextColor(TFT_WHITE,0xF9EB);
  sprite.drawString("GPS",120,212,2);
  sprite.setTextColor(TFT_ORANGE,backColor);
   sprite.drawString("",120,226);
   //..............................................push Sprite to screen  
   sprite.pushSprite(0,0);
  
}