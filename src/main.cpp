#include <Arduino.h>
#include <AudioFileSourceSPIFFS.h>
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include <I2S.h>

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;
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
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to initialize SPIFFS");
  }
  Serial.println("SPIFFS Initialized");
  delay(500);
  file = new AudioFileSourceSPIFFS("/pulse_armed.mp3");
  if (!file)
  {
    Serial.println("Error while loading file");
  }
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
  out->SetPinout(D6, D8, D7);
  out->SetBitsPerSample(16);
  out->SetRate(44100);
  mp3->begin(file, out);
}

void loop()
{
  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop(); 
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
  }
}