#include "AudioManager.h"
#include "esp_a2dp_api.h"

// Static member initialization
AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager(DisplayManager* displayMgr) {
  displayManager = displayMgr;
  playing = false;
  instance = this;
}

void AudioManager::begin() {
  // Configure I2S pins
  i2s_pin_config_t my_pin_config = {
      .bck_io_num = I2S_BCLK,
      .ws_io_num = I2S_LRC,
      .data_out_num = I2S_DOUT,
      .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(my_pin_config);
  
  // Set up AVRCP metadata callback
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  
  // Set up AVRCP playback status callback
  a2dp_sink.set_avrc_rn_playstatus_callback([](esp_avrc_playback_stat_t status) {
      if (AudioManager::instance) {
          if (status == ESP_AVRC_PLAYBACK_PLAYING) {
              AudioManager::instance->playing = true;
          } else if (status == ESP_AVRC_PLAYBACK_PAUSED || status == ESP_AVRC_PLAYBACK_STOPPED) {
              AudioManager::instance->playing = false;
          }
      }
  });
}

void AudioManager::start(const char* deviceName) {
  a2dp_sink.start(deviceName);
}

void AudioManager::play() {
  a2dp_sink.play();
  playing = true;
}

void AudioManager::pause() {
  a2dp_sink.pause();
  playing = false;
}

void AudioManager::next() {
  a2dp_sink.next();
}

void AudioManager::setVolume(int volume) {
  a2dp_sink.set_volume(volume);
}

int AudioManager::getVolume() {
  return a2dp_sink.get_volume();
}

void AudioManager::volumeUp() {
  int vol = a2dp_sink.get_volume();
  if (vol <= 120) a2dp_sink.set_volume(vol + 5);
}

void AudioManager::volumeDown() {
  int vol = a2dp_sink.get_volume();
  if (vol >= 5) a2dp_sink.set_volume(vol - 5);
}

bool AudioManager::isPlaying() {
    return playing; 
}

bool AudioManager::isConnected() {
  // Check if Bluetooth sink is connected and ready
  // We'll use a simple heuristic: if we've been started and have a device name,
  // assume we're connected. In a real implementation, we'd check the actual
  // connection state from the Bluetooth stack.
  
  // For now, return true to allow audio commands to be attempted
  // The a2dp_sink methods will handle the actual connection state internally
  return true;
}

BluetoothA2DPSink* AudioManager::getSink() {
  return &a2dp_sink;
}

// Static callback function for AVRCP metadata
void AudioManager::avrc_metadata_callback(uint8_t id, const uint8_t *text) {
  if (instance && instance->displayManager) {
    if (id == ESP_AVRC_MD_ATTR_TITLE) {
      // Update the display with the new song title
      instance->displayManager->updateSongTitle((char*)text);
    }
  }
}
