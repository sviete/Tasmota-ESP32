/*
  xdrv_42_0_i2s_0_lib_idf51.ino - Simplified Audio library, core class

  Copyright (C) 2021  Gerhard Mutz, Theo Arends, Staars, Stephan Hadinger

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(ESP32) && ESP_IDF_VERSION_MAJOR >= 5
#ifdef USE_I2S_AUDIO

/*********************************************************************************************\
 * Driver Settings in memory
\*********************************************************************************************/

/*********************************************************************************************\
 * Reminder of esp_err codes
 * 
 * #define ESP_OK          0       - esp_err_t value indicating success (no error) 
 * #define ESP_FAIL        -1      - Generic esp_err_t code indicating failure 
 * 
 * #define ESP_ERR_NO_MEM              0x101  257 - Out of memory 
 * #define ESP_ERR_INVALID_ARG         0x102  258 - Invalid argument 
 * #define ESP_ERR_INVALID_STATE       0x103  259 - Invalid state 
 * #define ESP_ERR_INVALID_SIZE        0x104  260 - Invalid size 
 * #define ESP_ERR_NOT_FOUND           0x105  261 - Requested resource not found 
 * #define ESP_ERR_NOT_SUPPORTED       0x106  262 - Operation or feature not supported 
 * #define ESP_ERR_TIMEOUT             0x107  263 - Operation timed out 
 * #define ESP_ERR_INVALID_RESPONSE    0x108  264 - Received response was invalid 
 * #define ESP_ERR_INVALID_CRC         0x109  265 - CRC or checksum was invalid 
 * #define ESP_ERR_INVALID_VERSION     0x10A  266 - Version was invalid 
 * #define ESP_ERR_INVALID_MAC         0x10B  267 - MAC address was invalid 
 * #define ESP_ERR_NOT_FINISHED        0x10C  268 - Operation has not fully completed 
\*********************************************************************************************/

/*********************************************************************************************\
 * This is the central class to acccess I2S in (rx) or out (tx)
 * 
 * It inherits from AudioOutput so it can be used as output instance for ESP8266Audio library
 * 
 * It also supports microphone input
 * 
\*********************************************************************************************/

class TasmotaI2S : public AudioOutput
{
public:

  // Constructor takes no parameter, everything is configured from template and config file
  TasmotaI2S() {
    // set some defaults
    hertz = 16000;
    bps = I2S_DATA_BIT_WIDTH_16BIT;
    channels = 2;
    gainF2P6 = 32;    // equivalent of 0.5
  }

  ~TasmotaI2S() {
    this->stop();
  }

  // Settings
  void setPinout(int32_t bclk, int32_t ws, int32_t dout, int32_t mclk, int32_t din,
                 bool mclk_inv = false, bool bclk_inv = false, bool ws_inv = false, bool apll = false);

  void setSlotConfig(i2s_port_t i2s_port, uint8_t tx_slot_config, uint8_t rx_slot_config,
                     uint8_t tx_slot_mask, uint8_t rx_slot_mask) {
    _i2s_port = i2s_port;
    _tx_slot_config = tx_slot_config;
    _rx_slot_config = rx_slot_config;
  }
  void setRxFreq(uint16_t freq) { _rx_freq = freq; }

  // ------------------------------------------------------------------------------------------
  // Setters for configuration parameters
  //
  // TODO: not sure we still need them since all this should be set at initialiation
  // ------------------------------------------------------------------------------------------
  virtual bool SetBitsPerSample(int bits) {
    AddLog(LOG_LEVEL_DEBUG,PSTR("I2S: SetBitsPerSample: %i"), bits);
    if ( (bits != 16) && (bits != 8) ) { return false; }
    this->bps = bits;
    return true;
  }

  virtual bool SetChannels(int channels) {
    AddLog(LOG_LEVEL_DEBUG,PSTR("I2S: SetChannels: %i"), channels);
    if ((channels < 1) || (channels > 2)) { return false; }
    if (channels == (int)this->channels) { return true; }
    this->channels = channels;
    return true;
  }

  virtual bool SetRate(int hz) {
    AddLog(LOG_LEVEL_DEBUG,PSTR("I2S: SetRate: %i was %i on=%i"), hz, this->hertz, _i2s_on);
    if (hz == (int) this->hertz) { return true; }
    this->hertz = hz;
    if (_i2s_on) {
      int result = updateClockConfig();
    }
    return true;
  }

