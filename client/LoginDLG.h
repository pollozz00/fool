#pragma once
#include "afxdialogex.h"


// Диалоговое окно LoginDLG

class LoginDLG : public CDialogEx
{
	DECLARE_DYNAMIC(LoginDLG)

public:
	LoginDLG(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~LoginDLG();

	CString m_username;
	CString m_displayName;
	bool m_loggedIn = false;
	int m_playerId = 0;

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogin();
};
