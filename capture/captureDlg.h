
// captureDlg.h : 头文件
//

#pragma once
#include "VideoDesktopIN.h"
#include "videoinputstream.h"
#include "videooutputstream.h"


// CcaptureDlg 对话框
class CcaptureDlg : public CDialog
{
// 构造
public:
	CcaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

    CVideoDesktopIN *m_videoinput;
    CVideoInputStream m_inputstream;
    CVideoOutputStream m_outputstream;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
};
