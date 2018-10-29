
// ExperimentImgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ExperimentImg.h"
#include "ExperimentImgDlg.h"
#include "afxdialogex.h"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CExperimentImgDlg 对话框



CExperimentImgDlg::CExperimentImgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EXPERIMENTIMG_DIALOG, pParent)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//加载对话框的时候初始化
	m_pImgSrc = NULL;
	m_pImgCpy = NULL;
	m_nThreadNum = 1;
	m_pThreadParam = new ThreadParam[MAX_THREAD];
	srand(time(0));
}

void CExperimentImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE, mPictureControl);
	//DDX_Control(pDX, IDC_CHECK_100, m_CheckCirculation);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_EDIT1, m_editText);
	DDX_Control(pDX, IDC_PICTURE2, m_pic);
	//DDX_Control(pDX,IDC_COMBO1,)
	//DDX_Control(pDX, IDC_CHECK_CIRCULATION, m_ncheck);
	DDX_Control(pDX, IDC_CHECK1, m_checkuse);
}

BEGIN_MESSAGE_MAP(CExperimentImgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CExperimentImgDlg::OnBnClickedButtonOpen)
//	ON_EN_CHANGE(IDC_EDIT1, &CExperimentImgDlg::OnEnChangeEdit1)
//	ON_EN_CHANGE(IDC_EDIT_INFO, &CExperimentImgDlg::OnEnChangeEditInfo)
ON_CBN_SELCHANGE(IDC_COMBO_FUNCTION, &CExperimentImgDlg::OnCbnSelchangeComboFunction)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREADNUM, &CExperimentImgDlg::OnNMCustomdrawSliderThreadnum)
ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CExperimentImgDlg::OnBnClickedButtonProcess)
ON_MESSAGE(WM_NOISE, &CExperimentImgDlg::OnNoiseThreadMsgReceived)
ON_MESSAGE(WM_MEDIAN_FILTER, &CExperimentImgDlg::OnMedianFilterThreadMsgReceived)
ON_MESSAGE(WM_MESSAGE, &CExperimentImgDlg::OnMsgReceived)
ON_STN_CLICKED(IDC_PICTURE, &CExperimentImgDlg::OnStnClickedPicture)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CExperimentImgDlg::OnTcnSelchangeTab1)
ON_EN_CHANGE(IDC_EDIT1, &CExperimentImgDlg::OnEnChangeEdit1)
ON_NOTIFY(NM_THEMECHANGED, IDC_SCROLLBAR1, &CExperimentImgDlg::OnNMThemeChangedScrollbar1)
ON_CBN_SELCHANGE(IDC_COMBO_THREAD, &CExperimentImgDlg::OnCbnSelchangeComboThread)
ON_STN_CLICKED(IDC_PICTURE2, &CExperimentImgDlg::OnStnClickedPicture2)
END_MESSAGE_MAP()


// CExperimentImgDlg 消息处理程序

