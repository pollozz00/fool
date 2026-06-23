#pragma once
#include "afxdialogex.h"


// Диалоговое окно RegisterDLG

class RegisterDLG : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDLG)

public:
	RegisterDLG(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~RegisterDLG();

	bool m_registered = false;

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRegister();
};
