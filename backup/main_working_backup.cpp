#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "BluetoothA2DPSink.h"

// Power & Amp
#define BACKLIGHT_PIN 32
#define AMP_ENABLE_PIN 14

// Screen Pins 
#define TFT_MOSI 21
#define TFT_DC   22
#define TFT_SCLK 23
#define TFT_CS   33
#define TFT_RST  -1

// Audio Pins 
#define I2S_BCLK 13
#define I2S_LRC  27
#define I2S_DOUT 26

// THE NEW SAFE BUTTON PINS
#define BTN_A 25
#define BTN_B 18
#define BTN_X 19
#define BTN_Y 16  

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
BluetoothA2DPSink a2dp_sink;

bool isPlaying = false;
String currentSong = "potato"; // Default text to hold the song name

// A helper function to easily redraw the song title
void drawScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println("NOW PLAYING:");
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 110);
  tft.println(currentSong);
}

// Automatically grabs the Song Title when a track changes
void avrc_metadata_callback(uint8_t id, const uint8_t *text) {
  if (id == ESP_AVRC_MD_ATTR_TITLE) { 
    currentSong = (char*)text; // Save the song name in memory
    drawScreen();              // Draw it on the screen
  }
}

// A cool pop-up screen for volume changes
void showVolume(int vol) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(3);
  tft.setCursor(20, 110);
  tft.print("VOL: ");
  // The ESP32 volume goes from 0 to 127. This maps it to a 0-100 percentage!
  tft.print(map(vol, 0, 127, 0, 100));
  tft.println("%");
  
  delay(800);   // Hold the volume on screen for just under a second
  drawScreen(); // Go back to the song title!
}

void setup() {
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  pinMode(AMP_ENABLE_PIN, OUTPUT);
  digitalWrite(AMP_ENABLE_PIN, HIGH);
  delay(200);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);

  tft.init(240, 240);
  tft.fillScreen(ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("READY TO PAIR");
  tft.setCursor(20, 140);
  tft.println("sir potato");

  i2s_pin_config_t my_pin_config = {
      .bck_io_num = I2S_BCLK,
      .ws_io_num = I2S_LRC,
      .data_out_num = I2S_DOUT,
      .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(my_pin_config);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  
  a2dp_sink.start("ESP32 Potato");
}

void loop() {
  // Button X (Top Left): PLAY / PAUSE
  if (digitalRead(BTN_X) == LOW) {
    if (isPlaying) {
      a2dp_sink.pause();
      isPlaying = false;
    } else {
      a2dp_sink.play();
      isPlaying = true;
    }
    while(digitalRead(BTN_X) == LOW) { delay(10); }
  }

  // Button Y (Bottom Left): NEXT TRACK
  if (digitalRead(BTN_Y) == LOW) {
    a2dp_sink.next();
    while(digitalRead(BTN_Y) == LOW) { delay(10); }
  }

  // Button A (Top Right): VOLUME UP
  if (digitalRead(BTN_A) == LOW) {
    int vol = a2dp_sink.get_volume();
    if (vol <= 117) vol += 10; // Go up in steps of 10
    else vol = 127;            // 127 is the absolute max volume
    
    a2dp_sink.set_volume(vol);
    showVolume(vol);
    
    while(digitalRead(BTN_A) == LOW) { delay(10); }
  }

  // Button B (Bottom Right): VOLUME DOWN
  if (digitalRead(BTN_B) == LOW) {
    int vol = a2dp_sink.get_volume();
    if (vol >= 10) vol -= 10; // Go down in steps of 10
    else vol = 0;             // 0 is mute
    
    a2dp_sink.set_volume(vol);
    showVolume(vol);
    
    while(digitalRead(BTN_B) == LOW) { delay(10); }
  }
  
  delay(50);
}