BOOL CExperimentImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
//	mEditInfo.SetWindowTextW(CString("File Path"));
	/*CComboBox * cmb_function = ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION));
	cmb_function->AddString(_T("椒盐噪声"));
	cmb_function->AddString(_T("中值滤波"));
	cmb_function->SetCurSel(0);*/

	CComboBox * cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	cmb_thread->InsertString(0, _T("WIN多线程"));
	cmb_thread->InsertString(1, _T("OpenMP"));
	cmb_thread->InsertString(2, _T("CUDA"));
	cmb_thread->SetCurSel(0);

	CString text1("正在等待");
	m_editText.SetWindowTextW(text1);
	m_tab.InsertItem(0, _T("椒盐噪声和滤波"));
	m_tab.InsertItem(1, _T("缩放和旋转"));
	m_tab.InsertItem(2, _T("傅里叶变换"));
	m_tab.InsertItem(3, _T("高斯噪声和滤波"));
	m_page1.Create(IDD_DIALOG1, &m_tab);
	m_page2.Create(IDD_DIALOG2, &m_tab);
	m_page3.Create(IDD_DIALOG3, &m_tab);
	m_page4.Create(IDD_DIALOG4, &m_tab);
	//设置显示
	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;	
	rc.right -= 0;
	m_page1.MoveWindow(&rc);
	m_page2.MoveWindow(&rc);
	m_page3.MoveWindow(&rc);
	m_page4.MoveWindow(&rc);

	pDialog[0] = &m_page1;
	pDialog[1] = &m_page2;
	pDialog[2] = &m_page3;
	pDialog[3] = &m_page4;

	pDialog[0]->ShowWindow(SW_SHOW);
	pDialog[1]->ShowWindow(SW_HIDE);
	pDialog[2]->ShowWindow(SW_HIDE);
	pDialog[3]->ShowWindow(SW_HIDE);
	m_CurSelTab = 0;

	CSliderCtrl * slider = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_THREADNUM));
	slider->SetRange(1, MAX_THREAD, TRUE);
	slider->SetPos(MAX_THREAD);

	AfxBeginThread((AFX_THREADPROC)&CExperimentImgDlg::Update, this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CExperimentImgDlg::paint(CStatic &pc, CImage *img)
{
	if (img != NULL)
	{
		int height;
		int width;
		CRect rect;
		CRect rect1;
		height = img->GetHeight();
		width = img->GetWidth();

		pc.GetClientRect(&rect);
		CDC *pDC = pc.GetDC();
		SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

		if (width <= rect.Width() && height <= rect.Width())
		{
			rect1 = CRect(rect.TopLeft(), CSize(width, height));
			img->StretchBlt(pDC->m_hDC, rect1, SRCCOPY);
		}
		else
		{
			float xScale = (float)rect.Width() / (float)width;
			float yScale = (float)rect.Height() / (float)height;
			float ScaleIndex = (xScale <= yScale ? xScale : yScale);
			rect1 = CRect(rect.TopLeft(), CSize((int)width*ScaleIndex, (int)height*ScaleIndex));
			//picture control调整到图像缩放后的大小
			CWnd *pWnd;
			pWnd = GetDlgItem(IDC_PICTURE);
			pWnd->MoveWindow(CRect((int)rect.top, (int)rect.left, (int)width*ScaleIndex, (int)height*ScaleIndex));
			img->StretchBlt(pDC->m_hDC, rect1, SRCCOPY);
			/*pWnd = GetDlgItem(IDC_PICTURE2);
			pWnd->MoveWindow(CRect((int)rect.top, (int)rect.left, (int)width*ScaleIndex, (int)height*ScaleIndex));
			img->StretchBlt(pDC->m_hDC, rect1, SRCCOPY);*/
		}
		ReleaseDC(pDC);
	}
}

void CExperimentImgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CExperimentImgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		paint(mPictureControl, m_pImgSrc);
		//paint(m_pic, m_pImgSrc);
		//if (m_pImgCpy != NULL) {
		//	m_pImgCpy->Destroy();
		//	delete m_pImgCpy;
		//}
		//m_pImgCpy = new CImage;
		//ImageCopy(m_pImgSrc, m_pImgCpy);
		//this->Invalidate();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CExperimentImgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
UINT CExperimentImgDlg::Update(void* p)
{
	while (1) {
		Sleep(200);
		CExperimentImgDlg* dlg = (CExperimentImgDlg*)p;
		dlg->paint(dlg->m_pic, dlg->m_pImgCpy);
		dlg->paint(dlg->mPictureControl, dlg->m_pImgSrc);
	}
	return 0;
}

void CExperimentImgDlg::ThreadDraw(DrawPara *p)
{
	CRect rect;
	GetClientRect(&rect);
	CDC     memDC;             // 用于缓冲绘图的内存画笔  
	CBitmap memBitmap;         // 用于缓冲绘图的内存画布
	memDC.CreateCompatibleDC(p->pDC);  // 创建与原画笔兼容的画笔
	memBitmap.CreateCompatibleBitmap(p->pDC, p->width, p->height);  // 创建与原位图兼容的内存画布
	memDC.SelectObject(&memBitmap);      // 创建画笔与画布的关联
	memDC.FillSolidRect(rect, p->pDC->GetBkColor());
	p->pDC->SetStretchBltMode(HALFTONE);
	// 将pImgSrc的内容缩放画到内存画布中
	p->pImgSrc->StretchBlt(memDC.m_hDC, 0, 0, p->width, p->height);

	// 将已画好的画布复制到真正的缓冲区中
	p->pDC->BitBlt(p->oriX, p->oriY, p->width, p->height, &memDC, 0, 0, SRCCOPY);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CExperimentImgDlg::wienerFileter()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime =GetTickCount64();
	
	//m_pImgCpy = new CImage;
	m_pImgCpy = new CImage;
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());
	this->Invalidate();
	switch (thread)
	{
	case 0:
	{	int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i) {
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
		m_pThreadParam[i].src = m_pImgSrc;
		m_pThreadParam[i].dst = m_pImgCpy;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::wienerFilter, &m_pThreadParam[i]);
	}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].dst = m_pImgCpy;
			ImageProcess::wienerFilter(&m_pThreadParam[i]);
		}
	}
	break;
	default:
		break;
	}
}

