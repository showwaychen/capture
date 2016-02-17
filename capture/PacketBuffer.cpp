#include"stdafx.h"
#include "PacketBuffer.h"


void CPacketBuffer::Buffer_Clear()
{
    AVPacketList *pkt, *pkt1;
    base::AutoLock lock(&m_sect);
    for (pkt = m_pfirst_pkt; pkt != NULL; pkt = pkt1)
    {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    m_pfirst_pkt = NULL;
    m_plast_pkt = NULL;
    m_nb_packets = 0;
    m_size = 0;
}
CPacketBuffer::CPacketBuffer()
{
    m_pfirst_pkt = NULL;
    m_plast_pkt = NULL;
    m_count = 10;
    m_nb_packets = 0;
    m_size = 0;
}
CPacketBuffer::~CPacketBuffer()
{
    
}
int CPacketBuffer::AddPacket(AVPacket *packet)
{
    int lret = 0;
    if (packet == NULL || av_dup_packet(packet) < 0)
    {
        lret = -1;
    }
    else
    {
        base::AutoLock lock(&m_sect);
        if (m_nb_packets > m_count)
        {
            lret = -1;
            return lret;
        }
        AVPacketList *pkt_temp;
        pkt_temp = (AVPacketList*)av_malloc(sizeof(AVPacketList));
        if(NULL == pkt_temp)
        {
            lret = -2;
        }
        else
        {
            pkt_temp->pkt = *packet;
            pkt_temp->next = NULL;
            if (NULL == m_plast_pkt)
            {
                m_pfirst_pkt = pkt_temp;
            }
            else
            {
                m_plast_pkt->next = pkt_temp;
            }
            m_plast_pkt = pkt_temp;
            m_nb_packets++;
            m_size += pkt_temp->pkt.size + sizeof(*pkt_temp);
        }
    }
    return lret;
}
int CPacketBuffer::GetPacket(AVPacket *packet)
{
    AVPacketList *l_pkt;
    int lret = -1;
    base::AutoLock lock(&m_sect);
    l_pkt = m_pfirst_pkt;
    if (l_pkt)
    {
        m_pfirst_pkt = l_pkt->next;
        if (NULL == m_pfirst_pkt)
        {
            m_plast_pkt = NULL;
        }
        m_nb_packets--;
        m_size -= l_pkt->pkt.size + sizeof(*l_pkt);
        *packet = l_pkt->pkt;
        av_free(l_pkt);
        lret = 0;
    }

    return lret;
}