  virtual bool SetGain(float f) {
    AddLog(LOG_LEVEL_DEBUG,PSTR("I2S: SetGain: %_f"), &f);
    return AudioOutput::SetGain(f);
  }

  // ------------------------------------------------------------------------------------------
  // Getters
  inline bool isDuplex(void) const { return _tx_configured && _rx_configured; }

  inline bool getExclusive(void) const {return _exclusive; }
  inline uint8_t getTxMode(void) const { return _tx_mode; }
  inline uint8_t getTxChannels(void) const { return channels; }
  inline bool getTxRunning(void) const { return _tx_running; }
  inline i2s_chan_handle_t getTxHandle(void) const { return _tx_handle; }

  inline uint8_t getRxMode(void) const { return _rx_mode; }
  inline uint8_t getRxBitsPerSample(void) const { return 16; }      // TODO - hardcoded to 16 bits for recording
  inline uint16_t getRxRate(void) const { return _rx_freq; }
  inline uint8_t getRxChannels(void) const { return _rx_channels; }
  inline float getRxGain(void) const { return 1.0f; }               // TODO - hardcoded to 1.0 for recording
  inline bool getRxRunning(void) const { return _rx_running; }
  inline i2s_chan_handle_t getRxHandle(void) const { return _rx_handle; }

  // ------------------------------------------------------------------------------------------
  // Setters
  inline void setExclusive(bool exclusive) { _exclusive = exclusive; }
  inline void setTxMode(uint8_t mode) { _tx_mode = mode; }
  inline void setTxChannels(uint8_t channels) { SetChannels(channels); }
  inline void setTxRunning(bool running) { _tx_running = running; }
  inline void setRxMode(uint8_t mode) { _rx_mode = mode; }
  inline void setRxChannels(uint8_t channels) { _rx_channels = channels; }
  inline void setRxRunning(bool running) { _rx_running = running; }

  // ------------------------------------------------------------------------------------------
  // AudioOutput has inconsistent case for methods, provide consistent setters for super class
  inline void setRate(int hz) { SetRate(hz); } 
  inline void setBitsPerSample(int bits) { SetBitsPerSample(bits); }
  inline void setChannels(int chan) { SetChannels(chan); }
  inline void setGain(float f) { SetGain(f); }

  bool begin(void);
  bool stop(void);
  bool ConsumeSample(int16_t sample[2]);
  bool startI2SChannel(bool tx, bool rx);
  int updateClockConfig(void);

  // The following is now the preferred function
  // and allows to send multiple samples at once
  //
  // Max 128 samples, it is clipped otherwise
  // Returns: the number of samples actually consumed
  //   or -1 if an error occured
  //
  // The call is non blocking and does not wait
  int32_t consumeSamples(int16_t *samples, size_t count);

  // ------------------------------------------------------------------------------------------
  // Microphone related methods
  uint32_t I2sMicInit(void);
  void I2sMicDeinit(void);

protected:
  void loadSettings(void);            // load all settings from Settings file and template - the goal is to have zero touch

protected:

  bool    _i2s_on = false;                // is I2S audio active
  bool    _exclusive = false;             // in exclusive mode, stopping this instance needs to uninstall driver, and reinstall for next use
  i2s_port_t  _i2s_port = I2S_NUM_AUTO;   // I2S port, I2S_NUM_0/I2S_NUM_1/I2S_NUM_AUTO

  // local copy of useful settings for audio
  // TX
  bool    _tx_configured = false;         // true = configured, false = not configured
  uint8_t _tx_mode = I2S_MODE_STD;        // I2S_MODE_STD / I2S_MODE_PDM / I2S_MODE_TDM / I2S_MODE_DAC
  uint8_t _tx_slot_mask = I2S_SLOT_NOCHANGE;
  bool    _tx_running = false;            // true = enabled, false = disabled
  // uint8_t    _tx_channels = 2;            // number of channels, 1 = mono, 2 = stereo -- `channels`
  i2s_chan_handle_t _tx_handle = nullptr; // I2S channel handle, automatically computed
  uint8_t    _tx_slot_config = I2S_SLOT_MSB;// I2S slot configuration