void CExperimentImgDlg::ImageCopy(CImage* pImgSrc, CImage* pImgDrt)
{
	int MaxColors = pImgSrc->GetMaxColorTableEntries();
	RGBQUAD* ColorTab;
	ColorTab = new RGBQUAD[MaxColors];

	CDC *pDCsrc, *pDCdrc;
	if (!pImgDrt->IsNull())
	{
		pImgDrt->Destroy();
	}
	pImgDrt->Create(pImgSrc->GetWidth(), pImgSrc->GetHeight(), pImgSrc->GetBPP(), 0);

	if (pImgSrc->IsIndexed())
	{
		pImgSrc->GetColorTable(0, MaxColors, ColorTab);
		pImgDrt->SetColorTable(0, MaxColors, ColorTab);
	}

	pDCsrc = CDC::FromHandle(pImgSrc->GetDC());
	pDCdrc = CDC::FromHandle(pImgDrt->GetDC());
	pDCdrc->BitBlt(0, 0, pImgSrc->GetWidth(), pImgSrc->GetHeight(), pDCsrc, 0, 0, SRCCOPY);
	pImgSrc->ReleaseDC();
	pImgDrt->ReleaseDC();
	delete ColorTab;

}

void CExperimentImgDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("JPEG(*jpg)|*.jpg|*.bmp|*.png|TIFF(*.tif)|*.tif|All Files （*.*）|*.*||");
	CString filePath("");
	
	CFileDialog fileOpenDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDialog.DoModal() == IDOK)
	{
		VERIFY(filePath = fileOpenDialog.GetPathName());
		CString strFilePath(filePath);
//		mEditInfo.SetWindowTextW(strFilePath);	//在文本框中显示图像路径
		tempPath = strFilePath;
		if (m_pImgSrc != NULL)
		{
			m_pImgSrc->Destroy();
			delete m_pImgSrc;
		}
		m_pImgSrc = new CImage();
		m_pImgSrc->Load(strFilePath);
		str = strFilePath;
		m_editText.SetWindowTextW(str);
		this->Invalidate();

	}
}


void CExperimentImgDlg::OnCbnSelchangeComboFunction()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CExperimentImgDlg::OnNMCustomdrawSliderThreadnum(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_THREADNUM);
	CString text("");
	m_nThreadNum = slider->GetPos();
	text.Format(_T("%d"), m_nThreadNum);
	GetDlgItem(IDC_STATIC_THREADNUM)->SetWindowText(text);
	*pResult = 0;
}


