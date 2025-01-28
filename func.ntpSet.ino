extern DateTime now;
extern RTC_PCF8563 rtc;
extern bool timeinit;
extern int setopvalue[20];

void ntpRtcInit(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  rtc.adjust(DateTime(timeinfo.tm_year+1900,
                      timeinfo.tm_mon+1,
                      timeinfo.tm_mday,
                      timeinfo.tm_hour,
                      timeinfo.tm_min,
                      timeinfo.tm_sec
                      ));
  now=rtc.now();
  timeinit=1;
  setopvalue[3]=1;
  setOPInit();
  WiFi.disconnect();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}


void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %Y %m %d %H:%M:%S"); //%A-Sunday,%Y-2022,%m-3,%d-27,%H:%M:%S-21:10:02
}