
// MFCOpenGLCardsView.cpp: реализация класса CMFCOpenGLCardsView
//
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "MFCOpenGLCards.h"
#endif

#include "MFCOpenGLCardsDoc.h"
#include "MFCOpenGLCardsView.h"

#include "Mainfrm.h"

#include "Game.h"
#include "Menu.h"
#include "Shuffle.h"
#include "EndGameDialog.h"

#include "LoginDLG.h"
#include "RegisterDLG.h"
#include <winhttp.h>
#include <string>
#pragma comment(lib, "winhttp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCOpenGLCardsView

IMPLEMENT_DYNCREATE(CMFCOpenGLCardsView, CView)

BEGIN_MESSAGE_MAP(CMFCOpenGLCardsView, CView)
	// Стандартные команды печати
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCOpenGLCardsView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_ACCOUNT_LOGOUT, &CMFCOpenGLCardsView::OnAccountLogout)
	ON_COMMAND(ID_ACCOUNT_LOGIN, &CMFCOpenGLCardsView::OnAccountLogin)
	ON_COMMAND(ID_ACCOUNT_REGISTER, &CMFCOpenGLCardsView::OnAccountRegister)
END_MESSAGE_MAP()

// Создание или уничтожение CMFCOpenGLCardsView

CMFCOpenGLCardsView::CMFCOpenGLCardsView() noexcept
{
	// TODO: добавьте код создания

}

CMFCOpenGLCardsView::~CMFCOpenGLCardsView()
{
	delete game;
	delete menu;
	delete shuffle;
}

BOOL CMFCOpenGLCardsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: изменить класс Window или стили посредством изменения
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

bool CMFCOpenGLCardsView::Init_3d()
{
	HGLRC		hrc;  // дескриптор OPENGL
	int pixelformat;

	CMainFrame* pp = (CMainFrame*)this->GetParentFrame();

	if ((pixelformat = ChoosePixelFormat(GetDC()->GetSafeHdc(), &(pp->pfd))) == 0 ||
		SetPixelFormat(GetDC()->GetSafeHdc(), pixelformat, &pp->pfd) == FALSE)
		return false;


	hrc = wglCreateContext(GetDC()->GetSafeHdc());// создание контекста
	wglMakeCurrent(GetDC()->GetSafeHdc(), hrc);   // установка контекста

	// Формирование списка шрифтов в OpenGL

	glEnable(GL_DEPTH_TEST); // активизация буфера глубины

	return true;
}

// Рисование CMFCOpenGLCardsView

void CMFCOpenGLCardsView::OnDraw(CDC* /*pDC*/)
{
	
	CMFCOpenGLCardsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CRect rect;
	this->GetClientRect(rect);
	glViewport(0, 0,rect.Width(), rect.Height());

	game->SetWindowSize(rect.Width(), rect.Height());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//створення перспективної камери
	gluPerspective(20.0, (float)rect.Width() / rect.Height(), 0.1, 100.0);
	//1)FOV 2)співвідношення сторін вікна 3)ближча площина 4) дальня площина 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//поворот камери(сцени)
	glTranslatef(pDoc->translateX, pDoc->translateY, pDoc->translateZ);

	//поворот навколо осі
	glRotatef(pDoc->rotateX, 1.0f, 0.0f, 0.0f);
	glRotatef(pDoc->rotateY, 0.0f, 1.0f, 0.0f);
	glRotatef(pDoc->rotateZ, 0.0f, 0.0f, 1.0f);
	
	if (menu->IsInGame())
		game->DrawAll();
	else
		menu->DrawMenu();

	SwapBuffers(wglGetCurrentDC());
}


// Печать CMFCOpenGLCardsView


void CMFCOpenGLCardsView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCOpenGLCardsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// подготовка по умолчанию
	return DoPreparePrinting(pInfo);
}

void CMFCOpenGLCardsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте дополнительную инициализацию перед печатью
}

void CMFCOpenGLCardsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте очистку после печати
}

void CMFCOpenGLCardsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCOpenGLCardsView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Диагностика CMFCOpenGLCardsView

#ifdef _DEBUG
void CMFCOpenGLCardsView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCOpenGLCardsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCOpenGLCardsDoc* CMFCOpenGLCardsView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCOpenGLCardsDoc)));
	return (CMFCOpenGLCardsDoc*)m_pDocument;
}
#endif //_DEBUG


