#include <Arduino.h>
#include <AudioFileSourceLittleFS.h>
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include <I2S.h>
#include <LittleFS.h>

AudioGeneratorWAV *wav;
AudioFileSourceLittleFS *file1;
AudioFileSourceLittleFS *file2;
AudioOutputI2S *out;

void littleFsListDir(const char *dirname);

void setup()
{
  Serial.begin(9600);
  delay(500);
  if (!I2S.begin(I2S_PHILIPS_MODE, 44100, 16))
  {

    Serial.println("Failed to initialize I2S!");

    while (1)
      ; // do nothing
  }
  Serial.println("\nI2S Initialized");
  delay(500);
  if (!LittleFS.begin())
  {
    Serial.println("Failed to initialize LittleFS");
  }
  Serial.println("LittleFS Initialized");
  littleFsListDir("/");
  delay(500);
  file1 = new AudioFileSourceLittleFS("/armed.wav");
  file2 = new AudioFileSourceLittleFS("/pulse_armed.wav");
  if (!file1)
  {
    Serial.println("Error while loading file");
  }
  out = new AudioOutputI2S();
  wav = new AudioGeneratorWAV();
  out->SetBitsPerSample(16);
  out->SetRate(44100);
  wav->begin(file1, out);
}

void loop()
{
  wav->loop();
}

void littleFsListDir(const char *dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.printf(" FILE: %s SIZE: %u\n", root.fileName().c_str(), file.size());
    file.close();
  }
}