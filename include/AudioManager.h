#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include "BluetoothA2DPSink.h"
#include "DisplayManager.h"

/**
 * AudioManager - Handles Bluetooth A2DP audio streaming and I2S output
 * 
 * Manages:
 * - Bluetooth A2DP sink (device name: "ESP32 Potato")
 * - I2S audio output configuration
 * - AVRCP metadata callbacks for song titles
 * - Volume control (0-127 range)
 * - Playback control (play, pause, next)
 * 
 * I2S Pin Configuration:
 * - BCLK: GPIO 13
 * - LRC/WS: GPIO 27
 * - DOUT: GPIO 26
 * - DIN: Not connected (I2S_PIN_NO_CHANGE)
 */

class AudioManager {
public:
  AudioManager(DisplayManager* displayMgr);
  
  // Initialize audio system
  void begin();
  
  // Start Bluetooth A2DP sink
  void start(const char* deviceName);
  
  // Playback control
  void play();
  void pause();
  void next();
  
  // Volume control
  void setVolume(int volume);
  int getVolume();
  void volumeUp();
  void volumeDown();
  
  // Check if currently playing
  bool isPlaying();
  
  // Check if Bluetooth is connected
  bool isConnected();
  
  // Get the BluetoothA2DPSink object (for advanced usage)
  BluetoothA2DPSink* getSink();
  
  // Play system sound (boot, connected, disconnected, error)
  void playSystemSound(const unsigned char* audio_data, size_t data_length);

private:
  BluetoothA2DPSink a2dp_sink;
  DisplayManager* displayManager;
  
  // I2S pin configuration
  static const int I2S_BCLK = 13;
  static const int I2S_LRC = 27;
  static const int I2S_DOUT = 26;
  
  // Playback state
  bool playing;
  bool connected = false;
  
  // AVRCP metadata callback
  static void avrc_metadata_callback(uint8_t id, const uint8_t *text);
  
  // Static pointer to instance for callback
  static AudioManager* instance;
};

#endif // AUDIO_MANAGER_H
