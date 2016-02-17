#include"stdafx.h"
#include "VideoDesktopIN.h"
#include "videoconvertcontext.h"

CVideoDesktopIN::CVideoDesktopIN()
{
    
}
CVideoDesktopIN::~CVideoDesktopIN()
{
    
}
int CVideoDesktopIN::init()
{
    int lret = 0;
    av_register_all();
    avdevice_register_all();
    avformat_network_init();
    codectx = NULL;
    fomatctx = NULL;
    pcodec = NULL;
    AVInputFormat *ifomat = av_find_input_format("gdigrab");

    if (avformat_open_input(&fomatctx, "desktop", ifomat, NULL) != 0)
    {
        return -1;
    }

    if (avformat_find_stream_info(fomatctx, NULL) < 0)
    {
        return -2;
    }
    
    int videoindex = -1;  
    for (int i = 0; i<fomatctx->nb_streams; i++)  
        if (fomatctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)  
        {  
            videoindex = i;  
            break;  
        }  
    if (videoindex == -1)  
    {  
        printf("Couldn't find a video stream.（没有找到视频流）\n");  
        return -1;  
    }  
    if (avcodec_open2(fomatctx->streams[videoindex]->codec, avcodec_find_decoder(fomatctx->streams[videoindex]->codec->codec_id), NULL)<0)  
    {  
        printf("Could not open codec.（无法打开解码器）\n");  
        return -1;  
    }  
    CVideoConvertContext *covins = CVideoConvertContext::Inst();
    if (covins)
    {
        covins->m_pinCodec = pcodec;
        covins->m_pinCodecCtx = fomatctx->streams[videoindex]->codec;
        covins->m_pinFormatCtx = fomatctx;
    }
    return lret;
}
int CVideoDesktopIN::GetPacket(AVPacket *pkt)
{
    return av_read_frame(fomatctx, pkt);
}