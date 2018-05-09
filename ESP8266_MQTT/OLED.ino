#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <Time.h>
#include <TimeLib.h>
#include "SH1106.h"
#include "images.h"             // WIFI LOGO for OLED
SH1106 display(0x3c, D1, D2);
#define DEMO_DURATION 5000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;

String connectinfo1 = " ";
String connectinfo2 = " ";
//String connectinfo3 = "try MQTT"; //see global in main 

void OLED_setup() {
  connectinfo1 +="ssid: ";
  connectinfo1 += ssid;
  connectinfo2 += "IP: ";
  connectinfo2 += WiFi.localIP().toString();


// OLED init
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void draw_KLL_Demo() {
    int Rhalfwidth = 20;
    int Rhalfhight = 10;
    for (int i = 0; i < 64; i++) {
      display.drawVerticalLine(2*i, 0, i+1);
    }
    display.setColor(BLACK);
    display.fillRect(64-Rhalfwidth, 32-Rhalfhight, 2*Rhalfwidth, 2*Rhalfhight);
    display.setColor(WHITE);
    display.drawRect(64-Rhalfwidth, 32-Rhalfhight, 2*Rhalfwidth-1, 2*Rhalfhight);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64,23, "KLL");

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 52, "KLL engineering");
}

void draw_webserver_info() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "MQTT client: pcu213");  // process control unit + ...IP
    display.drawString(0, 11, connectinfo1 );
    display.drawString(0, 22, connectinfo2 );
    display.drawString(0, 33, connectinfo3 );
}

void draw_mqtt_info() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0,  0, mqttcom1);
    display.drawString(0, 10, mqttcom2);
    display.drawString(0, 20, mqttcom3);
    display.drawString(0, 30, mqttcom4);
    display.drawString(0, 40, mqttcom5);
    display.drawString(0, 50, mqttcom6);
}

void new_mqtt_info(String mqttnew) {
    mqttcom1 = mqttcom2;
    mqttcom2 = mqttcom3;
    mqttcom3 = mqttcom4;
    mqttcom4 = mqttcom5;
    mqttcom5 = mqttcom6;
    mqttcom6 = mqttnew;
}
Demo demos[] = {drawImageDemo, draw_KLL_Demo, draw_webserver_info, draw_mqtt_info};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void show_OLED() {
  display.clear();
  // draw the current demo method
  demos[demoMode]();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
  }
  counter++;
  delay(10);
}