void CExperimentImgDlg::OnBnClickedButtonProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CurSelTab = m_tab.GetCurSel();
	if (m_checkuse.GetCheck() == 1) {
		if (!m_pImgCpy->IsNull()) {
			m_pImgSrc = new CImage;
			m_pImgSrc = m_pImgCpy;
		}
	}
	if (m_CurSelTab == 0) {
		auto p1 = (CPage1*)(this->pDialog[0]);
		auto func1 = p1->m_nIndex.GetCurSel();
		switch (func1)
		{
		case 0:  //椒盐噪声
		{	
			CString tempstr;
			tempstr = "\r\n进行椒盐噪声处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			AddNoise();
			//paint(m_pic, m_pImgSrc);
		}
			break;
		case 1: //自适应中值滤波
		{
			CString tempstr;
			tempstr = "\r\n进行中值滤波处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			MedianFilter(); 
			//paint(m_pic, m_pImgSrc);
		}
			break;
		default:
			break;
		}
	}
	if (m_CurSelTab == 1) {
		auto p2 = (CPage2*)(this->pDialog[1]);
		auto func2 = p2->m_nIndexPAGE2.GetCurSel();
		//int func = cmb_function->GetCurSel();
	/*	if (m_CheckCirculation.GetCheck()==1) {
			m_pImgSrc = new CImage();
			m_pImgSrc->Load(tempPath);
		}*/
		switch (func2)
		{
		case 0:  //缩放
		{
			CString tempstr;
			tempstr = "\r\n进行缩放处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			zoom();
		}
			break;
		case 1: //旋转
		{
			CString tempstr;
			tempstr = "\r\n进行旋转处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			rorate();
		}
			break;
		default:
			break;
		}
	}
	if (m_CurSelTab == 2) {
		auto p3 = (CPage3*)(this->pDialog[2]);
		auto func3 = p3->m_ncombopage3.GetCurSel();
		switch (func3)
		{
		case 0:
		{
			CString tempstr;
			tempstr = "\r\n进行傅里叶变换处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			dft();
		}
			break;
		case 1:
		{
			CString tempstr;
			tempstr = "\r\n进行傅里叶变换处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			if (!m_pImgCpy->IsNull()) {
				m_pImgSrc = m_pImgCpy;
			}
			dft();
			
		}
			break;
		default:
			break;
		}
	}
	if (m_CurSelTab == 3) {
		auto p4 = (CPage4*)(this->pDialog[3]);
		auto func4 = p4->m_nComboG.GetCurSel();
		switch (func4)
		{
		case 0:  
		{
			CString tempstr;
			tempstr = "\r\n进行添加高斯噪声处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			addGaussianNoise();
			
		}
			break;
		case 1: 
		{
			CString tempstr;
			tempstr = "\r\n进行平滑线性滤波处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			
			meansFilter();
			
		}
			break;
		case 2:
		{
			CString tempstr;
			tempstr = "\r\n进行维纳滤波处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
			wienerFileter();
			
		}
			break;
		case 3: 
		{
			CString tempstr;
			tempstr = "\r\n进行高斯滤波处理：";
			str += tempstr;
			m_editText.SetWindowTextW(str);
		
			GaussianFilter();
			
		}
			break;
		default:
			break;
		}
	}
}


void CExperimentImgDlg::AddNoise()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	//CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	startTime = GetTickCount64();
	//m_pImgCpy = new CImage;
	//m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());
	switch (thread)
	{
	case 0://win多线程
	{
		
		AddNoise_WIN();
	}

	break;

	case 1://openmp
	{
		int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

		#pragma omp parallel for num_threads(m_nThreadNum)
			for (int i = 0; i < m_nThreadNum; ++i)
			{
				m_pThreadParam[i].startIndex = i * subLength;
				m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
					(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
				m_pThreadParam[i].src = m_pImgSrc;
				ImageProcess::addNoise(&m_pThreadParam[i]);
			}
	}

	break;

	case 2://cuda
		break;
	}
}

