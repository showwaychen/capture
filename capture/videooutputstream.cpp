#include"stdafx.h"
#include"videooutputstream.h"
#include "videoconvertcontext.h"
#include "videoinputstream.h"


//获取编码数据的线程
unsigned __stdcall CVideoOutputStream::EncodeThread(void* param)
{
    CVideoOutputStream *ins = (CVideoOutputStream*)param;
    if (param)
    {
        ins->EncodePacket();
    }
    return 1;
}
//写包线程。
unsigned __stdcall CVideoOutputStream::WriteThread(void* param)
{
    CVideoOutputStream *ins = (CVideoOutputStream*)param;
    if (ins)
    {
        ins->WritePacket();
    }
    return 1;
}
int CVideoOutputStream::init(int encodetype)
{
    int lret = 0;
    int retcode = 0;
    const char* out_path = ".\\test.h264";	 
    m_encodetype = encodetype;
    m_encoder = avcodec_find_encoder((AVCodecID)encodetype);
	retcode = avformat_alloc_output_context2(&m_pFormatCtx, NULL, "h264", out_path);
    if (retcode < 0)
    {
        return -10;
    }
    if (m_encoder)
    {
        m_encodectx = avcodec_alloc_context3(m_encoder);
        m_encodectx->pix_fmt = AV_PIX_FMT_YUV420P;
        m_encodectx->width = CVideoConvertContext::Inst()->GetInputWidth();
        m_encodectx->height = CVideoConvertContext::Inst()->GetInputHeight();
		m_encodectx->time_base.num = 1;
		m_encodectx->time_base.den = 25;
		m_encodectx->bit_rate = 4000000;
		m_encodectx->gop_size = 15;
		m_encodectx->qmin = 10;
		m_encodectx->qmax = 51;
		AVDictionary *param = 0;
		//av_dict_set(&param, "preset", "fast", 0);
		//av_dict_set(&param, "tune", "zerolatency", 0);
        retcode = avcodec_open2(m_encodectx, m_encoder, NULL);
		if (retcode >= 0)
		{
			m_video_st = avformat_new_stream(m_pFormatCtx, m_encoder);
			if (m_video_st)
			{
				m_video_st->time_base.num = 1;
				m_video_st->time_base.den = 25;
				m_video_st->codec = m_encodectx;
                
                retcode = avio_open(&m_pFormatCtx->pb, out_path, AVIO_FLAG_READ_WRITE);
                char errdes[256] = {0};
                //AVERROR(retcode);
                av_strerror(retcode, errdes, 256);
                //av_err2str(AVERROR(retcode));
                ////sprintf(errdes, "%s", av_err2str(AVERROR(retcode)));
                if (retcode >= 0)
                {
                   retcode = avformat_write_header(m_pFormatCtx, NULL);
                   if (retcode < 0)
                   {
                       return -11;
                   }
                }
                else
                {
                     lret = -5;
                }
			}
			else
			{
				lret = -4;
			}
		}
		else
		{
			lret = -2;
		}
    }
    else
    {
        lret = -1;
    }
    return lret;
}

CVideoOutputStream::CVideoOutputStream()
{
	m_encoded_buff.SetMaxCount(50);
    m_isover = false;
    m_encoding_needed = true;
}
CVideoOutputStream::~CVideoOutputStream()
{
	
}
int CVideoOutputStream::Flush_Encoder()
{
    int lret = 0;
    return lret;
}
void CVideoOutputStream::EncodePacket()
{
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int got_pkt = 0;

    CVideoInputStream *inputstream = CVideoConvertContext::Inst()->GetInputStream();
    if (inputstream)
    {
        while(1)
        {
            //从输入流中取一包。
            while(inputstream->GetFrame(frame) != 0)
            {
                Sleep(5);
            }
            if (m_encoding_needed)
            {

                int enret = avcodec_encode_video2(m_encodectx,
                    pkt, frame, &got_pkt);
                if (enret < 0)
                {
                    char errdes[256] = {0};
                    //AVERROR(retcode);
                    av_strerror(AVERROR(enret), errdes, 256);
                    
                    //break;
                }
                if(got_pkt == 1)
                {
                    int wlret = av_write_frame(m_pFormatCtx, pkt);
                    if (m_isover)
                    {
                        break;
                    } 
                    if (wlret < 0)
                    {
                        char errdes[256] = {0};
                        //AVERROR(retcode);
                        av_strerror(AVERROR(enret), errdes, 256);
                    }
                    //m_encoded_buff.AddPacket(pkt);
                }
                av_frame_free(&frame);
                frame = av_frame_alloc();
            }
            else
            {
                m_encoded_buff.AddPacket(pkt);
            }
           
        }
       
    }
}
void CVideoOutputStream::WritePacket()
{
    //从已解码的包中取出一个包，并写入。
    
    while(1)
    {
        AVPacket *pkt = av_packet_alloc();
        while(m_encoded_buff.GetPacket(pkt) == -1)
        {
            Sleep(5);
        }
        //取完包后写数据。
        int wlret = 0;//av_write_frame(m_pFormatCtx, pkt);
        av_free_packet(pkt);
        if (wlret < 0 || m_isover)
        {
            break;
        }
    }
}
void CVideoOutputStream::StartOutput()
{
    DWORD dwThreadid = 0;
    _beginthreadex(NULL, 0, EncodeThread, (void*)this, 0, (unsigned *)&dwThreadid);
    //_beginthreadex(NULL, 0, WriteThread, (void*)this, 0, (unsigned *)&dwThreadid);
}
void CVideoOutputStream::CloseStream()
{
    //flush_encoder
    m_isover = true;
}