  // RX
  bool      _rx_configured = false;       // true = configured, false = not configured
  uint8_t   _rx_mode = I2S_MODE_STD;      // I2S_MODE_STD / I2S_MODE_PDM / I2S_MODE_TDM / I2S_MODE_DAC
  uint8_t   _rx_slot_mask = I2S_SLOT_NOCHANGE;
  bool      _rx_running = false;          // true = enabled, false = disabled
  uint8_t   _rx_channels = 2;             // number of channels, 1 = mono, 2 = stereo
  i2s_chan_handle_t _rx_handle = nullptr; // I2S channel handle, automatically computed
  uint8_t   _rx_slot_config = I2S_SLOT_MSB;// I2S slot configuration
  uint16_t  _rx_freq = 16000;             // I2S Rx sampling frequency in Hz

  // GPIOs for I2S
  gpio_num_t  _gpio_mclk = GPIO_NUM_NC;   // GPIO for master clock
  gpio_num_t  _gpio_bclk = GPIO_NUM_NC;   // GPIO for bit clock
  gpio_num_t  _gpio_ws = GPIO_NUM_NC;     // GPIO for word select
  gpio_num_t  _gpio_dout = GPIO_NUM_NC;   // GPIO for data out
  gpio_num_t  _gpio_din = GPIO_NUM_NC;    // GPIO for data in
  bool    _gpio_mclk_inv = false;         // invert master clock
  bool    _gpio_bclk_inv = false;         // invert bit clock
  bool    _gpio_ws_inv = false;           // invert word select
  bool    _apll = false;                  // use APLL instead of PLL
};


// ------------------------------------------------------------------------------------------
// Methods
// ------------------------------------------------------------------------------------------

void TasmotaI2S::setPinout(int32_t bclk, int32_t ws, int32_t dout, int32_t mclk, int32_t din,
                bool mclk_inv, bool bclk_inv, bool ws_inv, bool apll) {
  _gpio_mclk = (gpio_num_t) mclk;
  _gpio_bclk = (gpio_num_t) bclk;
  _gpio_ws   = (gpio_num_t) ws;
  _gpio_dout = (gpio_num_t) dout;
  _gpio_din  = (gpio_num_t) din;
  _gpio_mclk_inv = mclk_inv;
  _gpio_bclk_inv = bclk_inv;
  _gpio_ws_inv = ws_inv;
  _apll = apll;

  _tx_configured = (_gpio_dout != GPIO_NUM_NC);
  _rx_configured = (_gpio_din != GPIO_NUM_NC);

  AddLog(LOG_LEVEL_DEBUG, "I2S: setPinout: gpios[%i,%i,%i,%i,%i] inv[%i,%i,%i] apll:%i _tx_configured:%i _rx_configured:%i",
        _gpio_mclk, _gpio_bclk, _gpio_ws, _gpio_dout, _gpio_din,
        _gpio_mclk_inv, _gpio_bclk_inv, _gpio_ws_inv, _apll,
        _tx_configured, _rx_configured);
}

bool TasmotaI2S::begin() {
  AddLog(LOG_LEVEL_DEBUG, "I2S: begin _tx_running:%i _i2s_on:%i", _tx_running, _i2s_on);
  if (_tx_running) { return true; }
  // if (!_i2s_on) {
  //   if ((!_rx_configured || !_tx_configured) && _rx_configured) {    // not duplex -- TODO ?
  //     this->startI2SChannel();
  //   }
  // }
  int result = i2s_channel_enable(_tx_handle);
  if (result != 0){
      AddLog(LOG_LEVEL_INFO, "I2S: Could not enable i2s_channel: %i", result);
      return false;
  }
  _tx_running = true;
  AddLog(LOG_LEVEL_DEBUG, "I2S: begin _tx_running succeeded");
  return true;
}

bool TasmotaI2S::stop() {
  i2s_channel_disable(_tx_handle);
  if ((!_rx_configured || !_tx_configured) && _rx_configured) {    // not duplex -- TODO ?
    i2s_del_channel(_tx_handle);
    _i2s_on = false;
    AddLog(LOG_LEVEL_INFO, "I2S: stop: I2S channel disabled");
  }
  _tx_running = false;
  return true;
}

bool TasmotaI2S::ConsumeSample(int16_t sample[2]) {
  return consumeSamples(sample, 1);
}

