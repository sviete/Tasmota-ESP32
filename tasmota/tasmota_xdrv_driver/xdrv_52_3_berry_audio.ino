/*
  xdrv_52_3_berry_audio.ino - Berry scripting language, support for I2S audio

  Copyright (C) 2021 Stephan Hadinger, Berry language by Guan Wenliang https://github.com/Skiars/berry

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


#ifdef USE_BERRY

#ifdef USE_I2S_AUDIO_BERRY

#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceFS.h"

#include <berry.h>

#if ESP_IDF_VERSION_MAJOR < 5
  #error "USE_I2S_AUDIO_BERRY is only supported for ESP-IDF 5.1 or later"
#endif

/*********************************************************************************************\
 * AudioOutput class
 * 
\*********************************************************************************************/
extern "C" {

  // AudioOutputI2S.init() -> instance 
  void* be_audio_output_i2s_init(void) {
    return audio_i2s.out;               // return the singleton of TasmotaI2S which is already initialized
  }

  // AudioOutputI2S.deinit()-> void
  void be_audio_output_i2s_deinit(TasmotaI2S * out) {
    out->stop();
  }

  // AudioOutputI2S.begin() -> bool
  int be_audio_output_i2s_begin(TasmotaI2S* out) {
    if (I2SPrepareTx()) { return false; }
    return out->begin();
  }

  // AudioOutputI2S.stop() -> bool
  int be_audio_output_i2s_stop(TasmotaI2S* out) {
    return out->stop();
  }

  // AudioOutputI2S.flush() -> bool
  void be_audio_output_i2s_flush(AudioOutput* out) {
    out->flush();
  }

  // AudioOutput.set_rate(rate_hz:int) -> bool
  int be_audio_output_set_rate(AudioOutput* out, int hz) {
    return out->SetRate(hz);
  }
  
  // AudioOutput.set_bits_per_sample(bits_per_sample:int) -> bool
  int be_audio_output_set_bits_per_sample(AudioOutput* out, int bps) {
    return out->SetBitsPerSample(bps);
  }

  // AudioOutput.set_channels(channels:int) -> bool
  int be_audio_output_set_channels(AudioOutput* out, int channels) {
    return out->SetChannels(channels);
  }

  // AudioOutput.set_gain(gain:real) -> bool
  int be_audio_output_set_gain(AudioOutput* out, float gain) {
    return out->SetGain(gain);
  }

  // AudioOutput.consume_mono(bytes) -> int
  int be_audio_output_consume_mono(AudioOutput* out, uint16_t *pcm, int bytes_len, int index) {
    int pcm_len = bytes_len / 2;
    int n;
    // berry_log_C("be_audio_output_consume_mono_ntv out=%p pcm=%p bytes_len=%i index=%i", out, pcm, bytes_len, index);
    for (n = 0; index + n < pcm_len; n++) {
      int16_t ms[2];
      ms[AudioOutput::LEFTCHANNEL] = ms[AudioOutput::RIGHTCHANNEL] = pcm[index + n];
      if (!out->ConsumeSample(ms)) { break; }
    }
    return n;
  }

  // AudioOutput.consume_stereo(bytes) -> int
  int be_audio_output_consume_stereo(AudioOutput* out, uint16_t *pcm, int bytes_len, int index) {
    int pcm_len = bytes_len / 4;  // 2 samples LEFT+RIGHT of 2 bytes each
    int n;
    // berry_log_C("be_audio_output_consume_stereo_ntv out=%p pcm=%p bytes_len=%i index=%i", out, pcm, bytes_len, index);
    for (n = 0; index + n < pcm_len; n++) {
      int16_t ms[2];
      ms[AudioOutput::LEFTCHANNEL] = pcm[index + n + n];
      ms[AudioOutput::RIGHTCHANNEL] = pcm[index + n + n + 1];
      if (!out->ConsumeSample(ms)) { break; }
    }
    return n;
  }

  // AudioOutput.consume_silence() -> int, push silence frames
  int be_audio_output_consume_silence(AudioOutput* out) {
    int n = 0;
    int16_t ms[2] = {0, 0};
    while (true) {
      if (!out->ConsumeSample(ms)) { break; }
      n++;
    }
    return n;
  }

  //
  // AudioOutputI2S()
  //
  int i2s_output_i2s_init(bvm *vm) {
    TasmotaI2S * audio = new TasmotaI2S();
    be_pushcomptr(vm, (void*) audio);
    be_setmember(vm, 1, ".p");
    be_return_nil(vm);
  }

  int i2s_output_i2s_deinit(bvm *vm) {
    int argc = be_top(vm);
    be_getmember(vm, 1, ".p");
    TasmotaI2S * audio = (TasmotaI2S *) be_tocomptr(vm, -1);
    if (audio) {
      delete audio;
      // clear
      be_pushcomptr(vm, (void*) NULL);
      be_setmember(vm, 1, ".p");
    }

    be_return_nil(vm);
  }

  //
  // AudioGeneratorWAV()
  //
  int i2s_generator_wav_init(bvm *vm) {
    AudioGeneratorWAV * wav = new AudioGeneratorWAV();
    be_pushcomptr(vm, (void*) wav);
    be_setmember(vm, 1, ".p");
    be_return_nil(vm);
  }

  AudioGeneratorWAV * i2s_generator_wav_get(bvm *vm) {
    be_getmember(vm, 1, ".p");
    AudioGeneratorWAV * wav = (AudioGeneratorWAV *) be_tocomptr(vm, -1);
    return wav;
  }

  int i2s_generator_wav_deinit(bvm *vm) {
    int argc = be_top(vm);
    AudioGeneratorWAV * wav = i2s_generator_wav_get(vm);
    if (wav) {
      delete wav;
      // clear
      be_pushcomptr(vm, (void*) NULL);
      be_setmember(vm, 1, ".p");
    }

    be_return_nil(vm);
  }

  int i2s_generator_wav_begin(bvm *vm) {
    int argc = be_top(vm);
    if (argc > 2) {
      if (I2SPrepareTx()) { be_raisef(vm, "internal_error", "I2SPrepareTx() failed"); be_return_nil(vm); } 
      AudioGeneratorWAV * wav = i2s_generator_wav_get(vm);
      be_getmember(vm, 2, ".p");
      AudioFileSource * source = (AudioFileSource*) be_tocomptr(vm, -1);
      be_getmember(vm, 3, ".p");
      AudioOutput * output = (AudioOutput*) be_tocomptr(vm, -1);
      be_pop(vm, 2);

      bool ret = wav->begin(source, output);
      be_pushbool(vm, ret);
      be_return(vm);
    }
    be_return_nil(vm);
  }

  int i2s_generator_wav_loop(bvm *vm) {
    AudioGeneratorWAV * wav = i2s_generator_wav_get(vm);
    bool ret = wav->loop();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  int i2s_generator_wav_stop(bvm *vm) {
    AudioGeneratorWAV * wav = i2s_generator_wav_get(vm);
    bool ret = wav->stop();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  int i2s_generator_wav_isrunning(bvm *vm) {
    AudioGeneratorWAV * wav = i2s_generator_wav_get(vm);
    bool ret = wav->isRunning();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  //
  // AudioGeneratorMP3()
  //
  int i2s_generator_mp3_init(bvm *vm) {
    AudioGeneratorMP3 * mp3 = new AudioGeneratorMP3();
    be_pushcomptr(vm, (void*) mp3);
    be_setmember(vm, 1, ".p");
    be_return_nil(vm);
  }

  AudioGeneratorMP3 * i2s_generator_mp3_get(bvm *vm) {
    be_getmember(vm, 1, ".p");
    AudioGeneratorMP3 * mp3 = (AudioGeneratorMP3 *) be_tocomptr(vm, -1);
    return mp3;
  }

  int i2s_generator_mp3_deinit(bvm *vm) {
    int argc = be_top(vm);
    AudioGeneratorMP3 * mp3 = i2s_generator_mp3_get(vm);
    if (mp3) {
      delete mp3;
      // clear
      be_pushcomptr(vm, (void*) NULL);
      be_setmember(vm, 1, ".p");
    }

    be_return_nil(vm);
  }

  int i2s_generator_mp3_begin(bvm *vm) {
    int argc = be_top(vm);
    if (argc > 2) {
      if (I2SPrepareTx()) { be_raisef(vm, "internal_error", "I2SPrepareTx() failed"); be_return_nil(vm); } 
      AudioGeneratorMP3 * mp3 = i2s_generator_mp3_get(vm);
      be_getmember(vm, 2, ".p");
      AudioFileSource * source = (AudioFileSource*) be_tocomptr(vm, -1);
      be_getmember(vm, 3, ".p");
      AudioOutput * output = (AudioOutput*) be_tocomptr(vm, -1);
      be_pop(vm, 2);

      bool ret = mp3->begin(source, output);
      be_pushbool(vm, ret);
      be_return(vm);
    }
    be_return_nil(vm);
  }

  int i2s_generator_mp3_loop(bvm *vm) {
    AudioGeneratorMP3 * mp3 = i2s_generator_mp3_get(vm);
    bool ret = mp3->loop();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  int i2s_generator_mp3_stop(bvm *vm) {
    AudioGeneratorMP3 * mp3 = i2s_generator_mp3_get(vm);
    bool ret = mp3->stop();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  int i2s_generator_mp3_isrunning(bvm *vm) {
    AudioGeneratorMP3 * mp3 = i2s_generator_mp3_get(vm);
    bool ret = mp3->isRunning();
    be_pushbool(vm, ret);
    be_return(vm);
  }

  // File Source FS
  //
#ifdef USE_UFILESYS
  int i2s_file_source_fs_init(bvm *vm) {
    int argc = be_top(vm);
    if (argc > 1 && be_isstring(vm, 2)) {
      const char * file_name = be_tostring(vm, 2);
      AudioFileSourceFS * file_source = new AudioFileSourceFS(*ufsp, file_name);
      be_pushcomptr(vm, (void*) file_source);
      be_setmember(vm, 1, ".p");
      be_return_nil(vm);
    }
    be_raise(vm, kTypeError, nullptr);
  }

  int i2s_file_source_fs_deinit(bvm *vm) {
    int argc = be_top(vm);
    be_getmember(vm, 1, ".p");
    AudioFileSourceFS * file_source = (AudioFileSourceFS *) be_tocomptr(vm, -1);
    if (file_source) {
      delete file_source;
      // clear
      be_pushcomptr(vm, (void*) NULL);
      be_setmember(vm, 1, ".p");
    }

    be_return_nil(vm);
  }
#endif // USE_UFILESYS


  // ----------------------------------------------------------------------
  // AudioInputI2S.init() -> instance 
  void* be_audio_input_i2s_init(void) {
    return audio_i2s.in;               // return the singleton of TasmotaI2S which is already initialized
  }

  // AudioInputI2S.deinit()-> void
  void be_audio_input_i2s_deinit(TasmotaI2S * in) {
    in->stop();
  }

  // AudioInputI2S.begin() -> bool
  int be_audio_input_i2s_begin(bvm *vm, TasmotaI2S* in) {
    if (I2SPrepareRx()) { be_raisef(vm, "internal_error", "I2SPrepareRx() failed"); be_return_nil(vm); } 
    in->I2sMicInit();
    return in->getRxRunning();
  }

  // AudioInputI2S.stop() -> bool
  int be_audio_input_i2s_stop(TasmotaI2S* in) {
    in->I2sMicDeinit();
    return true;
  }


  // AudioInputI2S.get_rate() -> int
  int be_audio_input_i2s_get_rate(TasmotaI2S* in) {
    return in->getRxRate();
  }

  // AudioInputI2S.get_bits_per_sample() -> int
  int be_audio_input_i2s_get_bits_per_sample(TasmotaI2S* in) {
    return in->getRxBitsPerSample();
  }

  // AudioInputI2S.get_channels() -> int
  int be_audio_input_i2s_get_channels(TasmotaI2S* in) {
    return in->getRxChannels();
  }

  // AudioInputI2S.get_gain() -> real
  float be_audio_input_i2s_get_gain(TasmotaI2S* in) {
    return in->getRxGain();
  }

  // AudioInputI2S.set_gain(gain:real) -> bool
  int be_audio_input_set_gain(TasmotaI2S* in, float gain) {
    return true;     // TODO
    // return in->SetGain(gain);
  }


  // AudioInputI2S.read_bytes() -> bytes()
  //
  // Reads bytes in the input buffer
  // Pre-condition: microphone but be recording (call begin() first)
  //
  // Returns `nil` if not configured or buffer empty
  // Returns `bytes()` instance with 16-bits audio
  int be_audio_input_i2s_read_bytes(bvm *vm) {
    int argc = be_top(vm);
    if (!audio_i2s.in->getRxRunning()) { be_return_nil(vm); }

    uint16_t buf_audio[256];

    size_t bytes_read = 0;
    esp_err_t err = i2s_channel_read(audio_i2s.in->getRxHandle(), buf_audio, sizeof(buf_audio), &bytes_read, 0);
    // AddLog(LOG_LEVEL_DEBUG, "BRY: be_audio_input_i2s_read_bytes - err=%i bytes=%i", err, bytes_read);
    if ((err != ESP_OK) && (err != ESP_ERR_TIMEOUT)) { be_return_nil(vm); }
    if (bytes_read == 0) { be_return_nil(vm); }

    // we received some data
    if (argc >= 2 && be_isbytes(vm, 2)) {
      // we have already a bytes() buffer
      be_pushvalue(vm, 2);    // push on top
      // resize to btr
      be_getmember(vm, -1, "resize");
      be_pushvalue(vm, -2);
      be_pushint(vm, bytes_read);
      be_call(vm, 2);
      be_pop(vm, 3);
    } else {
      be_pushbytes(vm, nullptr, bytes_read); // allocate a buffer of size btr filled with zeros
    }

    // get the address of the buffer
    be_getmember(vm, -1, "_buffer");
    be_pushvalue(vm, -2);
    be_call(vm, 1);
    uint8_t * buf = (uint8_t*) be_tocomptr(vm, -2);
    be_pop(vm, 2);

    // copy
    memmove(buf, buf_audio, bytes_read);

    be_return(vm);  /* return code */
  }

}

#endif  // USE_I2S_AUDIO_BERRY
#endif  // USE_BERRY
