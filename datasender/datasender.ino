#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "TinyGPS++.h"

#define UTC 8

uint8_t broadcastAddress[] = {0xC0, 0x4E, 0x30, 0x12, 0xC8, 0xA4};//c0:4e:30:12:c8:a4

typedef struct struct_message {
  int speed;
  char nowtime[15];                       // 202501081541006 => 15
} struct_message;

struct_message enData;
int speed;

esp_now_peer_info_t peerInfo;
TinyGPSPlus gps;

String nowtime="202501010000003"; // 202501081541006 => 15

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  while(Serial2.available()){
    gps.encode(Serial2.read());
  }
  if(gps.speed.isUpdated()){
    speed=gps.speed.kmph();
    enData.speed=speed;
    Serial.print("\nSpeed: ");Serial.println(enData.speed);

    nowtime="";
    nowtime+=String(gps.date.year());
    if(gps.date.month()<10)
      nowtime+="0";
    nowtime+=String(gps.date.month());
    if(gps.date.day()<10)
      nowtime+="0";
    nowtime+=String(gps.date.day());
    if((gps.time.hour()+UTC)%24<10)
      nowtime+="0";
    nowtime+=String((gps.time.hour()+UTC)%24);
    if(gps.time.minute()<10)
      nowtime+="0";
    nowtime+=String(gps.time.minute());
    if(gps.time.second()<10)
      nowtime+="0";
    nowtime+=String(gps.time.second());
    nowtime+=String(0);
    nowtime.toCharArray(enData.nowtime, nowtime.length()+1);
    Serial.println("nowtime: "+nowtime);

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &enData, sizeof(enData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
  if(gps.time.isUpdated()) {
    Serial.println("Time has been updated");
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.print("Time now [GMT]:  ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
    Serial.print("timeAGE:");
    Serial.print(gps.time.age());
    Serial.print(" /speedAGE:");
    Serial.print(gps.speed.age());
    Serial.print(" /dateAGE:");
    Serial.print(gps.date.age());
    Serial.print(" /locatAGE:");
    Serial.println(gps.location.age());
  }
}
