#include "stdafx.h"
#include"videoinputstream.h"
#include "VideoInput.h"
#include "videoconvertcontext.h"


CVideoInputStream::CVideoInputStream()
{
    m_frambuf.SetFreeFun(FreeFrames);
    m_pktbuf.SetMaxCount(20);
}
CVideoInputStream::~CVideoInputStream()
{
    m_frambuf.Clear();
}

int CVideoInputStream::StartStream()
{
    CVideoConvertContext::Inst()->SetInputStream(this);
    DWORD dwThreadid = 0;
    _beginthreadex(NULL, 0, DecodeThread, (void*)this, 0, (unsigned *)&dwThreadid);
    //开启读包线程。
    DWORD m_dwThreadID = 0;
    _beginthreadex(NULL, 0, ReadPacketThread, (void*)this, 0, (unsigned *)&m_dwThreadID);
    return 0;
}
int CVideoInputStream::init(CVideoInput *input)
{
    m_width = 0;
    m_height = 0;
    int lret = 0;
    m_video_input = input;
    if (m_video_input)
    {
        m_decoding_needed = m_video_input->IsNeedDecode();

        if (m_decoding_needed)
        {
            m_pFormatCtx = CVideoConvertContext::Inst()->m_pinFormatCtx;
            m_dec_ctx = CVideoConvertContext::Inst()->m_pinCodecCtx;
            ////开启解码线程。
            //DWORD dwThreadid = 0;
            //_beginthreadex(NULL, 0, DecodeThread, (void*)this, 0, (unsigned *)&dwThreadid);
        }

        
    }
    else
    {
        lret = -1;
    }
    return lret;
}

int CVideoInputStream::init(CVideoDesktopIN *input)
{
    m_width = 0;
    m_height = 0;
    int lret = 0;
    m_desktop_input = input;
    if (m_desktop_input)
    {
        m_decoding_needed = m_desktop_input->IsNeedDecode();

        if (m_decoding_needed)
        {
            m_dec_ctx = CVideoConvertContext::Inst()->m_pinCodecCtx;
            m_pFormatCtx = CVideoConvertContext::Inst()->m_pinFormatCtx;
            m_height = m_dec_ctx->height;
            m_width = m_dec_ctx->width;
            //开启解码线程。
            DWORD dwThreadid = 0;
            _beginthreadex(NULL, 0, DecodeThread, (void*)this, 0, (unsigned *)&dwThreadid);
        }

        //开启读包线程。
        DWORD m_dwThreadID = 0;
        _beginthreadex(NULL, 0, ReadPacketThread, (void*)this, 0, (unsigned *)&m_dwThreadID);
    }
    else
    {
        lret = -1;
    }
    return lret;
}
int CVideoInputStream::init(const string& filename)
{
    return 0;
}

void CVideoInputStream::DecodePacket()
{
    int ret = 0;
    while(1)
    {
        //从缓存中取一个帧
        AVPacket *pkt = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();
        while(m_pktbuf.GetPacket(pkt) < 0)
        {
            Sleep(5);
        }
        if (m_decoding_needed)
        {
            int got_pic = 0;
            ret = avcodec_decode_video2(m_dec_ctx, frame, &got_pic, pkt);
            if (got_pic)
            {
                PixFmtConvert(&frame);
                m_frambuf.AddData(frame);
            }
        }
        else
        {
            
        }
    }
}

unsigned __stdcall CVideoInputStream::ReadPacketThread(void* param)
{
    CVideoInputStream *ins = (CVideoInputStream*)param;
    if (ins)
    {
        ins->ReadPacket();
    }
    return 0;
}
//解码线程。
unsigned __stdcall CVideoInputStream::DecodeThread(void* param)
{
    CVideoInputStream *ins = (CVideoInputStream*)param;
    if (ins)
    {
        ins->DecodePacket();
    }
    return 0;
}
void CVideoInputStream::ReadPacket()
{
    while(1)
    {
        AVPacket *pkt = av_packet_alloc();
        int getret = 0;
        do 
        {
            getret = m_desktop_input->GetPacket(pkt);
            if (getret > 0)
            {
                Sleep(5);
            }
        } while (getret > 0);
        if (getret < 0)
        {
            av_free_packet(pkt);
            break;
        }
        //如果不需要解码则直接将包转成AVFrame 放入队列中。
        if (m_decoding_needed)
        {
            //放入包缓存中。
            m_pktbuf.AddPacket(pkt);
        }
        else
        {
            AVFrame *frame = av_frame_alloc();
            uint8_t *buff = (uint8_t*)(av_malloc(pkt->size));
            memcpy(buff, pkt->data, pkt->size);
            frame->data[0] = buff;
            frame->extended_data[0] = buff;
            frame->linesize[0] = pkt->size / m_height;
            m_frambuf.AddData(frame);
        }
        //
    }
}
int CVideoInputStream::GetFrame(AVFrame *pkt)
{
    int lret = 0;
    AVFrame *temp = NULL;
    if (!m_frambuf.GetData(&temp))
    {
        lret = 1;
    }
    else
    {
        *pkt = *temp;
    }
    return lret;
}
void CVideoInputStream::PixFmtConvert(AVFrame **frame)
{
    //将图像转换成yuv420的格式。
    SwsContext *img_convert_ctx;
    AVPixelFormat format = AV_PIX_FMT_YUV420P;
    int bufsize = avpicture_get_size(format, m_width, m_height);
    uint8_t *out_buff = (uint8_t*)(av_malloc(bufsize));
    AVFrame *yuvframe = av_frame_alloc();
    
    avpicture_fill((AVPicture *)yuvframe, out_buff,
                    format, m_width, m_height);
    img_convert_ctx = sws_getContext(m_width, m_height,
        (AVPixelFormat)(m_dec_ctx->pix_fmt),
        m_width, m_height, format, SWS_BICUBIC,
        NULL, NULL, NULL);
    int retcode = sws_scale(img_convert_ctx, (const uint8_t * const*)(*frame)->data,
        (*frame)->linesize, 0, m_height, yuvframe->data,
        yuvframe->linesize);
    if (retcode < 0)
    {
        char errdes[256];
        av_strerror(retcode, errdes, 256);
    }
    av_frame_free(frame);
    *frame = yuvframe;
    (*frame)->extended_data = (*frame)->data;
}