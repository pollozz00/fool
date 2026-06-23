
// MFCOpenGLCardsView.h: интерфейс класса CMFCOpenGLCardsView
//

#pragma once
#include "Game.cpp"
#include "Menu.cpp"
#include "Shuffle.cpp"
#include "OpponentAI.cpp"

class CMFCOpenGLCardsView : public CView
{
protected: // создать только из сериализации
	CMFCOpenGLCardsView() noexcept;
	DECLARE_DYNCREATE(CMFCOpenGLCardsView)

// Атрибуты
public:
	CMFCOpenGLCardsDoc* GetDocument() const;

// Операции
public:
	bool Init_3d();
	Game* game = nullptr;
	Menu* menu = nullptr;
	Shuffle* shuffle = nullptr;


// Переопределение
public:
	virtual void OnDraw(CDC* pDC);  // переопределено для отрисовки этого представления
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Реализация
public:
	virtual ~CMFCOpenGLCardsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAccountLogout();
	afx_msg void OnAccountLogin();
	afx_msg void OnAccountRegister();
	void SendGameResult(int playerId, const std::string& result, int score);
	void ShowEndDialog(const CString& result);
};

#ifndef _DEBUG  // версия отладки в MFCOpenGLCardsView.cpp
inline CMFCOpenGLCardsDoc* CMFCOpenGLCardsView::GetDocument() const
   { return reinterpret_cast<CMFCOpenGLCardsDoc*>(m_pDocument); }
#endif

