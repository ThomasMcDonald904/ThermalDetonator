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

// const int BUFFER_SIZE = 512;
// int16_t audio_buffer[BUFFER_SIZE];

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

// void setup(){
//     Serial.begin(9600);
//     delay(1000);
//     i2s_begin();
//     i2s_set_rate(SAMPLE_RATE);

//     float delta_phase = 2 * PI * frequency/SAMPLE_RATE;
//     float phase = 0.0;

//     for (int i = 0; i < BUFFER_SIZE; i++)
//     {
//         uint32_t sample = (int16_t)(AMPLITUDE * sin(phase)) & 0xffff;
//         sample = sample | (sample << 16);
//         audio_buffer[i] = sample;
//         phase += delta_phase;
//         if (phase >= 2*PI)
//         {
//             phase -= 2*PI;
//         }
        
//     }
//     Serial.println("I2S initialized");
// }

// void loop(){
//     for (int i = 0; i < BUFFER_SIZE; i++)
//     {
//         Serial.print(">sine:");
//         Serial.println(audio_buffer[i]);
//         i2s_write_sample(audio_buffer[i]);
//     }
// }


// int count = 0;


// void setup() 
// {
//   Serial.begin(9600);
//   if (!I2S.begin(I2S_PHILIPS_MODE, I2S_SAMPLE_RATE, I2S_SAMPLE_BITS))
//   {

//     Serial.println("Failed to initialize I2S!");

//     while (1)
//       ; // do nothing
//   }
// }
// int16_t GenerateSineWave()
// {
//     double rad = 2 * M_PI * 1000 * count++ / I2S_SAMPLE_RATE;
//     int16_t sineVal = 32767 * sin(rad);
//     return sineVal;
// }
// void loop()
// {
//     int16_t sine_wave = GenerateSineWave();
//     I2S.write(sine_wave);
//     I2S.write(sine_wave);
//     Serial.print(">sine:");
//     Serial.print(sine_wave);
//     Serial.print("\n");
// }