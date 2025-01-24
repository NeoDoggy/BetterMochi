#include <AnimatedGIF.h>

extern AnimatedGIF gif;
extern TFT_eSPI tft;
extern bool playok;

void playGIF(int nowgif){
  if (nowgif==0&&gif.open((uint8_t *)(face1_lossy200), sizeof(face1_lossy200), GIFDraw)){
    tft.startWrite();
    while (playok&&gif.playFrame(true, NULL)){
      yield();
    }
    playok=1;
    gif.close();
    tft.endWrite(); 
  }
  if (nowgif==1&&gif.open((uint8_t *)(face2_lossy200), sizeof(face2_lossy200), GIFDraw)){
    // Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite();
    while (playok&&gif.playFrame(true, NULL)){
      yield();
    }
    playok=1;
    gif.close();
    tft.endWrite(); 
  }
  if (nowgif==2&&gif.open((uint8_t *)(face3_lossy200), sizeof(face3_lossy200), GIFDraw)){
    // Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite();
    while (playok&&gif.playFrame(true, NULL)){
      yield();
    }
    playok=1;
    gif.close();
    tft.endWrite(); 
  }
}