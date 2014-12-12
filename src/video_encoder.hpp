#ifndef _NO_VIDEOENC

#ifndef VIDEOENCODER_HPP_
#define VIDEOENCODER_HPP_


#include "options.hpp"
#include <stdio.h>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
}

struct SDL_Surface;

namespace neurocid {

class VideoEncoder {
private:
	AVCodec* codec_;
    AVCodecContext* context_;
    AVPacket* pkt_;
    int frameIndex_;
    int gotOutput_;
    FILE *file_;
    AVFrame *rgbFrame_;
    AVFrame *yuvFrame_;
    bool initialzed_;
    static VideoEncoder* instance_;
	VideoEncoder();
public:
	virtual ~VideoEncoder();
  void init(size_t width, size_t height, size_t fps, const char* filename, enum AVCodecID codec_id);
	void encode(SDL_Surface *surface);
	void close();

	static VideoEncoder* getInstance() {
		if(instance_ == NULL)
			instance_ = new VideoEncoder();

		return instance_;
	}

	static void init(const std::string& captureFile) {
#ifndef _NO_VIDEOENC
	  VideoEncoder& ve = *VideoEncoder::getInstance();
	  Options& opt = *Options::getInstance();

	  if (!captureFile.empty())
	    ve.init(opt.WINDOW_WIDTH, opt.WINDOW_HEIGHT, opt.FRAMERATE, captureFile.c_str(), AV_CODEC_ID_H264);
	}
#endif

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }
};

} /* namespace neurocid */

#endif /* VIDEOENCODER_HPP_ */

#endif
