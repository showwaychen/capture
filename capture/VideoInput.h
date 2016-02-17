#pragma once
#include"ffmpegcommon.h"
class CVideoInput
{
private:
    int m_width;
    int m_height;
    int m_offset_x;
    int m_offset_y;
    HWND m_hwnd;
    HDC m_source_hdc;
    HDC m_dest_hdc;
    //���ں���Ļ�ļ������� ��
    RECT m_clip_rect;
    BITMAPINFO m_bmi;
    //����λͼ���ݵĻ��档
    void *m_buffer;
    //�Ƿ�ץȡ����ͷ��
    bool m_capture_curser;
    //λͼ��Ϣͷ��С ��
    int m_header_size;
    //
    int m_frame_size;
public:
    CVideoInput();
    ~CVideoInput();
public:
    void IsDrawCursor(bool draw);
    //��ȡͼ��ĸ�ʽ��
    int GetPictureFm();
    int init();
    int OpenVideoIn();
    //0��ʾȡ���ɹ���������ʾȡ������
    //������ʾδȡ������
    int GetPacket(AVPacket *pkt);
    int Close();
    bool IsNeedDecode()
    {
        return false;
    }
    int GetEncodeType()
    {
        return AV_CODEC_ID_NONE;
    }
protected:
    void PaintMousePointer();
    int SaveBitMap2File();
};