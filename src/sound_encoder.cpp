#include "sound_encoder.hpp"
#include "error.hpp"

namespace neurocid {

SoundEncoder* SoundEncoder::instance_ = NULL;

void SoundEncoder::init(const string& name, const string& filename, int channels, int rate) {
  CHECK(!has(name));
  handles_[name] = SndfileHandle(filename, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, channels, rate);
}
void SoundEncoder::encode(const string& name, float* buffer, size_t len) {
  if(!has(name))
    return;
  handles_[name].write(buffer, len);
}

bool SoundEncoder::has(const string& name) {
  return handles_.find(name) != handles_.end();
}

void SoundEncoder::close() {
  handles_.clear(); // destructors take care of resources
}

} /* namespace neurocid */

