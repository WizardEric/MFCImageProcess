#pragma once


// CPage2 对话框

class CPage2 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage2)

public:
	CPage2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPage2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo1page2();
	CComboBox m_nIndexPAGE2;
	
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnCbnSelchangeCombopage2();
	CEdit m_nZoom;
};
