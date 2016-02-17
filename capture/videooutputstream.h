#pragma once
#include"stdafx.h"
#include"ffmpegcommon.h"
#include "PacketBuffer.h"
class CVideoOutputStream
{
private:
    int m_encodetype;
    AVCodec *m_encoder;
    AVCodecContext *m_encodectx;
	AVStream *m_video_st;
	AVFormatContext *m_pFormatCtx;

    int file_index;
    //stream index in output file
    int m_index;
    //此输出流是否需要编码。
    int m_encoding_needed;
    int m_frame_number;

    //pts of the first frame encodedfor this 
    //stream, used for limiting recording time
    int64_t m_first_pts;
    /* dts of the last packet sent to the muxer */
    int64_t m_last_mux_dts;
    AVRational m_frame_rate;

    //
    CPacketBuffer m_encoded_buff;

    //
    bool m_isover;
protected:
    //获取编码数据的线程
    static unsigned __stdcall EncodeThread(void* param);
    //写包线程。
    static unsigned __stdcall WriteThread(void* param);
public:
	CVideoOutputStream();
	~CVideoOutputStream();
    void SetEncodeType(int type);
    int init(int encodetype);
    void EncodePacket();
    void WritePacket();
    int Flush_Encoder();
    void StartOutput();
    void CloseStream();
};