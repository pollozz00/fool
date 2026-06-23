
// MFCOpenGLCardsDoc.h: интерфейс класса CMFCOpenGLCardsDoc 
//


#pragma once


class CMFCOpenGLCardsDoc : public CDocument
{
protected: // создать только из сериализации
	CMFCOpenGLCardsDoc() noexcept;
	DECLARE_DYNCREATE(CMFCOpenGLCardsDoc)

// Атрибуты
public:
	float translateX = 0.0f;
	float translateY = 0.0f;
	float translateZ = -5.0f;

	float rotateX = 50.0f;
	float rotateY = 0.0f;
	float rotateZ = 0.0f;

// Операции
public:

// Переопределение
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Реализация
public:
	virtual ~CMFCOpenGLCardsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Вспомогательная функция, задающая содержимое поиска для обработчика поиска
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
