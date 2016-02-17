#include"stdafx.h"
#include"videoconvertcontext.h"

CVideoConvertContext* CVideoConvertContext::s_inst = NULL;
CVideoConvertContext* CVideoConvertContext::Inst()
{
    if (s_inst == NULL)
    {
        s_inst = new CVideoConvertContext;
    }
    return s_inst;
}