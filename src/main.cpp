#include <Arduino.h>
#include <AudioFileSourceLittleFS.h>
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include <I2S.h>
#include <LittleFS.h>
#include <Scheduler.h>
#include <Task.h>


AudioGeneratorWAV *wav;
AudioFileSourceLittleFS *file_armed;
AudioFileSourceLittleFS *file_pulsearmed;
AudioFileSourceLittleFS *file_activate;
AudioFileSourceLittleFS *file_deactivate;
AudioOutputI2S *out;

#define TRIGGER D1
#define ARMED_LED D5
#define WARNING_LEFT_LED D6
#define WARNING_MIDDLE_LED D7
#define WARNING_RIGHT_LED D2

void littleFsListDir(const char *dirname);
void loop_warning_lights();
void stop_all_lights();

bool warning_lights_on = false;

class AudioTask : public Task
{
private:
  bool triggered = false;
  bool previousState = false;
  bool playingArmed = false;
  bool playingPulse = false;

protected:
  void setup()
  {
    if (!I2S.begin(I2S_PHILIPS_MODE, 44100, 16))
    {
      while (1)
        ; // do nothing
    }
    Serial.println("\nI2S Initialized");
    if (!LittleFS.begin())
    {
      Serial.println("Failed to initialize LittleFS");
    }
    Serial.println("LittleFS Initialized");
    littleFsListDir("/");
    file_armed = new AudioFileSourceLittleFS("/armed.wav");
    file_pulsearmed = new AudioFileSourceLittleFS("/pulse_armed.wav");
    out = new AudioOutputI2S();
    wav = new AudioGeneratorWAV();
    out->SetBitsPerSample(16);
    out->SetRate(44100);
  }
  void loop()
  {
    bool currentState = digitalRead(TRIGGER);
    Serial.println(currentState);

    // Only play sounds when the state changes and no sound is currently playing
    if (!wav->isRunning())
    {
      // State changed from LOW to HIGH
      if (currentState == HIGH && previousState == LOW)
      {
        warning_lights_on = true;
        if (file_activate) file_activate->close();  // Close if exists
        file_activate = new AudioFileSourceLittleFS("/activate.wav");
        if (file_activate->isOpen()) {
          wav->begin(file_activate, out);
          triggered = true;
        } else {
          Serial.println("Failed to open activate.wav");
        }
      }
      // State changed from HIGH to LOW
      else if ((currentState == LOW && previousState == HIGH ) || (currentState == LOW && previousState == HIGH && triggered))
      {
        if (file_deactivate) file_deactivate->close();  // Close if exists
        file_deactivate = new AudioFileSourceLittleFS("/deactivate.wav");
        if (file_deactivate->isOpen()) {
          wav->begin(file_deactivate, out);
          triggered = false;
        } else {
          Serial.println("Failed to open deactivate.wav");
        }
      }
      // If triggered and no sound playing, play armed.wav in loop
      else if (triggered) {
        if (file_armed) file_armed->close();
        file_armed = new AudioFileSourceLittleFS("/armed.wav");
        if (file_armed->isOpen()) {
          wav->begin(file_armed, out);
          triggered = true;
        }
      }
    } 
    else if (currentState == LOW && previousState == HIGH && triggered)
    {
      warning_lights_on = false;
      if (file_deactivate) file_deactivate->close();  // Close if exists
      file_deactivate = new AudioFileSourceLittleFS("/deactivate.wav");
      if (file_deactivate->isOpen()) {
        wav->begin(file_deactivate, out);
        triggered = false;
      } else {
        Serial.println("Failed to open deactivate.wav");
      }
    }
    
    previousState = currentState;
    if (wav->isRunning()) {
      if (!wav->loop()) {
        wav->stop();
      }
    }
  }
}audio_task;

class LightsTask : public Task
{
  protected:
    void setup()
    {
      pinMode(ARMED_LED, OUTPUT);
      pinMode(WARNING_LEFT_LED, OUTPUT);
      pinMode(WARNING_MIDDLE_LED, OUTPUT);
      pinMode(WARNING_RIGHT_LED, OUTPUT);
      pinMode(TRIGGER, INPUT);

      digitalWrite(WARNING_LEFT_LED, LOW);
      digitalWrite(WARNING_MIDDLE_LED, LOW);
      digitalWrite(WARNING_RIGHT_LED, LOW);
      digitalWrite(ARMED_LED, LOW);
    }
    void loop()
    {
      if (warning_lights_on)
      {
        digitalWrite(ARMED_LED, HIGH);
        digitalWrite(WARNING_LEFT_LED, HIGH);
        delay(300);
        digitalWrite(WARNING_LEFT_LED, LOW);
        digitalWrite(WARNING_MIDDLE_LED, HIGH);
        delay(300);
        digitalWrite(WARNING_MIDDLE_LED, LOW);
        digitalWrite(WARNING_RIGHT_LED, HIGH);
        delay(300);
        digitalWrite(WARNING_RIGHT_LED, LOW);
      } else
      {
        digitalWrite(WARNING_LEFT_LED, LOW);
        digitalWrite(WARNING_MIDDLE_LED, LOW);
        digitalWrite(WARNING_RIGHT_LED, LOW);
        digitalWrite(ARMED_LED, LOW);
      }      
    }
}lights_task;


void setup()
{
  Serial.begin(9600);
  Serial.println("");
  delay(500);

  Scheduler.start(&audio_task);
  Scheduler.start(&lights_task);

  Scheduler.begin();

}

void loop(){}

void littleFsListDir(const char *dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.printf(" FILE: %s SIZE: %u\n", root.fileName().c_str(), file.size());
    file.close();
  }
}