int32_t TasmotaI2S::consumeSamples(int16_t *samples, size_t count) {
  if (!_tx_running) { return -1; }
  if (count == 0) { return 0; }
  if (count > 128) { count = 128; }

  int16_t ms[count*2];
  for (int32_t i = 0; i < count; i++) {
    int16_t left = samples[i*2 + LEFTCHANNEL];
    int16_t right = samples[i*2 + RIGHTCHANNEL];

    if (channels == 1) {    // if mono, average the two samples
      // Average the two samples and overwrite
      int32_t ttl = left + right;
      left = right = (ttl>>1) & 0xffff;
    }

    if (bps == 8) {
      left = (((int16_t)(left & 0xff)) - 128) << 8;
      right = (((int16_t)(right & 0xff)) - 128) << 8;
    }

    if (_tx_mode == I2S_MODE_DAC) {
      left = Amplify(left) + 0x8000;
      right = Amplify(right) + 0x8000;
    } else {
      left = Amplify(left);
      right = Amplify(right);
    }

    ms[i*2 + LEFTCHANNEL] = left;
    ms[i*2 + RIGHTCHANNEL] = right;
  }

  // AddLog(LOG_LEVEL_DEBUG, "I2S: consumeSamples: left=%i right=%i", ms[0], ms[1]);

  size_t i2s_bytes_written;
  esp_err_t err = i2s_channel_write(_tx_handle, ms, sizeof(ms), &i2s_bytes_written, 0);
  if (err && err != ESP_ERR_TIMEOUT) {
    AddLog(LOG_LEVEL_INFO, "I2S: Could not write samples (count=%i): %i", count, err);
    return -1;
  }
  return i2s_bytes_written;
}

