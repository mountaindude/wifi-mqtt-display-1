
#include <ESP8266WiFi.h>
#include "CMMC_OTA.h"

#include <TM1638.h>



#ifndef WIFI_SSID
  #define WIFI_SSID       ""
  #define WIFI_PASSPHRASE ""
#endif

CMMC_OTA ota;


// define a module on data pin D3, clock pin D2 and strobe pin D4
TM1638 module(D3, D2, D4);



void init_wifi_hardware()
{
  WiFi.disconnect(true);
  Serial.begin(115200);
  Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("Starting wifi...");
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  while (WiFi.status() != WL_CONNECTED) {
   Serial.printf("connecting %s:%s \r\n", WIFI_SSID, WIFI_PASSPHRASE);
   delay(1000);
 }

  Serial.print("READY!! IP address: ");

  Serial.println(WiFi.localIP());
}


void setup() {
  init_wifi_hardware();

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("ledmatrix-1");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"mypwd");

  ota.init();

  delay(3000);

  // display a hexadecimal number and set the left 4 dots
  module.setDisplayToHexNumber(0x1234ABCD, 0x0F);
}

void loop() {
  ota.loop();

  byte keys = module.getButtons();

  // light the first 4 red LEDs and the last 4 green LEDs as the buttons are pressed
  module.setLEDs(((keys & 0xF0) << 8) | (keys & 0xF));
}
