#pragma once
#include"stdafx.h"
#include"ffmpegcommon.h"
#include "commonutils\sync_object.h"
#include "commonutils\auto_lock.h"
class CPacketBuffer
{
public:
    CPacketBuffer();
    ~CPacketBuffer();
    int AddPacket(AVPacket *packet);
    int GetPacket(AVPacket *packet);
    void SetMaxCount(int count)
    {
        m_count = count;
    }
    void Buffer_Clear();
protected:
    AVPacketList *m_pfirst_pkt;
    AVPacketList *m_plast_pkt;
    int m_count;
    int m_nb_packets;
    int m_size;
    base::CCriticalSection	m_sect;
private:
};
//class CFrameBuffer
//{
//public:
//    CFrameBuffer();
//    ~CFrameBuffer();
//    int AddFrame(VideoPicture *packet);
//    int GetFrame(VideoPicture *packet);
//    void SetMaxCount(int count)
//    {
//        m_count = count;
//    }
//    void Buffer_Clear();
//protected:
//    VideoPicture *m_pfirst_pkt;
//    int m_count;
//    base::CCriticalSection	m_sect;
//private:
//};


template<typename BUFTYPE>
class BuffQuene
{
public:
    typedef void (*FREEFUN)(BUFTYPE );
private:
    list<BUFTYPE> m_buff;
    base::CCriticalSection	m_sect;
    FREEFUN m_freefun;
public:
    BuffQuene():
     m_freefun(NULL)
    {
        
    }
    
    void SetFreeFun(FREEFUN fun)
    {
        m_freefun = fun;
    }
    int AddData(BUFTYPE indata)
    {
        base::AutoLock lock(&m_sect);
        m_buff.push_back(indata);
        return 0;
    }
    bool GetData(BUFTYPE *outdata)
    {
        base::AutoLock lock(&m_sect);
        if (m_buff.size() == 0)
        {
            return false;
        }
        *outdata = m_buff.front();
        m_buff.pop_front();
        return true;
    }
    bool Clear()
    {
        base::AutoLock lock(&m_sect);
        if (m_freefun)
        {
            list<BUFTYPE>::iterator ite = m_buff.begin();
            while(ite != m_buff.end())
            {
                m_freefun(*ite);
            }
        }
        m_buff.clear();
        return true;
    }
};