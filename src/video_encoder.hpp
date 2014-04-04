#ifndef _NO_VIDEOENC

#ifndef VIDEOENCODER_HPP_
#define VIDEOENCODER_HPP_

#include <stdio.h>
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
    void init(const char* filename, enum AVCodecID codec_id);
	void encode(SDL_Surface *surface);
	void close();

	static VideoEncoder* getInstance() {
		if(instance_ == NULL)
			instance_ = new VideoEncoder();

		return instance_;
	}
};

} /* namespace neurocid */

#endif /* VIDEOENCODER_HPP_ */

#endif
