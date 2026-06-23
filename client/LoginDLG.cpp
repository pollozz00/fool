// LoginDLG.cpp: файл реализации
//

#include "pch.h"
#include "MFCOpenGLCards.h"
#include "afxdialogex.h"
#include "LoginDLG.h"
#include <winhttp.h>
#include <string>
#pragma comment(lib, "winhttp.lib")

// Диалоговое окно LoginDLG

IMPLEMENT_DYNAMIC(LoginDLG, CDialogEx)

LoginDLG::LoginDLG(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

LoginDLG::~LoginDLG()
{
}

void LoginDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoginDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &LoginDLG::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// Обработчики сообщений LoginDLG

void LoginDLG::OnBnClickedButtonLogin()
{
    CString username, password;
    GetDlgItemText(IDC_EDIT4, username);
    GetDlgItemText(IDC_EDIT3, password);

    if (username.IsEmpty() || password.IsEmpty()) {
        MessageBox(L"Заповніть всі поля!", L"Помилка", MB_ICONWARNING);
        return;
    }

    CString json;
    json.Format(L"{\"username\":\"%s\",\"password\":\"%s\"}", username, password);
    CT2A jsonUtf8(json, CP_UTF8);
    std::string body(jsonUtf8);

    HINTERNET hSession = WinHttpOpen(L"FoolGame/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", 7126, 0);

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,L"POST", L"/api/Player/login",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/json", -1L, WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)body.c_str(), (DWORD)body.size(), (DWORD)body.size(), 0);
    WinHttpReceiveResponse(hRequest, NULL);

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(DWORD);
    WinHttpQueryHeaders(hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        NULL, &statusCode, &statusSize, NULL);

    if (statusCode == 200) {
        std::string response;
        DWORD bytesRead = 0;
        char buf[4096];
        while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
            buf[bytesRead] = '\0';
            response += buf;
        }

        std::string key = "\"displayName\":\"";
        size_t pos = response.find(key);
        if (pos != std::string::npos) {
            pos += key.length();
            size_t end = response.find("\"", pos);
            if (end != std::string::npos) {
                std::string displayName = response.substr(pos, end - pos);
                m_displayName = CString(displayName.c_str());
            }
        }

        size_t idPos = response.find("\"id\":");
        if (idPos != std::string::npos) {
            idPos += 5; 
            size_t idEnd = response.find(",", idPos);
            m_playerId = std::stoi(response.substr(idPos, idEnd - idPos));
        }

        if (m_displayName.IsEmpty())
            m_displayName = username;

        m_loggedIn = true;
        m_username = username;

        CString msg = L"Вітаємо, " + m_displayName + L"!";
        MessageBox(msg, L"Вхід", MB_ICONINFORMATION);
        EndDialog(IDOK);
    }
    else if (statusCode == 401) {
        MessageBox(L"Невірний логін або пароль!", L"Помилка", MB_ICONWARNING);
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
