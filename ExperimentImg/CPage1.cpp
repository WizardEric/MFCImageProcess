// CPage1.cpp: 实现文件
//

#include "stdafx.h"
#include "ExperimentImg.h"
#include "CPage1.h"
#include "afxdialogex.h"


// CPage1 对话框

IMPLEMENT_DYNAMIC(CPage1, CDialogEx)

CPage1::CPage1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOPage1, m_nIndex);
}

BOOL CPage1::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_nIndex.AddString(_T("椒盐噪声"));
	m_nIndex.AddString(_T("中值滤波"));
	m_nIndex.SetCurSel(0);
	//p_cmb = cmb_function;
	return 0;
}


BEGIN_MESSAGE_MAP(CPage1, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBOPage1, &CPage1::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CPage1 消息处理程序


void CPage1::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}