void CExperimentImgDlg::AddNoise_WIN()
{
	int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
		m_pThreadParam[i].src = m_pImgSrc;
		//m_pThreadParam[i].dst = m_pImgCpy;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::addNoise, &m_pThreadParam[i]);
	}
}

void CExperimentImgDlg::zoom()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = GetTickCount64();
	auto p = (CPage2*)(this->pDialog[1]);
	CString scalestr;
	p->m_nZoom.GetWindowTextW(scalestr);
	double scale = _ttof(scalestr);

	CRect rect;
	GetClientRect(&rect);
	m_pImgCpy = new CImage;
	int origin_bpp = m_pImgSrc->GetBPP();
	int new_width = ceil(m_pImgSrc->GetWidth() * scale);
	int new_height = ceil(m_pImgSrc->GetHeight() * scale);
	if (new_width > rect.Width()) new_width = rect.Width();
	if (new_height > rect.Height()) new_height = rect.Height();
	if (!m_pImgCpy->IsNull()) {
		m_pImgCpy->Destroy();
	}
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(new_width,new_height,origin_bpp);

	//m_pImgSrc = shit;
	this->Invalidate();
	switch (thread)
	{
	case 0 :
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1; 
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].nzoom = scale;
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::zoom_WIN, &m_pThreadParam[i]);
		}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].nzoom = scale;
			ImageProcess::zoom_WIN(&m_pThreadParam[i]);
		}
	}
	default:
		break;
	}
	//delete shit;
	//delete tempsrc;
}

void CExperimentImgDlg::rorate()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	//CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	//int circulation = clb_circulation->GetCheck() == 0 ? 1 : 100;
	startTime = GetTickCount64();
	auto p = (CPage2*)(this->pDialog[1]);
	CString scalestr;
	p->m_nZoom.GetWindowTextW(scalestr);
	double angle = _ttof(scalestr);
	CRect rect;
	GetClientRect(&rect);

	m_pImgCpy = new CImage;
	auto length = sqrt(pow(m_pImgSrc->GetWidth(), 2) + pow(m_pImgSrc->GetHeight(), 2));
	auto A = angle * PI / 180.0;
	auto value1 = sqrt(pow(cos(A), 2));
	auto value2 = sqrt(pow(sin(A), 2));
	auto newwidth = length * (value1*m_pImgSrc->GetWidth()/length+value2*m_pImgSrc->GetHeight()/length);
	auto newheight = length * (value2*m_pImgSrc->GetWidth() / length + value1*m_pImgSrc->GetHeight() / length);
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	if (newwidth > rect.Width()) newwidth = rect.Width();
	if (newheight > rect.Height()) newheight = rect.Height();
	m_pImgCpy->Create(newwidth, newheight, m_pImgSrc->GetBPP());
	this->Invalidate();
	switch (thread)
	{
	case 0:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].angle = angle;
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::rorate_WIN, &m_pThreadParam[i]);
		}
	}
		break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].angle = angle;
			ImageProcess::rorate_WIN(&m_pThreadParam[i]);
		}
	}
	default:
		break;
	}
}

void CExperimentImgDlg::dft()
{

	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = GetTickCount64();
	
	m_pImgCpy = new CImage;
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());
	this->Invalidate();
	switch (thread)
	{
	case 0:
	{int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i) {
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
		m_pThreadParam[i].dst = m_pImgCpy;
		m_pThreadParam[i].src = m_pImgSrc;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::Dft, &m_pThreadParam[i]);
	}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			ImageProcess::Dft(&m_pThreadParam[i]);
		}
	}
	break;
	default:
		break;
	}
}

