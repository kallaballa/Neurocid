#ifndef _NO_VIDEOENC

#include "video_encoder.hpp"
#include "error.hpp"
#include <cmath>
#include <iostream>

#include <SDL/SDL.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

namespace neurocid {

VideoEncoder* VideoEncoder::instance_ = NULL;
VideoEncoder::VideoEncoder() :
codec_(NULL),
context_(NULL),
pkt_(new AVPacket()),
frameIndex_(0),
gotOutput_(0),
file_(NULL),
rgbFrame_(NULL),
yuvFrame_(NULL),
initialzed_(false) {
	av_log_set_level(AV_LOG_ERROR);
    /* register all the codecs (you can also register only the codec
       you wish to have smaller code */
    avcodec_register_all();
}

VideoEncoder::~VideoEncoder() {
	delete pkt_;
}

void VideoEncoder::init(size_t width, size_t height, size_t fps, const char* filename, enum AVCodecID codec_id) {
	CHECK(codec_ == NULL);
	CHECK(context_ == NULL);
	CHECK(frameIndex_ == 0);
	CHECK(gotOutput_ == 0);
	CHECK(file_ == NULL);
	CHECK(yuvFrame_ == NULL);

    /* find the mpeg1 video encoder */
    codec_ = avcodec_find_encoder(codec_id);
    if (!codec_) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    context_ = avcodec_alloc_context3(codec_);
    if (!context_) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* put sample parameters */
    context_->bit_rate = 800000;
    /* resolution must be a multiple of two */
    context_->width = width;
    context_->height = height;
    /* frames per second */
    context_->time_base = AVRational{1,fps};
    context_->gop_size = 10; /* emit one intra frame every ten frames */
    context_->max_b_frames = 1;
    context_->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264) {
    	av_opt_set(context_->priv_data, "preset", "slow", 0);
    	av_opt_set(context_->priv_data, "tune", "animation", 0);
    	av_opt_set(context_->priv_data, "qp", "0", 0);
    }

    /* open it */
    if (avcodec_open2(context_, codec_, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    file_ = fopen(filename, "wb");
    if (!file_) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    yuvFrame_ = av_frame_alloc();
    if (!yuvFrame_) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    yuvFrame_->format = context_->pix_fmt;
    yuvFrame_->width  = context_->width;
    yuvFrame_->height = context_->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(yuvFrame_->data, yuvFrame_->linesize, context_->width, context_->height,
                         context_->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }


    rgbFrame_ = av_frame_alloc();
    if (!rgbFrame_) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    rgbFrame_->format = AV_PIX_FMT_RGBA;
    rgbFrame_->width  = context_->width;
    rgbFrame_->height = context_->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(rgbFrame_->data, rgbFrame_->linesize, context_->width, context_->height,
    		AV_PIX_FMT_RGBA, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

    av_init_packet(pkt_);
    pkt_->data = NULL;    // packet data will be allocated by the encoder
    pkt_->size = 0;
	fflush(stdout);
	initialzed_ = true;
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void VideoEncoder::encode(SDL_Surface *surface) {
	if(!initialzed_)
		return;

    SDL_PixelFormat* fmt = surface->format;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    size_t cnt = 0;
	for (int y = 0; y < context_->height; y++) {
		for (int x = 0; x < context_->width; x++) {
			Uint32 p = getpixel(surface,x,y);
		    SDL_GetRGB(p, fmt, &r, &g, &b);
		    rgbFrame_->data[0][(y * rgbFrame_->linesize[0] + (x * 3))] = r;
		    rgbFrame_->data[0][(y * rgbFrame_->linesize[0] + (x * 3)) + 1] = g;
		    rgbFrame_->data[0][(y * rgbFrame_->linesize[0] + (x * 3)) + 2] = b;

		    ++cnt;
		}
	}

    struct SwsContext* convertCtx = sws_getContext(
    		context_->width, context_->height, PIX_FMT_RGB24,
    		context_->width, context_->height, PIX_FMT_YUV420P,
    		SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(convertCtx, rgbFrame_->data, rgbFrame_->linesize, 0, rgbFrame_->height, yuvFrame_->data, yuvFrame_->linesize);
    sws_freeContext(convertCtx);

	int ret;
	yuvFrame_->pts = frameIndex_;

	/* encode the image */
	ret = avcodec_encode_video2(context_, pkt_, yuvFrame_, &gotOutput_);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		exit(1);
	}

	if (gotOutput_) {
		fwrite(pkt_->data, 1, pkt_->size, file_);
		av_free_packet(pkt_);
	}
	++frameIndex_;
}

void VideoEncoder::close() {
	if(!initialzed_)
		return;

    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    int ret;
    /* get the delayed frames */
    for (gotOutput_ = 1; gotOutput_; frameIndex_++) {
        fflush(stdout);

        ret = avcodec_encode_video2(context_, pkt_, NULL, &gotOutput_);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (gotOutput_) {
            fwrite(pkt_->data, 1, pkt_->size, file_);
            av_free_packet(pkt_);
        }
    }

    /* add sequence end code to have a real mpeg file */
    fwrite(endcode, 1, sizeof(endcode), file_);
    fclose(file_);

    avcodec_close(context_);
    av_free(context_);
    av_freep(&yuvFrame_->data[0]);
    av_freep(&rgbFrame_->data[0]);
    av_frame_free(&yuvFrame_);
    codec_ = NULL;
    context_= NULL;
    frameIndex_=0;
    gotOutput_=0;
    file_=NULL;
    yuvFrame_=NULL;
    rgbFrame_=NULL;
    initialzed_ = false;
}
} /* namespace neurocid */

#endif
