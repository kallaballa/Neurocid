#ifndef SOUNDENCODER_HPP_
#define SOUNDENCODER_HPP_

#include "options.hpp"
#include <stdio.h>
#include <string>
#include <map>
#include <sndfile.hh>

namespace neurocid {
using std::map;
using std::string;

class SoundEncoder {
private:
  map<string, SndfileHandle> handles_;
  static SoundEncoder* instance_;
  SoundEncoder() {}
public:
  virtual ~SoundEncoder() {}
  void init(const string& name, const string& filename, int channels, int rate);
  void encode(const string& name, float* buffer, size_t len);
  bool has(const string& name);
  void close();

  static SoundEncoder* getInstance() {
    if(instance_ == NULL)
      instance_ = new SoundEncoder();

    return instance_;
  }

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }
};

} /* namespace neurocid */

#endif /* SOUNDENCODER_HPP_ */
