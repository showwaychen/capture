#pragma  once
extern "C"
{
#ifdef HAVE_AV_CONFIG_H
#undef  HAVE_AV_CONFIG_H
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include<libswresample/swresample.h>
#include<libavdevice/avdevice.h>
#include <libavutil\error.h>

};