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
    //��ȡδ�������ݵ��߳�
    static unsigned __stdcall ReadPacketThread(void* param);
    //�����̡߳�
    static unsigned __stdcall DecodeThread(void* param);

    //ͼ���ʽת����
    void PixFmtConvert(AVFrame **frame);
    static void FreeFrames(AVFrame *frame)
    {
        av_frame_free(&frame);
    }
private:
    //���ֽ�Ϊ��λ��һ֡�Ĵ�С ��
    int m_frame_size;
    //
    AVRational m_time_base;
    //��ǰʱ�� ��
    int64_t m_time_frame;
    //֡�ʡ�
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
    //�����ж������¸�����Ԥ���dts.
    //����һ�����а�������֡������¡�
    //�ڵ�ǰ������һ֡��Ԥ���dts.
    int64_t m_next_dts;
    //�����ж�ȡ�����µ�һ����pts;
    int64_t m_dts;

    //
    int64_t m_next_pts;
    //��ǰ�Ѿ������֡��pts
    int64_t m_pts;

    //�����������Ѷ�ȡ�İ��Ĵ�С ��
    uint64_t m_data_size;
    //�ɹ���������ж�ȡ�İ������� ��
    uint64_t m_nb_packets;
    //�ӽ������ж�ȡ����֡������ ��
    uint64_t m_frames_decoded;

    int m_width;
    int m_height;

    //��Ƶ����װ�á�
    CVideoInput *m_video_input;
    CVideoDesktopIN *m_desktop_input;

    //�ѽ���֡���档
    BuffQuene<AVFrame*> m_frambuf;
    //δ������Ļ��档
    CPacketBuffer m_pktbuf;
};