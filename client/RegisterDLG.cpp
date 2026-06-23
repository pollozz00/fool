// RegisterDLG.cpp: файл реализации
//

#include "pch.h"
#include "MFCOpenGLCards.h"
#include "afxdialogex.h"
#include "RegisterDLG.h"

#include <winhttp.h>
#include <string>
#pragma comment(lib, "winhttp.lib")


// Диалоговое окно RegisterDLG

IMPLEMENT_DYNAMIC(RegisterDLG, CDialogEx)

RegisterDLG::RegisterDLG(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REGISTER, pParent)
{

}

RegisterDLG::~RegisterDLG()
{
}

void RegisterDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RegisterDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &RegisterDLG::OnBnClickedButtonRegister)
END_MESSAGE_MAP()


// Обработчики сообщений RegisterDLG

void RegisterDLG::OnBnClickedButtonRegister()
{
	CString username;
	CString password;
	CString displayName;

	GetDlgItemText(IDC_EDIT_USER, username);
	GetDlgItemText(IDC_EDIT_PASSWORD, password);
	GetDlgItemText(IDC_EDIT_USERNAME, displayName);

    if (username.IsEmpty() || password.IsEmpty() || displayName.IsEmpty()) {
        MessageBox(L"Заповніть всі поля!", L"Помилка", MB_ICONWARNING);
        return;
    }

    CString json;
    json.Format(
        L"{\"username\":\"%s\",\"password\":\"%s\",\"displayName\":\"%s\"}",
        username, password, displayName
    );
    CT2A jsonUtf8(json, CP_UTF8);
    std::string body(jsonUtf8);

    HINTERNET hSession = WinHttpOpen(L"FoolGame/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", 7126, 0);

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"POST", L"/api/player/register",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    WinHttpAddRequestHeaders(hRequest,
        L"Content-Type: application/json", -1L,
        WINHTTP_ADDREQ_FLAG_ADD);

    WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(), (DWORD)body.size(),
        (DWORD)body.size(), 0);

    WinHttpReceiveResponse(hRequest, NULL);

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(DWORD);
    WinHttpQueryHeaders(hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        NULL, &statusCode, &statusSize, NULL);

    if (statusCode == 201) {
        m_registered = true;
        MessageBox(L"Аккаунт створено! Теперь зайди.", L"Успіх", MB_ICONINFORMATION);
        EndDialog(IDOK);
    }
    else if (statusCode == 409) {
        MessageBox(L"Username вже зайнятий!", L"Помилка", MB_ICONWARNING);
    }
    else {
        CString err;
        err.Format(L"Помилка: %d", statusCode);
        MessageBox(err, L"Помилка", MB_ICONERROR);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}
