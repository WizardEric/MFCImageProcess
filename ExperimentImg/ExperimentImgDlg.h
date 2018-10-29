
// ExperimentImgDlg.h : 头文件
//

#pragma once
#include"CPage1.h"
#include"CPage2.h"
#include"CPage3.h"
#include"CPage4.h"
#include "afxwin.h"
#include "ImageProcess.h"

#define PI 3.141592653
#define MAX_THREAD 8
#define MAX_SPAN 15
struct DrawPara
{
	CImage* pImgSrc;
	CDC* pDC;
	int oriX;
	int oriY;
	int width;
	int height;
};

// CExperimentImgDlg 对话框
class CExperimentImgDlg : public CDialogEx
{
// 构造
public:
	CExperimentImgDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPERIMENTIMG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	CImage* getImage() { return m_pImgSrc; }
	void MedianFilter();
	void AddNoise();
	void AddNoise_WIN();
	void zoom();
	void rorate();
	void dft();
	void addGaussianNoise();
	void meansFilter();
	void GaussianFilter();
	void ThreadDraw(DrawPara *p);
	void wienerFileter();
	static UINT Update(void* p);
	
	void ImageCopy(CImage* pImgSrc, CImage* pImgDrt);
	void MedianFilter_WIN();
	afx_msg LRESULT OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnMsgReceived(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;
	CImage * m_pImgSrc;
	CImage * m_pImgCpy;
	//CTabCtrl m_tab;
	int m_nThreadNum;
	ThreadParam* m_pThreadParam;
	ULONGLONG startTime;
//	ThreadParam * m_pThreadParam;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void paint(CStatic&,CImage*);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	CEdit mEditInfo;
	CStatic mPictureControl;
	afx_msg void OnCbnSelchangeComboFunction();
	afx_msg void OnNMCustomdrawSliderThreadnum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonProcess();
	CButton m_CheckCirculation;
	afx_msg void OnStnClickedPicture();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	
	CTabCtrl m_tab;
	int m_CurSelTab;
	CPage1 m_page1;
	CPage2 m_page2;
	CPage3 m_page3;
	CPage4 m_page4;
	CDialog* pDialog[4];//用来保存对话框对象指针
	CString tempPath;
	CString str;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnNMThemeChangedScrollbar1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editText;
	afx_msg void OnCbnSelchangeComboThread();
	CStatic m_pic;
	afx_msg void OnStnClickedPicture2();
	CDC *tempDC;
	afx_msg void OnBnClickedCheckCirculation();
	
	CButton m_checkuse;
};