void CExperimentImgDlg::addGaussianNoise()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = GetTickCount64();
	auto p = (CPage4*)(this->pDialog[3]);
	CString s1;
	p->m_nedit3.GetWindowTextW(s1);
	double Means = _ttof(s1);
	p->m_nedit2.GetWindowTextW(s1);
	double Variance = _ttof(s1);
	
	//m_pImgSrc = m_pImgCpy;
	m_pImgCpy = new CImage();
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());
	this->Invalidate();
	switch (thread)
	{
	case 0:
	{
		int subLength = m_pImgSrc->GetWidth()* m_pImgSrc->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i) {
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
		m_pThreadParam[i].dst = m_pImgCpy;
		m_pThreadParam[i].src = m_pImgSrc;
		m_pThreadParam[i].means = Means;
		m_pThreadParam[i].variance = Variance;
		//m_pThreadParam[i].Gaussin = Gaussian;
		//m_pThreadParam[i].means = Means;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::addGaussian, &m_pThreadParam[i]);
	}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].dst = m_pImgCpy;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].means = Means;
			m_pThreadParam[i].variance = Variance;
			//m_pThreadParam[i].Gaussin = Gaussian;
			m_pThreadParam[i].means = Means;
			ImageProcess::addGaussian(&m_pThreadParam[i]);
		}
	}
	break;
	default:
		break;
	}
}

void CExperimentImgDlg::meansFilter()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = GetTickCount64();
	//m_pImgSrc = m_pImgCpy;
	m_pImgCpy = new CImage;
	if (!m_pImgCpy->IsNull()) {
		m_pImgCpy->Destroy();
	}
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());
	this->Invalidate();
	switch (thread)
	{
	case 0:
	{	int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].dst = m_pImgCpy;
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::meanFilter_WIN, &m_pThreadParam[i]);
	}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].dst = m_pImgCpy;
			ImageProcess::meanFilter_WIN(&m_pThreadParam[i]);
		}
	}
	break;
	default:
		break;
	}
}

void CExperimentImgDlg::GaussianFilter()
{
	startTime = GetTickCount64();
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	
	//m_pImgSrc = m_pImgCpy;
	auto p = (CPage4*)(this->pDialog[3]);
	CString s1;
	p->m_nedit1.GetWindowTextW(s1);
	int gaussian = _ttof(s1);
	p->m_nedit2.GetWindowTextW(s1);
	double Variance = _ttof(s1);
	m_pImgCpy = new CImage;
	if (!m_pImgCpy->IsNull()) {
		m_pImgCpy->Destroy();
	}
	//ImageCopy(m_pImgSrc, m_pImgCpy);
	m_pImgCpy->Create(m_pImgSrc->GetWidth(), m_pImgSrc->GetHeight(), m_pImgSrc->GetBPP());

	this->Invalidate();
	switch (thread)
	{
	case 0:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i) {
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
		m_pThreadParam[i].src = m_pImgSrc;
		m_pThreadParam[i].dst = m_pImgCpy;
		m_pThreadParam[i].Gaussin = gaussian;
		m_pThreadParam[i].variance = Variance;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::GaussianFilter_WIN, &m_pThreadParam[i]);
	}
	}
	break;
	case 1:
	{
		int subLength = m_pImgCpy->GetWidth()* m_pImgCpy->GetHeight() / m_nThreadNum;
#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i) {
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgCpy->GetWidth() * m_pImgCpy->GetHeight() - 1;
			m_pThreadParam[i].src = m_pImgSrc;
			m_pThreadParam[i].dst = m_pImgCpy;
			AfxBeginThread((AFX_THREADPROC)&ImageProcess::GaussianFilter_WIN, &m_pThreadParam[i]);
		}
	}
	break;
	default:
		break;
	}
}

