#pragma  once
#include"ffmpegcommon.h"
class CVideoDesktopIN
{
    AVCodecContext *codectx;
    AVFormatContext *fomatctx;
    AVCodec *pcodec;
public:
    CVideoDesktopIN();
    ~CVideoDesktopIN();
    int init();
    int GetPacket(AVPacket *pkt);
    int GetPictureFm()
    {
        return AV_PIX_FMT_BGR24;
    }

    bool IsNeedDecode()
    {
        return true;
    }
};