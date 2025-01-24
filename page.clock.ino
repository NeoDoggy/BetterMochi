#include "fonts/clockfont.h"

extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern String nowtime;
extern DateTime now;

#define clock_color1 0xFFFF
#define clock_color2 0x8410
#define clock_color3 0x5ACB
#define clock_color4 0x15B3
#define clock_color5 0x0000 //00A3
#define goodRed 0xF9EB
#define goodOrange 0xFBC0

volatile int counter = 0;
float VALUE;
float lastValue = 0;

double clock_rad = 0.01745;
float clock_x[360];
float clock_y[360];

float clock_px[360];
float clock_py[360];

float clock_lx[360];
float clock_ly[360];

int clock_r = 104;
int sx = 120;
int sy = 120;

String cc[12] = { "45", "40", "35", "30", "25", "20", "15", "10", "05", "0", "55", "50" };
String days[] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
int start[12];
int startP[60];

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

int clock_angle = 0;
bool onOff = 0;
bool debounce = 0;

String h, m, s, d1, d2, m1, m2;

void clockInit() {
  tft.fillScreen(TFT_BLACK);
  sprite.setSwapBytes(true);
  sprite.createSprite(240, 240);
  sprite.setTextDatum(4);

  int b = 0;
  int b2 = 0;

  for (int i = 0; i < 360; i++) {
    clock_x[i] = (clock_r * cos(clock_rad * i)) + sx;
    clock_y[i] = (clock_r * sin(clock_rad * i)) + sy;
    clock_px[i] = ((clock_r - 16) * cos(clock_rad * i)) + sx;
    clock_py[i] = ((clock_r - 16) * sin(clock_rad * i)) + sy;

    clock_lx[i] = ((clock_r - 26) * cos(clock_rad * i)) + sx;
    clock_ly[i] = ((clock_r - 26) * sin(clock_rad * i)) + sy;

    if (i % 30 == 0) {
      start[b] = i;
      b++;
    }
    if (i % 6 == 0) {
      startP[b2] = i;
      b2++;
    }
  }
}

int lastAngle = 0;
float circle = 100;
bool dir = 0;
int rAngle = 359;

void drawClock() {  // 202501081541006 => 15
  rAngle = rAngle - 2;
  clock_angle = now.second() * 6;

  s = String(now.second());
  m = String(now.minute());
  h = String(now.hour());

  if (m.toInt() < 10)
    m = "0" + m;

  if (h.toInt() < 10)
    h = "0" + h;

  if (s.toInt() < 10)
    s = "0" + s;


  if (now.day() > 10) {
    d1 = now.day() / 10;
    d2 = now.day() % 10;
  } else {
    d1 = "0";
    d2 = String(now.day());
  }

  if (now.month() > 10) {
    m1 = now.month() / 10;
    m2 = now.month() % 10;
  } else {
    m1 = "0";
    m2 = String(now.month());
  }

  if (clock_angle >= 360)
    clock_angle = 0;

  if (rAngle <= 0)
    rAngle = 359;

  if (dir == 0)
    circle = circle + 0.5;
  else
    circle = circle - 0.5;

  if (circle > 140)
    dir = !dir;

  if (circle < 100)
    dir = !dir;

  if (clock_angle > -1) {
    lastAngle = clock_angle;

    VALUE = ((clock_angle - 270) / 3.60) * -1;
    if (VALUE < 0)
      VALUE = VALUE + 100;



    sprite.fillSprite(TFT_BLACK);
    sprite.fillCircle(sx, sy, 124, clock_color5);

    sprite.setTextColor(TFT_WHITE, clock_color5);

    sprite.drawString(days[now.dayOfTheWeek()], 120, 120, 2);


    for (int i = 0; i < 12; i++)
      if (start[i] + clock_angle < 360) {
        sprite.drawString(cc[i], clock_x[start[i] + clock_angle], clock_y[start[i] + clock_angle], 2);
        sprite.drawLine(clock_px[start[i] + clock_angle], clock_py[start[i] + clock_angle], clock_lx[start[i] + clock_angle], clock_ly[start[i] + clock_angle], clock_color1);
      } else {
        sprite.drawString(cc[i], clock_x[(start[i] + clock_angle) - 360], clock_y[(start[i] + clock_angle) - 360], 2);
        sprite.drawLine(clock_px[(start[i] + clock_angle) - 360], clock_py[(start[i] + clock_angle) - 360], clock_lx[(start[i] + clock_angle) - 360], clock_ly[(start[i] + clock_angle) - 360], clock_color1);
      }




    sprite.setFreeFont(&DSEG7_Modern_Bold_20);
    sprite.drawString(s, sx, sy - 36);
    sprite.setFreeFont(&DSEG7_Classic_Regular_28);
    sprite.drawString(h + ":" + m, sx, sy + 28);
    sprite.setTextFont(1);

    sprite.fillRect(70, 86, 12, 20, clock_color3);
    sprite.fillRect(84, 86, 12, 20, clock_color3);
    sprite.fillRect(150, 86, 12, 20, clock_color3);
    sprite.fillRect(164, 86, 12, 20, clock_color3);
    
    // sprite.setFreeFont(&Slackey_Regular_16);
    sprite.setTextColor(0x35D7, TFT_BLACK);
    // sprite.drawString("MONTH", 84, 78);
    // sprite.drawString("DAY", 162, 78);
    sprite.setTextColor(TFT_ORANGE, TFT_BLACK);
    sprite.drawString("UTC+8", 120, 174);
    // sprite.drawString("***", 120, 104);

    sprite.setTextColor(TFT_WHITE, clock_color3);
    sprite.drawString(m1, 77, 96, 2);
    sprite.drawString(m2, 91, 96, 2);

    sprite.drawString(d1, 157, 96, 2);
    sprite.drawString(d2, 171, 96, 2);

    for (int i = 0; i < 60; i++)
      if (startP[i] + clock_angle < 360)
        sprite.fillCircle(clock_px[startP[i] + clock_angle], clock_py[startP[i] + clock_angle], 1, clock_color1);
      else
        sprite.fillCircle(clock_px[(startP[i] + clock_angle) - 360], clock_py[(startP[i] + clock_angle) - 360], 1, clock_color1);

    sprite.fillTriangle(sx - 1, sy - 70, sx - 5, sy - 56, sx + 4, sy - 56, goodRed);
    // sprite.fillCircle(clock_px[rAngle], clock_py[rAngle], 6, goodRed);
    sprite.pushSprite(0, 0);
  }
}