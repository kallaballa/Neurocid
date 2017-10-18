#include "sound_encoder.hpp"
#include "error.hpp"

namespace neurocid {

SoundEncoder* SoundEncoder::instance_ = NULL;

void SoundEncoder::init(const string& name, const string& filename, int channels, int rate) {
#ifndef _NO_SOUNDENC
  CHECK(!has(name));
  handles_[name] = SndfileHandle(filename, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, channels, rate);
#endif
}
void SoundEncoder::encode(const string& name, float* buffer, size_t len) {
#ifndef _NO_SOUNDENC
  if(!has(name))
    return;
  handles_[name].write(buffer, len);
#endif
}

bool SoundEncoder::has(const string& name) {
#ifndef _NO_SOUNDENC
  return handles_.find(name) != handles_.end();
#endif
}

void SoundEncoder::close() {
#ifndef _NO_SOUNDENC
  handles_.clear(); // destructors take care of resources
#endif
}

} /* namespace neurocid */

