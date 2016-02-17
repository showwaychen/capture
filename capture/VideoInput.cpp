#include "stdafx.h"
#include"VideoInput.h"

CVideoInput::CVideoInput():
m_hwnd(NULL),
m_buffer(NULL),
m_source_hdc(NULL),
m_dest_hdc(NULL)
{
    m_width = 0;
    m_height = 0;
    m_offset_x = 0;
    m_offset_y = 0;
    m_capture_curser = true;
}
CVideoInput::~CVideoInput()
{
    
}
int CVideoInput::OpenVideoIn()
{
    init();
    int lret = 0;
    m_hwnd = ::GetDesktopWindow();
    if (!::IsWindow(m_hwnd))
    {
        return -1;
    }
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
   /* if (m_width == 0 || m_height == 0)
    {*/
        m_clip_rect.left = rc.left;
        m_clip_rect.top = rc.top;
        m_clip_rect.right = rc.right;
        m_clip_rect.bottom = rc.bottom;
   // }
    m_width = m_clip_rect.right - m_clip_rect.left;
    m_height = m_clip_rect.bottom - m_clip_rect.top;
    if (m_width <= 0 || m_height <= 0)
    {
        return -1;
    }
    m_source_hdc = ::GetDC(m_hwnd);
    if (m_source_hdc)
    {
        int bpp = ::GetDeviceCaps(m_source_hdc, BITSPIXEL);
        if (bpp > 24)
        {
            bpp = 24;
        }

        m_dest_hdc = ::CreateCompatibleDC(m_source_hdc);
        if (m_dest_hdc)
        {
            m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            m_bmi.bmiHeader.biWidth = m_width;
            m_bmi.bmiHeader.biHeight = m_height;
            m_bmi.bmiHeader.biPlanes = 1;
            m_bmi.bmiHeader.biBitCount = bpp;
            m_bmi.bmiHeader.biCompression = BI_RGB;
            m_bmi.bmiHeader.biSizeImage = 0;
            m_bmi.bmiHeader.biXPelsPerMeter = 0;
            m_bmi.bmiHeader.biYPelsPerMeter = 0;
            m_bmi.bmiHeader.biClrUsed = 0;
            m_bmi.bmiHeader.biClrImportant = 0;
            
            HBITMAP hbmp   = NULL;
            void *buffer   = NULL;
            BITMAP bmp;
            hbmp = ::CreateDIBSection(m_dest_hdc, &m_bmi, 
                            DIB_RGB_COLORS, &m_buffer, NULL, 0);
            if (!hbmp)
            {
                lret = -1;
            }
            else
            {
                if (::SelectObject(m_dest_hdc, hbmp))
                {
                    ::GetObject(hbmp, sizeof(BITMAP), &bmp);

                    m_frame_size = bmp.bmWidthBytes * bmp.bmHeight * bmp.bmPlanes;
                    m_header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                                        (bpp <= 8 ? (1 << bpp): 0) * sizeof(RGBQUAD);
                }
            }
        }
    }
    else
    {
        lret = -2;
    }
    return lret;
}
int CVideoInput::init()
{
    int lret = 0;
    avdevice_register_all();
    return 0;
}
void CVideoInput::IsDrawCursor(bool draw)
{
    m_capture_curser = draw;
}
void CVideoInput::PaintMousePointer()
{
    CURSORINFO ci = {0};
    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci))
    {
        HCURSOR icon = CopyCursor(ci.hCursor);
        ICONINFO info;
        POINT pos;
        //RECT 
        HWND hwnd = m_hwnd;
        info.hbmMask = NULL;
        info.hbmColor = NULL;
        if (ci.flags != CURSOR_SHOWING)
        {
            return ;
        }

        if (!icon)
        {
            icon = CopyCursor(LoadCursor(NULL, IDC_ARROW));
        }

        if (!GetIconInfo(icon, &info))
        {
            return ;
        }
        pos.x = ci.ptScreenPos.x - info.xHotspot;
        pos.y = ci.ptScreenPos.y - info.yHotspot;
        if (hwnd)
        {
            RECT rect;
            if (GetWindowRect(hwnd, &rect))
            {
                pos.x -= rect.left;
                pos.y -= rect.top;
            }
            else
            {
                return ;
            }
        }

        if (pos.x >= 0 && pos.x <= m_width && pos.y >= 0 && pos.y <= m_height)
        {
            if (!::DrawIcon(m_dest_hdc, pos.x, pos.y, icon))
            {
                return ;
            }
        }

        if (info.hbmMask)
        {
            DeleteObject(info.hbmMask);
        }
        if (info.hbmColor)
        {
            DeleteObject(info.hbmColor);
        }
        if (icon)
        {
            DestroyCursor(icon);
        }
    }
}
int CVideoInput::GetPacket(AVPacket *pkt)
{
    int lret = 0;
    int64_t starttime = ::GetTickCount();
    if (!BitBlt(m_dest_hdc, 0, 0,
                m_clip_rect.right - m_clip_rect.left,
                m_clip_rect.bottom - m_clip_rect.top,
                m_source_hdc,
                m_clip_rect.left, m_clip_rect.top,
                SRCCOPY | CAPTUREBLT))
    {
        lret = -1;
    }
    if (m_capture_curser)
    {
        PaintMousePointer();
    }
    starttime = ::GetTickCount() - starttime;
    pkt->size =  m_frame_size;
    pkt->data = (uint8_t *)av_malloc(pkt->size);
    memcpy(pkt->data, m_buffer, m_frame_size);
    
    //memcpy(pkt->data, &bfh, sizeof(bfh));

    //memcpy(pkt->data + sizeof(bfh), &m_bmi.bmiHeader, sizeof(m_bmi.bmiHeader));

    /*if (m_bmi.bmiHeader.biBitCount <= 8)
    {
        GetDIBColorTable(m_dest_hdc, 0, 1 << m_bmi.bmiHeader.biBitCount)
    }*/
    
    return lret;
}

int CVideoInput::GetPictureFm()
{
    return AV_PIX_FMT_BGR24;
}
int CVideoInput::SaveBitMap2File()
{
    BITMAPFILEHEADER bfh;
    int filesize = m_header_size + m_frame_size;
    //"bm" in little-endian
    bfh.bfType = 0x4d42;
    bfh.bfSize = filesize;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = m_header_size;
    HANDLE fh;
    
    fh = ::CreateFile(_T("test.bmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (fh == INVALID_HANDLE_VALUE)
        return -1;
    DWORD dwWritten = 0;
    WriteFile(fh, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(fh, (LPSTR)&m_bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(fh, (LPSTR)m_buffer, m_frame_size, &dwWritten, NULL);
    CloseHandle(fh);
    return 0;
}