// Initialize I2S channel
// return `true` if successful, `false` otherwise
bool TasmotaI2S::startI2SChannel(bool tx, bool rx) {
  if (!tx) { _tx_configured = false; }
  if (!rx) { _rx_configured = false; }
  if (!_tx_configured && !_rx_configured) { return false; }   // nothing configured

  esp_err_t err = ESP_OK;
  gpio_num_t _DIN = I2S_GPIO_UNUSED;          // no input pin by default

  if (_tx_configured) {
    // default dma_desc_num = 6 (DMA buffers), dma_frame_num = 240 (frames per buffer)
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);

    AddLog(LOG_LEVEL_DEBUG, "I2S: tx_chan_cfg id:%i role:%i dma_desc_num:%i dma_frame_num:%i auto_clear:%i",
          tx_chan_cfg.id, tx_chan_cfg.role, tx_chan_cfg.dma_desc_num, tx_chan_cfg.dma_frame_num, tx_chan_cfg.auto_clear);

    if (_tx_configured && _rx_configured) {
      _DIN = (gpio_num_t)_gpio_din;
      err = i2s_new_channel(&tx_chan_cfg, &_tx_handle, &audio_i2s.out->_rx_handle);
    } else{
      err = i2s_new_channel(&tx_chan_cfg, &_tx_handle, NULL);
    }

    AddLog(LOG_LEVEL_DEBUG, "I2S: i2s_new_channel err:%i", err);

    // by default we configure for MSB 2 slots `I2S_STD_MSB_SLOT_DEFAULT_CONFIG`
    i2s_std_config_t tx_std_cfg = {
      .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(hertz),
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG((i2s_data_bit_width_t)bps, (i2s_slot_mode_t)channels),
      .gpio_cfg = {
        .mclk = _gpio_mclk,
        .bclk = _gpio_bclk,
        .ws   = _gpio_ws,
        .dout = _gpio_dout,
        .din  =  _DIN,
        .invert_flags = {
          .mclk_inv = _gpio_mclk_inv,
          .bclk_inv = _gpio_bclk_inv,
          .ws_inv   = _gpio_ws_inv,
        }
      }
    };

    // change configuration if we are using PCM or PHILIPS
    if (_tx_slot_config == I2S_SLOT_PCM) {    // PCM
      tx_std_cfg.slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG((i2s_data_bit_width_t)bps, (i2s_slot_mode_t)channels);
    } else if (_tx_slot_config == I2S_SLOT_PHILIPS) { // PHILIPS
      tx_std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG((i2s_data_bit_width_t)bps, (i2s_slot_mode_t)channels);
    }
    if (_tx_slot_mask != I2S_SLOT_NOCHANGE) { tx_std_cfg.slot_cfg.slot_mask = (i2s_std_slot_mask_t)_tx_slot_mask; }

    // AddLog(LOG_LEVEL_INFO, ">>>: I2S tx_chan_cfg=%*_H", sizeof(tx_chan_cfg), &tx_chan_cfg);
    // AddLog(LOG_LEVEL_INFO, ">>>: I2S tx_std_cfg=%*_H", sizeof(tx_std_cfg), &tx_std_cfg);

    err = i2s_channel_init_std_mode(_tx_handle, &tx_std_cfg);
    AddLog(LOG_LEVEL_DEBUG, "I2S: TX channel bits:%i channels:%i hertz:%i initialized err=0x%04X", bps, channels, hertz, err);
    if (err != ERR_OK) {
      _i2s_on = false;
      return false;
    }

    _i2s_on = true;
    if (_rx_configured) {   // full duplex mode
      err = i2s_channel_init_std_mode(audio_i2s.out->_rx_handle, &tx_std_cfg);
      AddLog(LOG_LEVEL_DEBUG, "I2S: i2s_channel_init_std_mode err:%i", err);
      AddLog(LOG_LEVEL_DEBUG, "I2S: RX channel added in full duplex mode");
    }
  }   // if (tx)

  // configure Rx Microphone
  if (_rx_configured && !_tx_configured) {    // if Microphone and not duplex
    gpio_num_t clk_gpio;

    i2s_slot_mode_t slot_mode = (_rx_channels == 1) ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO;
    AddLog(LOG_LEVEL_DEBUG, "I2S: mic init rx_channels:%i rx_running:%i rx_handle:%p", slot_mode, _rx_running, _rx_handle);

    if (_tx_configured && _rx_running) {      // duplex mode, mic was already initialized
      AddLog(LOG_LEVEL_DEBUG, "I2S: mic init exit Rx already enabled");
      return 0; // no need to en- or disable when in full duplex mode and already initialized
    }

    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);

    AddLog(LOG_LEVEL_DEBUG, "I2S: rx_chan_cfg id:%i role:%i dma_desc_num:%i dma_frame_num:%i auto_clear:%i",
          rx_chan_cfg.id, rx_chan_cfg.role, rx_chan_cfg.dma_desc_num, rx_chan_cfg.dma_frame_num, rx_chan_cfg.auto_clear);

    err = i2s_new_channel(&rx_chan_cfg, NULL, &_rx_handle);
    AddLog(LOG_LEVEL_DEBUG, "I2S: mic init i2s_new_channel err=%i", err);
    switch (_rx_mode){
      case I2S_MODE_PDM:
        {
          i2s_pdm_rx_config_t rx_pdm_cfg = {
            .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(_rx_freq),
            /* The default mono slot is the left slot (whose 'select pin' of the PDM microphone is pulled down) */
            .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, (i2s_slot_mode_t)_rx_channels),
            .gpio_cfg = {
              .clk = _gpio_ws,
              .din = _gpio_din,
              .invert_flags = {
                .clk_inv = _gpio_ws_inv,
              },
            },
          };
          // if (_rx_slot_mask != I2S_SLOT_NOCHANGE) { rx_pdm_cfg.slot_cfg.slot_mask = (i2s_pdm_slot_mask_t)_rx_slot_mask; }

          // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg clk_cfg sample_rate_hz:%i clk_src:%i mclk_multiple:%i dn_sample_mode:%i",
          //       rx_pdm_cfg.clk_cfg.sample_rate_hz, rx_pdm_cfg.clk_cfg.clk_src, rx_pdm_cfg.clk_cfg.mclk_multiple, rx_pdm_cfg.clk_cfg.dn_sample_mode);

          // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg slot_cfg data_bit_width:%i slot_bit_width:%i slot_mode:%i slot_mask:%i",
          //       rx_pdm_cfg.slot_cfg.data_bit_width, rx_pdm_cfg.slot_cfg.slot_bit_width, rx_pdm_cfg.slot_cfg.slot_mode, rx_pdm_cfg.slot_cfg.slot_mask);

          // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg gpio_cfg clk:%i din:%i clk_inv:%i",
          //       rx_pdm_cfg.gpio_cfg.clk, rx_pdm_cfg.gpio_cfg.din, rx_pdm_cfg.gpio_cfg.invert_flags.clk_inv);

          err = i2s_channel_init_pdm_rx_mode(_rx_handle, &rx_pdm_cfg);
          AddLog(LOG_LEVEL_DEBUG, PSTR("I2S: RX channel in PDM mode, CLK: %i, DIN: %i, 16 bit width, %i channel(s), err code: 0x%04X"),
                  _gpio_ws, _gpio_din, _rx_channels, err);
          _i2s_on = true;
        }
        break;
      case I2S_MODE_STD:
        {        
          i2s_std_config_t rx_std_cfg = {
            .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(_rx_freq),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, slot_mode),
            .gpio_cfg = {
              .mclk = (gpio_num_t)Pin(GPIO_I2S_MCLK),
              .bclk = (gpio_num_t)Pin(GPIO_I2S_BCLK),
              .ws   = (gpio_num_t)Pin(GPIO_I2S_WS),
              .dout = I2S_GPIO_UNUSED,
              .din  = (gpio_num_t)Pin(GPIO_I2S_DIN),
              .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
              },
            },
          };
          if (_rx_slot_mask != I2S_SLOT_NOCHANGE) { rx_std_cfg.slot_cfg.slot_mask = (i2s_std_slot_mask_t)_rx_slot_mask; }

          err = i2s_channel_init_std_mode(audio_i2s.rx_handle, &rx_std_cfg);
          AddLog(LOG_LEVEL_DEBUG, "I2S: RX i2s_channel_init_std_mode err:%i", err);
          AddLog(LOG_LEVEL_DEBUG, "I2S: RX channel in standard mode with 16 bit width on %i channel(s) initialized", slot_mode);
          _i2s_on = true;
        }
        break;
      default:
        AddLog(LOG_LEVEL_INFO, "I2S: invalid rx mode=%i", _rx_mode);
    }
  }
  return true;
}

