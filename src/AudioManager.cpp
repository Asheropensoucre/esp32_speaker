#include "AudioManager.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "../test/audio_data.h"
#include <driver/i2s.h>

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
  
  // Set up AVRC playback status callback to sync with phone
  a2dp_sink.set_avrc_rn_playstatus_callback([](esp_avrc_playback_stat_t status) {
    Serial.print("AVRC Playback Status Callback - Status: ");
    Serial.println(status);
    if (AudioManager::instance) {
      if (status == ESP_AVRC_PLAYBACK_PLAYING) {
        Serial.println("Phone State: PLAYING");
        AudioManager::instance->playing = true;
      } else if (status == ESP_AVRC_PLAYBACK_PAUSED || status == ESP_AVRC_PLAYBACK_STOPPED) {
        Serial.println("Phone State: PAUSED/STOPPED");
        AudioManager::instance->playing = false;
      }
    }
  });
  
  // Set up Bluetooth connection callback
  a2dp_sink.set_on_connection_state_changed([](esp_a2d_connection_state_t state, void* param) {
    Serial.print("Bluetooth Connection State: ");
    Serial.println(state);
    if (AudioManager::instance) {
      if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) { 
        Serial.println("Phone CONNECTED");
        AudioManager::instance->connected = true;
        AudioManager::instance->playSystemSound(sound_connected, sound_connected_LENGTH); 
      } else { 
        Serial.println("Phone DISCONNECTED"); 
        AudioManager::instance->connected = false;
        AudioManager::instance->playSystemSound(sound_disconnected, sound_disconnected_LENGTH); 
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
  return connected;
}

BluetoothA2DPSink* AudioManager::getSink() {
  return &a2dp_sink;
}

// Static callback function for AVRCP metadata
void AudioManager::avrc_metadata_callback(uint8_t id, const uint8_t *text) {
  Serial.print("AVRCP Metadata Callback - ID: ");
  Serial.println(id);
  if (instance && instance->displayManager) {
    if (id == ESP_AVRC_MD_ATTR_TITLE) {
      Serial.print("Song Title Received: ");
      Serial.println((char*)text);
      instance->displayManager->updateSongTitle((char*)text);
    }
  }
}

void AudioManager::playSystemSound(const unsigned char* audio_data, size_t data_length) {
    int16_t* pcm = (int16_t*)audio_data;
    size_t num_samples = data_length / 2; // 16-bit samples
    int16_t out_buf[4]; // 2 frames of stereo [L, R, L, R]

    for (size_t i = 0; i < num_samples; i++) {
        // Duplicate Mono to Stereo, and 22.05kHz to 44.1kHz
        // Scale volume to 5% (divide by 20)
        int16_t scaled_sample = pcm[i] / 20;
        out_buf[0] = scaled_sample; // Frame 1 Left
        out_buf[1] = scaled_sample; // Frame 1 Right
        out_buf[2] = scaled_sample; // Frame 2 Left
        out_buf[3] = scaled_sample; // Frame 2 Right

        size_t bytes_written;
        i2s_write(I2S_NUM_0, out_buf, sizeof(out_buf), &bytes_written, pdMS_TO_TICKS(10));
    }
}
