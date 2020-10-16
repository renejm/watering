
/*
A small project on ESP8266 + Capacity soil moisture sensor for an automated plant watering system.
Copyright © 2020 - Rene J. Martins
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "MQTTConnector.h"
#include "Credentials.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 10000

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
const int AIR_VALUE = 520;
const int WATER_VALUE = 280;
const int MOISTURE_SENSOR_PIN = A0;
const int RELAY_PIN = D5;
//===========================================================================
const int MINIMUM_MOISTURE = 40; // % Moisture treshold for watering
const int WATERING_TIME_IN_SECS = 30;
const int MINIMUM_TIME_BETWEEN_WATERINGS_IN_SECS = 3600; // 1-hour interval
const int PUBLISHING_RATE_IN_SECS = 300; // 5 minutes between publishings
//===========================================================================
int soilMoistureValue = 0;
unsigned long startTime = 0;
unsigned long pubTime = 0;
unsigned long endTime = 0;
unsigned long initialTime = 0;
unsigned long elapsedSecs = 0;

boolean pingMQTT = true;

 
void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  WiFiBegin(STA_SSID, STA_PASS);
  MQTTBegin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();
  pinMode(RELAY_PIN, OUTPUT);
  initialTime = getInitialTime();
  endTime = initialTime - MINIMUM_TIME_BETWEEN_WATERINGS_IN_SECS + 360;
}


void WiFiBegin(const char* ssid, const char* pass)
{
  WiFi.begin(ssid, pass);
  Serial.println("Aguardando conexão...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_TIMEOUT);
    Serial.printf(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.printf("\nConectado ao roteador. IP: %d.%d.%d.%d\n", ip[0],ip[1],ip[2],ip[3]);
}


String twoDigs(int nbr) {
  if (nbr < 10) {
    return "0" + String(nbr);
  }
  else {
    return String(nbr);
  }
}


unsigned long getInitialTime() {
  int sttime = PUBLISHING_RATE_IN_SECS;
  if (WATERING_TIME_IN_SECS > sttime) {
    sttime = WATERING_TIME_IN_SECS;
  }
  if (MINIMUM_TIME_BETWEEN_WATERINGS_IN_SECS > sttime) {
    sttime = MINIMUM_TIME_BETWEEN_WATERINGS_IN_SECS;
  }
  return sttime;
}


unsigned long elapsedTime() {
  return ((millis() / 1000) + initialTime);
}


void publishMoisture(int moistPerc) {
  if ( (elapsedTime() - pubTime) > PUBLISHING_RATE_IN_SECS ) {
    String msg = String(moistPerc);
    int msglen = msg.length() + 1;
    char charmsg[msglen];
    msg.toCharArray(charmsg, msglen);
    if (MQTTPublish(TOPIC1, charmsg)) {
      Serial.println("Umidade publicada!");
      pubTime = elapsedTime();
    }    
  }
}


void startWatering(int moistPerc) {
  if ( (elapsedTime() - endTime) >= MINIMUM_TIME_BETWEEN_WATERINGS_IN_SECS ) {
    if (moistPerc < MINIMUM_MOISTURE) {
      if (!digitalRead(RELAY_PIN)) {
        if (MQTTPublish(TOPIC2, "Regando as plantas")) {
          Serial.println("Status publicado: Regando as plantas");
        }
        delay(1000);
        startTime = elapsedTime();
        digitalWrite(RELAY_PIN, HIGH);
      }
    }
  }
}


void stopWatering(int moistPerc) {
  if (digitalRead(RELAY_PIN)) {
    Serial.println(elapsedTime() - startTime);
    if ( (elapsedTime() - startTime) >= WATERING_TIME_IN_SECS ) {
      digitalWrite(RELAY_PIN, LOW);
      endTime = elapsedTime();
      if (MQTTPublish(TOPIC2, "Bomba desligada")) {
        Serial.println("Status publicado: Bomba desligada");
      }
      delay(1000);
    }
  }
}


void displayText(String text, int row, int col, int textSize) {
  if (text.length() > 0) {
    display.setCursor(col, row);
    display.setTextSize(textSize);
    display.setTextColor(WHITE);
    display.println(text);
  }
}


void displayStatus() {  
  if (digitalRead(RELAY_PIN)) {
    displayText("Bomba", 1, 90, 1);
    displayText("ligada", 11, 90, 1);
    displayText(String(elapsedTime() - startTime) + "s", 20, 90, 1);
  }
  else {
    unsigned long secs = elapsedTime() - endTime;
    unsigned long mins = 0;
    unsigned long hrs = 0;
    displayText("Espera", 1, 90, 1);
    while (secs > 59) {
      mins++;
      secs -= 60;
    }
    while (mins > 59) {
      hrs++;
      mins -= 60;
    }
    if (hrs > 0) {
      displayText(String(hrs) + ":" + twoDigs(mins), 11, 90, 1);
      displayText("horas", 20, 90, 1);
    }
    else if (mins > 0) {
      displayText(String(mins) + ":" + twoDigs(secs), 11, 90, 1);
      displayText("mins.", 20, 90, 1);
    }
    else {
      displayText(String(secs) + "s", 11, 90, 1);
    }        
  }
}


void displayDiag(String l1, String l2, String l3) {
  displayText(l1, 35, 75, 1);
  displayText(l2, 45, 75, 1);
  displayText(l3, 55, 75, 1);
}


int getMoisturePercent(int sensorval) {
  //Serial.println(sensorval);
  int numberlength = 4;
  int soilmoisturepercent = 0;
  
  delay(250);
  display.clearDisplay();
  
  soilmoisturepercent = map(sensorval, AIR_VALUE, WATER_VALUE, 0, 100);
  if (soilmoisturepercent > 100) {
    soilmoisturepercent = 100;
  }
  else if (soilmoisturepercent < 0) {
    soilmoisturepercent = 0;
  }

  if (soilmoisturepercent < 10) {
    numberlength = 2;
  }
  else if (soilmoisturepercent < 100) {
    numberlength = 3;
  }

  displayText("Umidade", 0, 0, 2);
  displayText("do solo", 15, 0, 2);
  displayText(String(soilmoisturepercent) + "%", 40, 0, 3);
  //displayText(String(sensorval), 40, 0, 3);

  if (soilmoisturepercent < 30) {
    displayDiag("ATENCAO:", "Solo", "seco!");
  }
  else if (soilmoisturepercent < 60) {
    displayDiag("", "Solo", "umido");
  }
  else if (soilmoisturepercent < 80) {
    displayDiag("", "Solo", "molhado!");
  }
  else {
    displayDiag("ATENCAO:", "Solo", "alagado!");
  }
  
  displayStatus();
  
  display.display();

  return soilmoisturepercent;
}

 
void loop() {
  if (pingMQTT && elapsedTime() % 10 == 0) {
    Serial.println("Chamando MQTTLoop()");
    MQTTLoop();
    pingMQTT = false;
  }
  else if (!pingMQTT && elapsedTime() % 10 > 0) {
    pingMQTT = true;
  }
  int moistPerc = 0;
  soilMoistureValue = analogRead(MOISTURE_SENSOR_PIN);
  moistPerc = getMoisturePercent(soilMoistureValue);
  startWatering(moistPerc);
  stopWatering(moistPerc);
  publishMoisture(moistPerc);
}
