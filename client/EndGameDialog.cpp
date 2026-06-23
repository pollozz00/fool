// EndGameDialog.cpp: файл реализации
//

#include "pch.h"
#include "MFCOpenGLCards.h"
#include "afxdialogex.h"
#include "EndGameDialog.h"


// Диалоговое окно EndGameDialog

IMPLEMENT_DYNAMIC(EndGameDialog, CDialogEx)

EndGameDialog::EndGameDialog(const CString& result, CWnd* pParent) : CDialogEx(IDD_DIALOG_ENDGAME, pParent), m_resultText(result) {}

EndGameDialog::~EndGameDialog() {}

void EndGameDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL EndGameDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetDlgItemText(IDC_RESULT_TEXT, m_resultText);
    return TRUE;
}

BEGIN_MESSAGE_MAP(EndGameDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_RESTART, &EndGameDialog::OnBtnRestart)
    ON_BN_CLICKED(IDC_BUTTON_MENU, &EndGameDialog::OnBtnMenu)
END_MESSAGE_MAP()

void EndGameDialog::OnBtnRestart()
{
    m_choice = Choice::Restart;
    EndDialog(IDOK);
}

void EndGameDialog::OnBtnMenu()
{
    m_choice = Choice::Menu;
    EndDialog(IDOK);
}