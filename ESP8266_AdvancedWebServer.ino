/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

#define useFIXIP      // or disable with //
#if defined useFIXIP
IPAddress STAip(192,168,1,213);
IPAddress STAgateway(192, 168, 1, 1);
IPAddress STAsubnet(255, 255, 255, 0);
#endif
const int STAserverport = 4321;
String connectinfo1 = " ";
String connectinfo2 = " ";
String connectinfo3 = " ";
//KLL try fix IP and port
ESP8266WebServer server (STAserverport);

const int led = 13;

void handleRoot() {
	digitalWrite ( led, 1 );
	char temp[600];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 600,

"<html><head>\
    <meta http-equiv='refresh' content='15'/>\
    <title>ESP8266 Webserver Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head><body><br>\
    <h1>i am ESP8266 + OLED</h1>\
    <p>more info at <a href='http://kll.engineering-news.org/kllfusion01/articles.php?article_id=148' >my BLOG</a></p>\
    <p>from my <a href='http://dong-sen.com/d-duino/' >creator</a></p>\
    <img src='http://dong-sen.com/wp-content/uploads/2016/02/D-duino.png' width=600 alt='ESP8266OLED' >\
    <p>Uptime: %02d:%02d:%02d</p>\
  <br></body></html>",

		hr, min % 60, sec % 60
	);
	server.send ( 200, "text/html", temp );
	digitalWrite ( led, 0 );
}

void handleNotFound() {
	digitalWrite ( led, 1 );
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
	digitalWrite ( led, 0 );
}

void setup ( void ) {
	pinMode ( led, OUTPUT );
	digitalWrite ( led, 0 );
	Serial.begin ( 115200 );
	WiFi.mode ( WIFI_STA );
	WiFi.begin ( ssid, password );
#if defined useFIXIP
  WiFi.config(STAip,STAgateway,STAsubnet); 
#endif
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}

	Serial.println ( "" );
  connectinfo1 +="ssid: ";
  connectinfo1 += ssid;
	Serial.println ( connectinfo1 );

  connectinfo2 += "IP: ";
  connectinfo2 += WiFi.localIP().toString();

  if ( STAserverport != 80 ) {
      connectinfo2 += ":";
      connectinfo2 += STAserverport;
  }
  Serial.println ( connectinfo2 );
  
	if ( MDNS.begin ( "esp8266" ) ) {
		Serial.println ( "MDNS responder started" );

	}

	server.on ( "/", handleRoot );
//	server.on ( "/test.svg", drawGraph );
	server.on ( "/help", []() {
		server.send ( 200, "text/plain", "future help text" );
	} );
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );

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
    display.drawString(0, 0, "webserver:");
    display.drawString(0, 11, connectinfo1 );
    display.drawString(0, 22, connectinfo2 );
}

Demo demos[] = {drawImageDemo, draw_KLL_Demo, draw_webserver_info};
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

void loop ( void ) {
	server.handleClient();
  show_OLED();
}

/*
void drawGraph() {
	String out = "";
	char temp[100];
	out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
 	out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
 	out += "<g stroke=\"black\">\n";
 	int y = rand() % 130;
 	for (int x = 10; x < 390; x+= 10) {
 		int y2 = rand() % 130;
 		sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
 		out += temp;
 		y = y2;
 	}
	out += "</g>\n</svg>\n";

	server.send ( 200, "image/svg+xml", out);
}

HTML:
    <img src=\"/test.svg\" />\

*/

