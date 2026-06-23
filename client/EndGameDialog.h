#pragma once
#include "afxdialogex.h"


// Диалоговое окно EndGameDialog

class EndGameDialog : public CDialogEx
{
	DECLARE_DYNAMIC(EndGameDialog)

public:
	EndGameDialog(const CString& result, CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~EndGameDialog();

	enum class Choice { None, Restart, Menu };
	Choice m_choice = Choice::None;
// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ENDGAME };
#endif

protected:
	CString m_resultText;
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRestart();
	afx_msg void OnBtnMenu();
	DECLARE_MESSAGE_MAP()
};
