#include <Arduino.h>
// const int I2S_SAMPLE_RATE = 16000;
// const int I2S_SAMPLE_BITS = 16;
#include <I2S.h>

const int SAMPLE_RATE = 16000;
const int AMPLITUDE = 20000;
const float FREQUENCY = 261.63;
float phase = 0.0;

// const int BUFFER_SIZE = 512;
// int16_t audio_buffer[BUFFER_SIZE];

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("Starting I2S...");
  
  // Initialize I2S
  i2s_begin();
  i2s_set_rate(SAMPLE_RATE);
  
  Serial.println("I2S initialized");
}

void loop() {
  // Generate samples in real-time instead of from pre-calculated buffer
  static unsigned long lastSampleTime = 0;
  const unsigned long sampleInterval = 1000000 / SAMPLE_RATE; // in microseconds
  
  if (micros() - lastSampleTime >= sampleInterval) {
    // Generate next sample
    int16_t sampleValue = (int16_t)(AMPLITUDE * sin(phase));
    uint32_t sample = (sampleValue & 0xFFFF) | ((sampleValue & 0xFFFF) << 16);
    
    // Write to I2S
    i2s_write_sample(sample);
    
    // Update phase for next sample
    phase += TWO_PI * FREQUENCY / SAMPLE_RATE;
    if (phase >= TWO_PI) {
      phase -= TWO_PI;
    }
    
    lastSampleTime += sampleInterval;
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