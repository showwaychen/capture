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
    //窗口和屏幕的剪切区域 。
    RECT m_clip_rect;
    BITMAPINFO m_bmi;
    //包含位图数据的缓存。
    void *m_buffer;
    //是否抓取鼠标箭头。
    bool m_capture_curser;
    //位图信息头大小 。
    int m_header_size;
    //
    int m_frame_size;
public:
    CVideoInput();
    ~CVideoInput();
public:
    void IsDrawCursor(bool draw);
    //获取图像的格式。
    int GetPictureFm();
    int init();
    int OpenVideoIn();
    //0表示取包成功，负数表示取包错误
    //正数表示未取到包。
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