void CExperimentImgDlg::MedianFilter()
{
//	AfxMessageBox(_T("中值滤波"));
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	//CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	//int circulation = clb_circulation->GetCheck() == 0 ? 1 : 4;

	startTime = GetTickCount64();
	m_nThreadNum;
	switch (thread)
	{
	case 0://win多线程
	{
		MedianFilter_WIN();
	}

	break;

	case 1://openmp
	{
		int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

#pragma omp parallel for num_threads(m_nThreadNum)
		for (int i = 0; i < m_nThreadNum; ++i)
		{
			m_pThreadParam[i].startIndex = i * subLength;
			m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
				(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
			m_pThreadParam[i].maxSpan = MAX_SPAN;
			m_pThreadParam[i].src = m_pImgSrc;
			ImageProcess::medianFilter(&m_pThreadParam[i]);
		}
	}

	break;

	case 2://cuda
		break;
	}
}

void CExperimentImgDlg::MedianFilter_WIN()
{
	int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
	int h = m_pImgSrc->GetHeight() / m_nThreadNum;
	int w = m_pImgSrc->GetWidth() / m_nThreadNum;
	for (int i = 0; i < m_nThreadNum; ++i)
	{
		m_pThreadParam[i].startIndex = i * subLength;
		m_pThreadParam[i].endIndex = i != m_nThreadNum - 1 ?
			(i + 1) * subLength - 1 : m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
		m_pThreadParam[i].maxSpan = MAX_SPAN;
		m_pThreadParam[i].src = m_pImgSrc;
		AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &m_pThreadParam[i]);
	}
}

LRESULT CExperimentImgDlg::OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int tempThreadCount = 0;
	static int tempProcessCount = 0;
	//CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	//int circulation = clb_circulation->GetCheck() == 0 ? 1 : 100;
	if ((int)wParam == 1)
	{
		// 当所有线程都返回了值1代表全部结束~显示时间
		if (m_nThreadNum == ++tempThreadCount)
		{
			auto endTime = GetTickCount64();
			CString timeStr;
			timeStr.Format(_T("耗时:%dms"), endTime - startTime);
			tempThreadCount = 0;
			tempProcessCount++;
			if (tempProcessCount < 1)
				MedianFilter_WIN();
			else
			{
				tempProcessCount = 0;
				auto endTime = GetTickCount64();
				CString timeStr;
				timeStr.Format(_T("处理%d次,耗时:%dms"), 1, endTime - startTime);
				AfxMessageBox(timeStr);
			}
			// 显示消息窗口
//			AfxMessageBox(timeStr);
		}
	}
	return 0;
}

LRESULT CExperimentImgDlg::OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int tempCount = 0;
	static int tempProcessCount = 0;
	//CButton* clb_circulation = ((CButton*)GetDlgItem(IDC_CHECK_CIRCULATION));
	//int circulation = clb_circulation->GetCheck() == 0 ? 1 : 100;
	if ((int)wParam == 1)
		tempCount++;
	if (m_nThreadNum == tempCount)
	{
		//CTime endTime = CTime::GetTickCount();
		//CString timeStr;
		//timeStr.Format(_T("耗时:%dms", endTime - startTime));
		tempCount = 0;
		tempProcessCount++;
		if (tempProcessCount < 1)
			AddNoise_WIN();
		else
		{
			tempProcessCount = 0;
			auto endTime = GetTickCount64();
			CString timeStr;
			timeStr.Format(_T("处理%d次,耗时:%dms"), 1, endTime - startTime);
			AfxMessageBox(timeStr);
		}
	//	AfxMessageBox(timeStr);
	}
	return 0;
}

LRESULT CExperimentImgDlg::OnMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int threadCount = 0;
	//static int processCount = 0;
	if ((int)wParam == 1) {
		if (m_nThreadNum == ++threadCount) {
			threadCount = 0;
			auto endTime = GetTickCount64();
			CString tempstr;
			tempstr.Format(_T("\r\n耗时:%dms"), endTime-startTime);
			str += tempstr;
			m_editText.SetWindowTextW(str);
		}
	}
	return 0;
}

void CExperimentImgDlg::OnStnClickedPicture()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CExperimentImgDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	m_CurSelTab = m_tab.GetCurSel();
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);
	*pResult = 0;
}


void CExperimentImgDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CExperimentImgDlg::OnNMThemeChangedScrollbar1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CExperimentImgDlg::OnCbnSelchangeComboThread()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CExperimentImgDlg::OnStnClickedPicture2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CExperimentImgDlg::OnBnClickedCheckCirculation()
{
	// TODO: 在此添加控件通知处理程序代码
}
