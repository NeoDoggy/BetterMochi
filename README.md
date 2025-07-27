![Static Badge](https://img.shields.io/badge/Version-v0.3.2-blue?style=for-the-badge&logo=espressif&logoSize=auto&link=https%3A%2F%2Fgithub.com%2FNeoDoggy%2FBetterMochi%2Freleases%2Ftag%2Fv0.3.2&color=%231D9BF0)
![GitHub Repo stars](https://img.shields.io/github/stars/NeoDoggy/BetterMochi?style=for-the-badge&logo=github&logoSize=auto&label=STARS%20UWU&color=%23FE8A2C)
![Static Badge](https://img.shields.io/badge/Binaries-avaliable-green?style=for-the-badge&color=%2300ba7c)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/NeoDoggy/BetterMochi?style=for-the-badge&logo=github&logoSize=auto&color=ff4060)


# BetterMochi
###### Based on Dasai's Mochi

## Intro
Mochi is a cute little display like device that has animated faces and emotions. It's originally manufactured and designed by Dasai™.  

Though Mochi is very cute but it's very lack of any funtions. It only show animations and make noises, although in version 3, it could make movements according to your car, but to myself, it still lacks the functions that are suppose to have being a "car pet". And that's why I wanted to make some improvments to it and making it a **BETTER MOCHI**.  

<p align="center">
<img src="https://dasai.com.au/cdn/shop/files/IMG_13292_debdcf9e-f730-40b0-b9bd-089d5284aaa4.jpg?v=1703580072&width=1000">
    <b>Dasai™ Mochi</b>
</p>


## Features
Addition to the basic features mochi already has, I personally thinks that below features could add some touch to the cute little buddy.  
- speed guage
- welcoming tones
- clock

Also, the screen are both too small for the animimations and the features above. So, by changing it into a 1.28 inch round display, things starting to look interesting. First, it finally looks like a guage, or a clock.  

## Implement

To implement the above mentioned features, you could just use a vanilla ESP32/ESP32-S3 and a round tft display powered by GC9A01 with an GPS module. Or, you could use a M5Dial for more features like a rotary encoder for changing modes, and a built in buzzer for melodies.  

As for myself, all the software is tested and implement using the first way on a bread board and later been implemented onto the M5Dial and tested. The below installation steps are specified for implementation on the M5Dial or similar devices with same GPIO outpins.  
<p align="center">
<img src="https://i.imgur.com/32S1pZj.jpeg">
<b>Mochi on M5Dial</b>
</p>

## Installation

### Parts
To make your own Mochi on a M5Dial, you first need the following parts.  
1. A M5Dial
2. Neo6M GPS reciever
3. Any ESP32 (This includes S2, S3, C3, etc.)
4. CH340 UART to USB connector

### Software
For an easy burn in for the software, we'll use arduino as the platform.  

#### STEP 0
First, for the preparation for the tft_eSPI library. After installing the library, navigate to the location of the library in arduino, for example `C:\Users\yourname\Documents\Arduino\libraries\TFT_eSPI`.  
After, locate the `Setup46_GC9A01_ESP32.h` in the `User_Setups` and paste the following codes into it. Also remember to backup the original one.
```cpp
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 46

#define GC9A01_DRIVER

#define TFT_MOSI 5
#define TFT_SCLK 6
#define TFT_CS    7  // Chip select control pin
#define TFT_DC    4  // Data Command control pin
#define TFT_RST  8  // Reset pin (could connect to RST pin)
#define TFT_BL   9
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY  80000000 //40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS
```
Then, uncomment the `#include <User_Setups/Setup46_GC9A01_ESP32.h>` in the `User_Setup_Select.h` located in the root folder of tft_eSPI library.

#### STEP 1
In order to make the esp-now function correctly, please check the mac address of your m5dial and place here in the 8th line in datasender.ino:
```cpp
uint8_t broadcastAddress[] = {0xC0, 0x4E, 0x30, 0x12, 0xC8, 0xA4};//c0:4e:30:12:c8:a4
```
About how to get your esp32's mac, please navigate to [here](https://randomnerdtutorials.com/get-change-esp32-esp8266-mac-address-arduino/) for more informations.  
Also, if you want to use the ntp function, please remember to set up your AP's ssid and password located in `conf.secret.h`.

#### STEP 2
Place the files excluding the datasender folder into a sketch folder and choose M5Dial as your board, then hit compile&upload.  

#### STEP 3
Connect your Neo6M to your computer using the CH340 dongle and change the baude rate to `115200` and rate to `5Hz` using U-center.  
More infos of changing the settings could be found [Here](https://www.waveshare.com/wiki/UART_GPS_NEO-6M).  

#### STEP 4
Connect the GPS to the secondary ESP32 using UART. Here we use the second UART port for communication, but since specs may be different, feel free to modify the code to meet your own needs.  

#### STEP 5
Place the files in the datasender folder into another sketch folder and select the corresponding board then hit upload.  

#### STEP 6
At defualt, after installation, the dial should got animations running on the screen. You could use the rotary encoder to change to the guage page by spinnig the outer rim.  
Below is a picture showcasing the guage page.  
Also, the touch screen is for adjusting the brightness of the screen, swipe left to dim it and the opposite to lighten it!  

<p align="center">
<img src="https://i.imgur.com/Q3KdznQ.jpeg">
<b>Speed guage page</b>
</p>

---

## UPDATE v0.3.3
###### 25 JUN 27
Fixed gif playing issues when millsec() is below waitTime.  
The original boolean is  
```cpp
if(millis()-waitTime>startGIFTime)
```
This would result in a overflow if millsec is below waitTime. Fixed by changing the left and right value.  
```cpp
if(millis()>startGIFTime+waitTime)
```

## UPDATE v0.3.2
###### 25 JAN 28
Smol tweak for the speed guage, the guage now would set its speed to 0 when NAK from the GPS with a timeout of `0` seconds. Also, the `GPS` located at the bottom of the guage would turn into red when NAK and navy when ACKed.  

## UPDATE v0.3.1
###### 25 JAN 26
Now added ntp support for setting the time s.t. when there are no GPS signals, you could still set the internal RTC time.  

To set the time using NTP, simple navigate to the settings->Set Time.  

To make sure you could connect to the ntp server, you need to set your AP's SSID and PSW using the provided file `conf.secret.h`.  

In the file, simply place your AP's SSID and PSW into the string, for example:
```cpp=1
char ssid[] = "YOUR_SSID";
char pass[] = "YOUR_PW";
```
I would recommend you to set it to your mobile phone's hotspot etc s.t. you could set the time wherever you go.  

## UPDATE v0.3.0
###### 25 JAN 24
Now the mochi got settings, horay! To get to the set page, simply press the button located at the bottom of the round, or in other words, press the "M5" on the orange ring.  

In the set page, you could turn the rotary encoder to navigate to the settings you want to change, when navigated to the one, simply press the button again will move you into the setting itself and one more press will bring you out.  

To navigate back to the home page, turn to the part that is labeled `Back` and press the button.  

<p align="center">
<img src="https://raw.githubusercontent.com/NeoDoggy/BetterMochi/refs/heads/main/readmeImages/Roty.gif" alt="Rotaries!!!">
<br>
<b>Rotaries!!!</b>
</p>

BTW, the settings could also be adjust with the touch screen, left for adding `1` and right for subtracting `1`. The rotary's delta depends, for example, when in the backlight setting, turning left adds `5` to the value.  

#### SETTINGS
1. Backlight -> Backlight value (0-255)
2. Rotation -> Rotation of the screen (0-7)
3. Sound -> Whether the beep is muted (True/False)

---

## UPDATE v0.2.0
###### 25 JAN 20
Congrats!!! Now the mochi guage has 3 pages, the mochi page; the speed guage page; and the clock page!! No more distractions, mochi could help for telling the time now!!

### Clock Page
<p align="center">
<img src="https://i.imgur.com/UqplSaZ.jpeg">
<b>Speed guage page</b>
</p>

The time will be automatically set when connected to a resonable GPS signal.  