// Обработчики сообщений CMFCOpenGLCardsView

int CMFCOpenGLCardsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!Init_3d())
	{
		MessageBox(L"Помилка ініціалізації графiки!!!",
			L"Увага", MB_ICONINFORMATION);
		return -1;
	}

	game = new Game();
	menu = new Menu(game);
	shuffle = new Shuffle();

	game->InitDeck();	

	game->InitFont(GetDC()->GetSafeHdc());

	game->SetGameEndCallback([this](int playerId, std::string result, int score) {
		SendGameResult(playerId, result, score);
		});

	game->SetEndDialogCallback([this](CString result) {
		ShowEndDialog(result);
		});

	return 0;
}

void CMFCOpenGLCardsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMFCOpenGLCardsDoc* pDoc = GetDocument();

	float stepT = 0.1f;
	float stepR = 5.0f;

	switch (nChar) {

	// Переміщення осі
	case VK_LEFT:
		pDoc->rotateY -= stepR;
		break;
	case VK_RIGHT:
		pDoc->rotateY += stepR;
		break;
	case VK_UP:
		pDoc->rotateX -= stepR;
		break;
	case VK_DOWN:
		pDoc->rotateX += stepR;
		break;

	// Переміщення камери
	case 'W':
		pDoc->translateZ += stepT;
		break;
	case 'S':
		pDoc->translateZ -= stepT;
		break;
	case 'A':
		pDoc->translateX -= stepT;
		break;
	case 'D':
		pDoc->translateX += stepT;
		break;
	case 'Q':
		pDoc->translateY += stepT;
		break;
	case 'E':
		pDoc->translateY -= stepT;
		break;

	// Сброс
	case 'R':
		pDoc->rotateX = 50.0f;
		pDoc->rotateY = 0.0f;
		pDoc->rotateZ = 0.0f;

		pDoc->translateX = 0.0f;
		pDoc->translateY = 0.0f;
		pDoc->translateZ = -5.0f;

		break;
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMFCOpenGLCardsView::OnMouseMove(UINT nFlags, CPoint point)
{
	game->OnMouseMove(point.x, point.y);
	menu->OnMouseMove(point.x, point.y);
	Invalidate(FALSE);
}

void CMFCOpenGLCardsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	game->OnMouseClick(point.x, point.y);
	menu->OnMouseClick(point.x, point.y);
	Invalidate(FALSE);
}

void CMFCOpenGLCardsView::OnAccountLogin()
{
	LoginDLG dlg;
	if (dlg.DoModal() == IDOK && dlg.m_loggedIn) {
		CT2A displayName(dlg.m_displayName, CP_UTF8);
		game->SetPlayer(std::string(displayName), dlg.m_playerId);
		Invalidate(FALSE); 
	}
}

void CMFCOpenGLCardsView::OnAccountRegister()
{
	RegisterDLG dlg;
	dlg.DoModal();
}

void CMFCOpenGLCardsView::OnAccountLogout()
{
	if (!game->IsLoggedIn()) {
		MessageBox(L"Ви не увійшли в акаунт!", L"Інфо", MB_ICONINFORMATION);
		return;
	}

	int result = MessageBox(
		L"Ви впевнені що хочете вийти?",
		L"Вихід",
		MB_YESNO | MB_ICONQUESTION
	);

	if (result == IDYES) {
		game->Logout();
		GetParentFrame()->SetWindowText(L"Guest");
		Invalidate(FALSE);
	}
}

void CMFCOpenGLCardsView::SendGameResult(int playerId, const std::string& result, int score)
{
	std::string body = "{\"playerId\":" + std::to_string(playerId) +
		",\"score\":" + std::to_string(score) +
		",\"result\":\"" + result + "\"}";

	HINTERNET hSession = WinHttpOpen(L"FoolGame/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", 7126, 0);

	HINTERNET hRequest = WinHttpOpenRequest(hConnect,
		L"POST", L"/api/Rating/save",
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

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
}

void CMFCOpenGLCardsView::ShowEndDialog(const CString& result)
{
	EndGameDialog dlg(result, this);
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_choice == EndGameDialog::Choice::Restart) {
			menu->StartButton(); 
			Invalidate(FALSE);
		}
		else if (dlg.m_choice == EndGameDialog::Choice::Menu) {
			menu->ResetState(); 
			Invalidate(FALSE);
		}
	}
}