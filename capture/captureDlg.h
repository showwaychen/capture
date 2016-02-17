
// captureDlg.h : ͷ�ļ�
//

#pragma once
#include "VideoDesktopIN.h"
#include "videoinputstream.h"
#include "videooutputstream.h"


// CcaptureDlg �Ի���
class CcaptureDlg : public CDialog
{
// ����
public:
	CcaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

    CVideoDesktopIN *m_videoinput;
    CVideoInputStream m_inputstream;
    CVideoOutputStream m_outputstream;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
};
