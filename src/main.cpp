// minivolumidi main script
// packed together by Apfelwurm <Alexander@volzit.de>
// based on the following sources:
// https://github.com/lathoub/Arduino-AppleMIDI-Library/tree/master/examples
// https://github.com/LennartHennigs/ESPRotary/tree/master/examples
// https://github.com/FortySevenEffects/arduino_midi_library/tree/master/examples

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <AppleMIDI.h>
#include <Arduino.h>
#include "Button2.h" 
#include "ESPRotary.h"

/////////////////////////////////////////////////////////////////
// Configure your needs here
/////////////////////////////////////////////////////////////////
char ssid[] = "SSID"; //  your network SSID (name)
char pass[] = "PASSWORD";    // your network password (use for WPA, or use as key for WEP)


#define ROTARY_PIN1 D6 // DT pin of ky-040
#define ROTARY_PIN2 D7 // CLK pin of ky-040
#define BUTTON_PIN  D5 // SW pin of ky-040

#define CLICKS_PER_STEP 2   // this number depends on your rotary encoder
#define MIN_POS         0   // minimum value for the knob
#define MAX_POS         127 // maximum value for the knob
#define START_POS       20  // starting position for the knob
#define INCREMENT       1   // this number is the counter increment on each step

#define MIDI_CC         91  //CC Number that should be used
#define MIDI_CHANNEL    1   // Midi Channel that should be used
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// varible initialisation
/////////////////////////////////////////////////////////////////
bool newpos = false;
bool mute = false;
bool unmute = true;
unsigned long t0 = millis();
int8_t isConnected = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// object initialisation
/////////////////////////////////////////////////////////////////
APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();
ESPRotary r;
Button2 b;    // https://github.com/LennartHennigs/Button2
/////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////
// Rotary encoder functions
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


/////////////////////////////////////////////////////////////////
// setup
/////////////////////////////////////////////////////////////////
void setup() {

  //serial setup
  Serial.begin(115200);

  delay(50);

  //wifi setup
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");

  //RTP Midi setup
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
  
 
  //button setup
  b.begin(BUTTON_PIN);
  b.setTapHandler(click);
  b.setLongClickHandler(resetPosition);

  //rotary setup
  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);

  //init output
  Serial.println("\n\nminivolumidi");
  Serial.println("Is configured for values between " + String(MIN_POS) + " and " + String(MAX_POS) +".");
  Serial.print("Increment: ");
  Serial.println(r.getIncrement());
  Serial.print("Current position: ");
  Serial.println(r.getPosition());
  Serial.println();
}
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// loop
/////////////////////////////////////////////////////////////////
void loop() {
  
  //RTPMIDI processing
  MIDI.read();

  //rotary processing
  r.loop();

  //button processing
  b.loop();

  if ((isConnected > 0) && (millis() - t0) > 1000)
  {
    t0 = millis();

    // mute and unmute
    if (mute){
      MIDI.sendControlChange((byte)MIDI_CC,(byte)MIN_POS,MIDI_CHANNEL);
      mute=false;
    }
    if (unmute)
    {     
      MIDI.sendControlChange((byte)MIDI_CC,(byte)r.getPosition(),MIDI_CHANNEL);
      unmute=false;
    }

    // value update
    if (newpos)
    {
      MIDI.sendControlChange((byte)MIDI_CC,(byte)r.getPosition(),MIDI_CHANNEL);
      newpos = false;
    }



  }

}
/////////////////////////////////////////////////////////////////


