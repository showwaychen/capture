#pragma  once
#include"ffmpegcommon.h"
class CVideoInputStream;
class CVideoConvertContext
{
protected:
    int m_inputwidth;
    int m_inputheight;
    

    //输出相关参数。
    CVideoInputStream *m_inputstream;
    CVideoConvertContext():
    m_inputstream(NULL)
    {
        
    }
    static CVideoConvertContext* s_inst;
public:
    //输入相关参数。
    AVCodecContext *m_pinCodecCtx;
    AVCodec *m_pinCodec;
    AVFormatContext *m_pinFormatCtx;
    static CVideoConvertContext* Inst();
    int GetInputWidth()
	{
        if (m_pinCodecCtx)
        {
            return m_pinCodecCtx->width;
        }
        else
        {
            return 0;
        }
	}
    int GetInputHeight()
	{
        if (m_pinCodecCtx)
        {
            return m_pinCodecCtx->height;
        }
        else
        {
            return 0;
        }
	}

    CVideoInputStream *GetInputStream()
    {
        return m_inputstream;
    }
    void SetInputStream(CVideoInputStream *is)
    {
        m_inputstream = is;
    }
};