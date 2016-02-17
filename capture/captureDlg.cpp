
// captureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "capture.h"
#include "captureDlg.h"
#include "VideoInput.h"
#include "videoinputstream.h"
#include "VideoDesktopIN.h"
#include "videooutputstream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CcaptureDlg �Ի���




CcaptureDlg::CcaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CcaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcaptureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &CcaptureDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CcaptureDlg ��Ϣ�������

BOOL CcaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CcaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CcaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CcaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CcaptureDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    //OnOK();
    m_videoinput = new CVideoDesktopIN;
    if (m_videoinput->init() != 0)
    {
        ::MessageBox(NULL, "��ʧ��", "kk", 0);
    }
    else
    {
        //CVideoInputStream *inputstream = new CVideoInputStream;
        m_inputstream.init(m_videoinput);
        //CVideoOutputStream *outputstream = new CVideoOutputStream;
        if (m_outputstream.init(AV_CODEC_ID_H264) != 0)
        {
            ::MessageBox(NULL, "��ʧ��", "output", 0);
        }
        else
        {
            m_inputstream.StartStream();
            m_outputstream.StartOutput();
        }
        //outputstream
        /*AVPacket *pkt = av_packet_alloc();
        videoinput->GetPacket(pkt);
        videoinput->GetPacket(pkt);
        av_free_packet(pkt);*/
    }
    //CVideoInput *videoinput = new CVideoInput;
    //if (videoinput->OpenVideoIn()!= 0)
    //{
    //    ::MessageBox(NULL, "��ʧ��", "kk", 0);
    //}
    //else
    //{
    //    videoinput->IsDrawCursor(true);
    //    /*AVPacket *pkt = av_packet_alloc();
    //    AVFrame *frame = av_frame_alloc();
    //    CVideoInputStream inputstream;
    //    inputstream.init(videoinput);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);
    //    inputstream.GetFrame(frame);*/
    //    //videoinput.GetPacket(pkt);
    //    //av_frame_free(&frame);
    //    //av_packet_free(&pkt);
    //}
}
