#pragma  once
#include"ffmpegcommon.h"
#include "PacketBuffer.h"
#include "VideoDesktopIN.h"
class CVideoInput;
//struct WAVFrame
//{
//    WAVFrame(AVFrame *frame);
//    AVFrame *m_frame;
//    ~WAVFrame()
//    {
//        av_frame_free(&m_frame);
//    }
//};
class CVideoInputStream
{

public:
    CVideoInputStream();
    ~CVideoInputStream();
    int init(CVideoInput *input);
    int init(const string& filename);
    int init(CVideoDesktopIN *input);
    void SetVideoInput(CVideoInput *input)
    {
        m_video_input = input;
    }

    int GetPixFmt()
    {
        return AV_PIX_FMT_YUV420P;
    }
    int GetWidth()
    {
        return m_width;
    }
    int GetHeight()
    {
        return m_height;
    }

    int GetFrame(AVFrame *pkt);

    int StartStream();
protected:
    void DecodePacket();
    void ReadPacket();
    //获取未解码数据的线程
    static unsigned __stdcall ReadPacketThread(void* param);
    //解码线程。
    static unsigned __stdcall DecodeThread(void* param);

    //图像格式转换。
    void PixFmtConvert(AVFrame **frame);
    static void FreeFrames(AVFrame *frame)
    {
        av_frame_free(&frame);
    }
private:
    //以字节为单位的一帧的大小 。
    int m_frame_size;
    //
    AVRational m_time_base;
    //当前时间 。
    int64_t m_time_frame;
    //帧率。
    AVRational m_framerate;

    //
    int m_decoding_needed;

    AVCodecContext *m_dec_ctx;
    AVCodec *m_dec;
    AVFormatContext *m_pFormatCtx;
    AVDictionary *m_decoder_opts;
    AVFrame *m_decoded_frame;

    //start time;
    int64_t m_start_time;
    //从流中读出的下个包的预测的dts.
    //，在一个包中包含几个帧的情况下。
    //在当前包中下一帧的预测的dts.
    int64_t m_next_dts;
    //从流中读取的最新的一包的pts;
    int64_t m_dts;

    //
    int64_t m_next_pts;
    //当前已经解码的帧的pts
    int64_t m_pts;

    //包含了所有已读取的包的大小 。
    uint64_t m_data_size;
    //成功从这个流中读取的包的数量 。
    uint64_t m_nb_packets;
    //从解码器中读取到的帧的数量 。
    uint64_t m_frames_decoded;

    int m_width;
    int m_height;

    //视频输入装置。
    CVideoInput *m_video_input;
    CVideoDesktopIN *m_desktop_input;

    //已解码帧缓存。
    BuffQuene<AVFrame*> m_frambuf;
    //未解码包的缓存。
    CPacketBuffer m_pktbuf;
};