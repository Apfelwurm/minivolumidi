
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <AppleMIDI.h>
#include <Arduino.h>
#include "Button2.h" 
#include "ESPRotary.h"

char ssid[] = "SSID"; //  your network SSID (name)
char pass[] = "PASSWORD";    // your network password (use for WPA, or use as key for WEP)

unsigned long t0 = millis();
int8_t isConnected = 0;

#define ROTARY_PIN1 D6
#define ROTARY_PIN2 D7
#define BUTTON_PIN  D5

#define CLICKS_PER_STEP 2   // this number depends on your rotary encoder
#define MIN_POS         0
#define MAX_POS         127
#define START_POS       20
#define INCREMENT       1   // this number is the counter increment on each step

bool newpos = false;
bool mute = false;
bool unmute = true;

/////////////////////////////////////////////////////////////////
APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();
ESPRotary r;
Button2 b;    // https://github.com/LennartHennigs/Button2
/////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////

// on change
void rotate(ESPRotary& r) {
   Serial.println(r.getPosition());
   newpos = true;
}

// on left or right rotation
void showDirection(ESPRotary& r) {
  Serial.println(r.directionToString(r.getDirection()));
}

// single click
void click(Button2& btn) {
  mute=true;
  Serial.println("Click!");
}

// long click
void resetPosition(Button2& btn) {
  unmute = true;
  Serial.println("Reset!");
}

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);

   WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");

  Serial.println(F("OK, now make sure you an rtpMIDI session that is Enabled"));
  Serial.println(WiFi.localIP().toString()); 
  Serial.println(AppleMIDI.getPort()); 
  Serial.println(AppleMIDI.getName()); 
  Serial.println(F("Select and then press the Connect button"));
  Serial.println(F("Then open a MIDI listener and monitor incoming notes"));
  Serial.println(F("Listen to incoming MIDI commands"));
 
  MIDI.begin();
  
 AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
    isConnected++;
    Serial.println(F("Connected to session"));
    Serial.println(ssrc); 
    Serial.println(name); 

  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
    isConnected--;
    Serial.println(F("Disconnected"));
    Serial.println(ssrc); 
  });
  
 
  b.begin(BUTTON_PIN);
  b.setTapHandler(click);
  b.setLongClickHandler(resetPosition);

  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);

  Serial.println("\n\nRanged Counter");
  Serial.println("You can only set values between " + String(MIN_POS) + " and " + String(MAX_POS) +".");
  Serial.print("Increment: ");
  Serial.println(r.getIncrement());
  Serial.print("Current position: ");
  Serial.println(r.getPosition());
  Serial.println();
}

void loop() {
  MIDI.read();
  r.loop();
  b.loop();

  if ((isConnected > 0) && (millis() - t0) > 1000)
  {
    t0 = millis();
    if (mute){
      MIDI.sendControlChange((byte)91,(byte)0,1);
      mute=false;
    }
    if (unmute)
    {     
      MIDI.sendControlChange((byte)91,(byte)r.getPosition(),1);
      unmute=false;
    }

    if (newpos)
    {
      MIDI.sendControlChange((byte)91,(byte)r.getPosition(),1);
      newpos = false;
    }



  }

}


