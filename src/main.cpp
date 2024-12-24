#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "AudioFileSourceLittleFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// To run, set your ESP8266 build to 160MHz, and include a SPIFFS of 512KB or greater.
// Use the "Tools->ESP8266/ESP32 Sketch Data Upload" menu to write the MP3 to SPIFFS
// Then upload the sketch normally.

AudioGeneratorMP3 mp3; // Static allocation
AudioFileSourceLittleFS file("/pulse_armed.mp3"); // Static allocation
AudioOutputI2SNoDAC out; // Static allocation
AudioFileSourceID3 id3(&file); // Static allocation

void littleFsListDir(const char *dirname);

// Called when a metadata event occurs (i.e., an ID3 tag, an ICY block, etc.)
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2; // Skip BOM for Unicode
  }

  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++; // Skip the second byte in Unicode
    }
    Serial.printf("%c", a);
  }
  Serial.println("'");
}

void setup() {
  WiFi.mode(WIFI_OFF); 
  Serial.begin(9600);
  delayMicroseconds(50);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Error while mounting LittleFS. Halting...");
    while (true); // Halt execution if LittleFS fails
  }

  Serial.println("Sample MP3 playback begins...");
  delayMicroseconds(50);
  littleFsListDir("/");

  audioLogger = &Serial;
  
  // Check file existence and size
  if (!LittleFS.exists("/pulse_armed.mp3")) {
    Serial.println("Error: MP3 file not found!");
    while (true); // Halt if file is missing
  }

  File testFile = LittleFS.open("/pulse_armed.mp3", "r");
  if (!testFile || testFile.size() == 0) {
    Serial.println("Error: MP3 file is empty or inaccessible!");
    testFile.close();
    while (true); // Halt if file is empty
  }
  testFile.close();

  // Initialize components
  id3.RegisterMetadataCB(MDCallback, nullptr);

  if (!mp3.begin(&id3, &out)) {
    Serial.println("Error: Failed to initialize MP3 decoder.");
    while (true); // Halt on initialization failure
  }
}

void loop() {
  if (mp3.isRunning()) {
    if (!mp3.loop()) {
      mp3.stop();
    }
  } else {
    Serial.println("MP3 done");
    delay(1000);
  }
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