int TasmotaI2S::updateClockConfig(void) {
  i2s_channel_disable(_tx_handle);
  i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(hertz);
#ifdef SOC_I2S_SUPPORTS_APLL
  if (_apll) {
    clk_cfg.clk_src = I2S_CLK_SRC_APLL;
  }
#endif
  int result = i2s_channel_reconfig_std_clock(_tx_handle, &clk_cfg );
  if (_tx_running) { i2s_channel_enable(_tx_handle); }
  AddLog(LOG_LEVEL_DEBUG, "I2S: Updating clock config");
  return result;
}

/*********************************************************************************************\
 * microphone related functions
\*********************************************************************************************/

uint32_t TasmotaI2S::I2sMicInit(void) {
  if (!_rx_configured) { return 0; }   // nothing configured

  esp_err_t err = ESP_OK;
  gpio_num_t clk_gpio;

  i2s_slot_mode_t slot_mode = (_rx_channels == 1) ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO;
  AddLog(LOG_LEVEL_DEBUG, "I2S: mic init rx_channels:%i rx_running:%i rx_handle:%p", slot_mode, _rx_running, _rx_handle);

  // if (_tx_configured && _rx_running) {      // duplex mode, mic was already initialized
  //   AddLog(LOG_LEVEL_DEBUG, "I2S: mic init exit Rx already enabled");
  //   return 0; // no need to en- or disable when in full duplex mode and already initialized
  // }

  // if (_rx_handle == nullptr){
  //   i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);

  //   AddLog(LOG_LEVEL_DEBUG, "I2S: rx_chan_cfg id:%i role:%i dma_desc_num:%i dma_frame_num:%i auto_clear:%i",
  //         rx_chan_cfg.id, rx_chan_cfg.role, rx_chan_cfg.dma_desc_num, rx_chan_cfg.dma_frame_num, rx_chan_cfg.auto_clear);

  //   err = i2s_new_channel(&rx_chan_cfg, NULL, &_rx_handle);
  //   AddLog(LOG_LEVEL_DEBUG, "I2S: mic init i2s_new_channel err=%i", err);
  //   switch (_rx_mode){
  //     case I2S_MODE_PDM:
  //       {
  //         i2s_pdm_rx_config_t rx_pdm_cfg = {
  //           .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(_rx_freq),
  //           /* The default mono slot is the left slot (whose 'select pin' of the PDM microphone is pulled down) */
  //           .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
  //           // .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, slot_mode),
  //           .gpio_cfg = {
  //             .clk = _gpio_ws,
  //             .din = _gpio_din,
  //             .invert_flags = {
  //               .clk_inv = _gpio_ws_inv,
  //             },
  //           },
  //         };
  //         if (_rx_slot_mask != I2S_SLOT_NOCHANGE) { rx_pdm_cfg.slot_cfg.slot_mask = (i2s_pdm_slot_mask_t)_rx_slot_mask; }

  //         // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg clk_cfg sample_rate_hz:%i clk_src:%i mclk_multiple:%i dn_sample_mode:%i",
  //         //       rx_pdm_cfg.clk_cfg.sample_rate_hz, rx_pdm_cfg.clk_cfg.clk_src, rx_pdm_cfg.clk_cfg.mclk_multiple, rx_pdm_cfg.clk_cfg.dn_sample_mode);

  //         // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg slot_cfg data_bit_width:%i slot_bit_width:%i slot_mode:%i slot_mask:%i",
  //         //       rx_pdm_cfg.slot_cfg.data_bit_width, rx_pdm_cfg.slot_cfg.slot_bit_width, rx_pdm_cfg.slot_cfg.slot_mode, rx_pdm_cfg.slot_cfg.slot_mask);

  //         // AddLog(LOG_LEVEL_DEBUG, "I2S: rx_pdm_cfg gpio_cfg clk:%i din:%i clk_inv:%i",
  //         //       rx_pdm_cfg.gpio_cfg.clk, rx_pdm_cfg.gpio_cfg.din, rx_pdm_cfg.gpio_cfg.invert_flags.clk_inv);

  //         err = i2s_channel_init_pdm_rx_mode(_rx_handle, &rx_pdm_cfg);
  //         AddLog(LOG_LEVEL_DEBUG, PSTR("I2S: RX channel in PDM mode, CLK: %i, DIN: %i, 16 bit width, %i channel(s), err code: 0x%04X"),
  //                 _gpio_ws, _gpio_din, slot_mode, err);
  //       }
  //       break;
  //     case I2S_MODE_STD:
  //       {        
  //         i2s_std_config_t rx_std_cfg = {
  //           .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(_rx_freq),
  //           .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, slot_mode),
  //           .gpio_cfg = {
  //             .mclk = (gpio_num_t)Pin(GPIO_I2S_MCLK),
  //             .bclk = (gpio_num_t)Pin(GPIO_I2S_BCLK),
  //             .ws   = (gpio_num_t)Pin(GPIO_I2S_WS),
  //             .dout = I2S_GPIO_UNUSED,
  //             .din  = (gpio_num_t)Pin(GPIO_I2S_DIN),
  //             .invert_flags = {
  //               .mclk_inv = false,
  //               .bclk_inv = false,
  //               .ws_inv   = false,
  //             },
  //           },
  //         };
  //         if (_rx_slot_mask != I2S_SLOT_NOCHANGE) { rx_std_cfg.slot_cfg.slot_mask = (i2s_std_slot_mask_t)_rx_slot_mask; }

  //         err = i2s_channel_init_std_mode(audio_i2s.rx_handle, &rx_std_cfg);
  //         AddLog(LOG_LEVEL_DEBUG, "I2S: RX i2s_channel_init_std_mode err:%i", err);
  //         AddLog(LOG_LEVEL_DEBUG, "I2S: RX channel in standard mode with 16 bit width on %i channel(s) initialized", slot_mode);
  //       }
  //       break;
  //     default:
  //       AddLog(LOG_LEVEL_INFO, "I2S: invalid rx mode=%i", _rx_mode);
  //       return -1;
  //   }
  // }

  if (!_rx_running) {
    err = i2s_channel_enable(_rx_handle);
    AddLog(LOG_LEVEL_DEBUG, PSTR("I2S: RX channel enable err:0x%04X"),err);
    _rx_running = true;
  }
  return err;
}


void TasmotaI2S::I2sMicDeinit(void) {
  esp_err_t err = ESP_OK;
  gpio_num_t clk_gpio;

  AddLog(LOG_LEVEL_DEBUG, "I2S: mic deinit rx_running:%i rx_handle:%p", _rx_running, _rx_handle);
  if (!_rx_handle) { return; }

  if (!_tx_configured || !_rx_configured) {      // if duplex mode, there is no mic channel - TODO check this
    int err = i2s_channel_disable(_rx_handle);
    i2s_del_channel(_rx_handle);
    _rx_handle = nullptr;
    _rx_running = false;
    AddLog(LOG_LEVEL_DEBUG, "I2S: RX channel disable: %i", err);
  }
}

#endif // USE_I2S_AUDIO
#endif // defined(ESP32) && ESP_IDF_VERSION_MAJOR >= 5
