#pragma once

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") // для PNG

#include "Resource.h"
#include "Card.h"
#include "Shuffle.h"
#include "OpponentAI.h"
#include "Button.h"
#include "MFCOpenGLCardsDoc.h"

#include <ctime>
#include <random>
#include <vector>
#include <functional>

#define MAX_PAIRS 5
#define MAX_HAND DECKSIZE

class Game
{
private:
	GLuint cardFace = 0;
	GLuint cardBack = 0;
	GLuint opponent = 0;

	GLuint suits = 0;
	ULONG_PTR gdiplusToken = 0;

	GLuint fontBase = 0; //для шрифту
	HDC savedHDC = nullptr;

	std::vector<Card> deck;
	int DECKSIZE = 52;
	int deckTop = DECKSIZE;
	int deckBottom = 12;
	std::vector<Card> shuffleDeck;
	Suit trump;

	GLUquadric* quad = nullptr;

	std::vector<Card> discard;
	int discardCount = 0;

	std::vector<Card> playerHand;
	int playerCount = 6;
	std::vector<Card> enemyHand;
	int enemyCount = 6;

	bool playerStart = false;
	bool playerTurn = false;
	bool oppTurn = false;
	bool playerDefends = false;
	bool oppDefends = false;
	bool oppHasDefended = false;
	bool playerWasAttacker = false;
	bool trumpCardTaken = false;

	OpponentAI oppAI;
	Shuffle shuffle;

	struct CardPair {
		Card attack;
		Card defense;
		bool hasAttack = false;
		bool hasDefense = false;
	};

	CardPair pairs[MAX_PAIRS];
	int pairCount = 0;

	int hoveredCard = -1;
	int selectedCard = -1;
	int windowWidth;
	int windowHeight;

	bool canTake = false;  // Беру
	bool canPass = false;  // Пас 

	Button btnTake;
	Button btnPass;

	std::string m_displayName = "";
	bool m_isLoggedIn = false;
	int m_playerId = 0;

	std::function<void(int, std::string, int)> m_onGameEnd;
	std::function<void(CString)> m_onShowEndDialog;

private:
	GLuint LoadTextureFromResource(UINT resID)
	{
		HBITMAP hBmp = (HBITMAP)::LoadImage(
			AfxGetInstanceHandle(),
			MAKEINTRESOURCE(resID),
			IMAGE_BITMAP,
			0, 0,
			LR_CREATEDIBSECTION
		);

		if (!hBmp)
			return 0;

		BITMAP bmp;
		GetObject(hBmp, sizeof(BITMAP), &bmp);

		GLuint texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			bmp.bmBits
		);

		DeleteObject(hBmp);
		return texID;
	}

	GLuint LoadTextureFromPNGResource(UINT resID) {
		HRSRC hRes = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(resID), L"PNG");
		if (!hRes) return 0;

		HGLOBAL hData = LoadResource(AfxGetInstanceHandle(), hRes);
		if (!hData) return 0;

		DWORD size = SizeofResource(AfxGetInstanceHandle(), hRes);
		void* pData = LockResource(hData);

		IStream* pStream = SHCreateMemStream((BYTE*)pData, size);
		if (!pStream) return 0;

		Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromStream(pStream);
		pStream->Release();

		if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok) return 0;

		int w = bmp->GetWidth();
		int h = bmp->GetHeight();

		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(0, 0, w, h);
		bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);

		std::vector<unsigned char> pixels(w * h * 4);
		for (int y = 0; y < h; y++) {
			unsigned char* row = (unsigned char*)data.Scan0 + y * data.Stride;
			for (int x = 0; x < w; x++) {
				pixels[(y * w + x) * 4 + 0] = row[x * 4 + 2];
				pixels[(y * w + x) * 4 + 1] = row[x * 4 + 1];
				pixels[(y * w + x) * 4 + 2] = row[x * 4 + 0];
				pixels[(y * w + x) * 4 + 3] = row[x * 4 + 3];
			}
		}

		bmp->UnlockBits(&data);
		delete bmp;

		GLuint texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		return texID;
	}
		
	void DrawCardBack(float x, float y, float z) {
		glBindTexture(GL_TEXTURE_2D, cardBack);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex3f(x, y, z);

		glTexCoord2f(1, 1);
		glVertex3f(-x, y, z);

		glTexCoord2f(1, 0);
		glVertex3f(-x, -y, z);

		glTexCoord2f(0, 0);
		glVertex3f(x, -y, z);
		glEnd();
	}

	bool CanThrowCard(const Card& card) {
		if (pairCount == 0) return true;
		for (int i = 0; i < pairCount; i++) {
			if (pairs[i].hasAttack && pairs[i].attack.rank == card.rank) return true;
			if (pairs[i].hasDefense && pairs[i].defense.rank == card.rank) return true;
		}
		return false;
	}

	bool CanDefend(const Card& attacker, const Card& defender, Suit trump) {
		if (defender.suit == trump && attacker.suit != trump)
			return true;
		if (defender.suit == attacker.suit && defender.rank > attacker.rank)
			return true;
		if (defender.suit == trump && attacker.suit == trump && defender.rank > attacker.rank)
			return true;
		return false;
	}
public:
	Game() {
		srand(time(0));
		quad = gluNewQuadric();

		InitDeck();

		cardFace = LoadTextureFromResource(IDB_BITMAP4);
		cardBack = LoadTextureFromResource(IDB_BITMAP3);

		Gdiplus::GdiplusStartupInput gdiplusInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, NULL);
		suits = LoadTextureFromPNGResource(IDB_PNG1);
	}
	~Game() {
		if (quad)
			gluDeleteQuadric(quad);

		Gdiplus::GdiplusShutdown(gdiplusToken);
	}


	void InitDeck() {
		deck.resize(DECKSIZE);
		shuffleDeck.resize(DECKSIZE);
		discard.resize(DECKSIZE);
		playerHand.resize(DECKSIZE);
		enemyHand.resize(DECKSIZE);

		int minRank;
		if (DECKSIZE == 52) { minRank = 0; }
		else if (DECKSIZE == 36) { minRank = 4; }
		else { minRank = 7; }

		int i = 0;
		for (int s = 0; s < 4; ++s)
			for (int r = minRank; r < 13; ++r) {
				deck[i] = { (Rank)(r + 2), (Suit)s };
				shuffleDeck[i] = deck[i];
				i++;
			}
	}

	Card& GetCard(int i) {
		return shuffleDeck[i];
	}

	std::vector<Card>& GetDeck() {
		return shuffleDeck;
	}

	int GetDeckSize() {
		return DECKSIZE;
	}

	void SetDeckSize(int size) {
		DECKSIZE = size;
		InitDeck();
	}
	
	void DrawCasinoTable() {
		glPushMatrix();
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluQuadricNormals(quad, GLU_SMOOTH);  

		GLfloat matFelt[] = { 0.2f, 0.66f, 0.22f, 1.0f };
		GLfloat darkFelt[] = { 0.07f, 0.29f, 0.1f, 1.0f };
		GLfloat specFelt[] = { 0.05f, 0.05f, 0.05f, 1.0f }; 
		GLfloat shinFelt = 4.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matFelt);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specFelt);
		glMaterialf(GL_FRONT, GL_SHININESS, shinFelt);
		glColor3f(1, 1, 1); 
		gluDisk(quad, 0.0, 1.7, 30, 1);

		GLfloat matWood[] = { 0.51f, 0.36f, 0.13f, 1.0f };
		GLfloat specWood[] = { 0.3f,  0.2f,  0.1f,  1.0f };
		GLfloat shinWood = 20.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matWood);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specWood);
		glMaterialf(GL_FRONT, GL_SHININESS, shinWood);
		gluPartialDisk(quad, 1.5, 1.6, 30, 1, 0, 360);
		gluCylinder(quad, 1.5, 1.5, 0.1, 30, 10);
		gluCylinder(quad, 1.6, 1.6, 0.1, 30, 10);

		glTranslatef(0.0f, 0.0f, 0.1f);
		gluPartialDisk(quad, 1.5, 1.6, 30, 1, 0, 360);

		glTranslatef(0.0f, 0.0f, -1.1f);
		GLfloat matSide[] = { 0.075f, 0.29f, 0.105f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matSide);
		gluCylinder(quad, 1.7, 1.7, 1.0, 30, 10);

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matWood);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(quad, 0.0, 1.7, 30, 1);

		glPopMatrix();
	}

	void DrawCard(const Card& card, float x, float y, float z)
	{
		float cardW = 1.0f / 13.0f;
		float cardH = 1.0f / 5.0f;

		int col;
		if (card.rank == Ace)
			col = 0;
		else
			col = card.rank - 1;

		int row = card.suit;

		float u0 = col * cardW;
		float u1 = u0 + cardW;

		float v1 = 1.0f - row * cardH;
		float v0 = v1 - cardH;

		float width = 0.2f;
		float height = width * 1.4f;

		float hw = width * 0.5f;
		float hh = height * 0.5f;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, cardFace);
		glColor3f(1, 1, 1);

		//FACE
		glBegin(GL_QUADS);

		glTexCoord2f(u0, v1);
		glVertex3f(-x, y, z + 0.001);

		glTexCoord2f(u1, v1);
		glVertex3f(x, y, z + 0.001);

		glTexCoord2f(u1, v0);
		glVertex3f(x, -y, z + 0.001);

		glTexCoord2f(u0, v0);
		glVertex3f(-x, -y, z + 0.001);

		glEnd();

		//BACK
		DrawCardBack(x, y, z);

		glDisable(GL_TEXTURE_2D);
	}

	void DrawRemainingCards() {
		float x = 0.1f;
		float y = 0.12f;
		float z = -0.001f;

		glPushMatrix();
		glRotatef(270.0f, -0.5f, 0.0f, 0.0f);
		glTranslatef(0.7f, 0.0f, 0.0f);

		for (int i = deckBottom; i < deckTop; i++) {
			DrawCard(GetCard(i), x, y, z);
			z -= 0.001f;
		}

		if (!trumpCardTaken) {
			if (deckTop == deckBottom) {
				glTranslatef(-0.15f, 0.0f, -0.01f);
				DrawCard(GetCard(DECKSIZE - 1), x, y, 0.0f);
			}
			else {
				glPushMatrix();
				glRotatef(180.0f, -0.5f, 0.0f, 0.0f);
				glTranslatef(0.15f, 0.0f, 0.01f);
				DrawCard(GetCard(DECKSIZE - 1), x, y, -0.01f);
				glPopMatrix();
			}
		}


		glPopMatrix();
	}


	void DiscardCards() {
		for (int i = 0; i < pairCount; i++) {
			if (pairs[i].hasAttack)  discard[discardCount++] = pairs[i].attack;
			if (pairs[i].hasDefense) discard[discardCount++] = pairs[i].defense;
		}
		pairCount = 0;
	}

	void DrawPlayedCards() {
		if (discardCount == 0) return;

		float x = 0.1f;
		float y = 0.12f;

		glPushMatrix();
		glRotatef(270.0f, -0.5f, 0.0f, 0.0f);
		glTranslatef(-0.9f, 0.0f, 0.0f);

		float z = 0.0f;
		for (int i = 0; i < discardCount; i++) {
			DrawCard(discard[i], x, y, z);
			z -= 0.001f;
		}

		glPopMatrix();
	}

	void DrawOpponentCards(bool hide) {
		float x = 0.1f;
		float y = 0.12f;
		float z = 0.1f;

		glPushMatrix();
		glRotatef(90.0f, -0.5f, 0.0f, 0.0f);

		if (hide) {
			glRotatef(180.0f, 0.0f, -0.5f, 0.0f);
			glTranslatef(0.0f, 0.0f, -0.01f);
		}

		float maxWidth = 1.6f;
		float cardSpacing = (enemyCount > 1) ? min(0.2f, maxWidth / enemyCount) : 0.2f;

		float totalWidth = enemyCount * cardSpacing;
		float startX = -totalWidth / 2.0f + cardSpacing / 2.0f;

		for (int i = enemyCount - 1; i >= 0; i--) {
			glPushMatrix();
			glTranslatef(startX + i * cardSpacing, 0.7f, -0.1f);
			DrawCard(enemyHand[i], x, y, z);
			glPopMatrix();
		}

		glPopMatrix();
	}

	void DrawCurrentCards() {
		float x = 0.1f;
		float y = 0.12f;
		float z = 0.1f;

		glPushMatrix();
		glRotatef(90.0f, -0.5f, 0.0f, 0.0f);

		float maxWidth = 1.6f;
		float cardSpacing = (playerCount > 1) ? min(0.2f, maxWidth / playerCount) : 0.2f;

		float totalWidth = playerCount * cardSpacing;
		float startX = -totalWidth / 2.0f + cardSpacing / 2.0f;

		for (int i = playerCount - 1; i >= 0; i--) {
			glPushMatrix();
			glTranslatef(startX + i * cardSpacing, -0.7f, -0.1f);

			if (i == hoveredCard && i != selectedCard)
				glTranslatef(0.0f, 0.05f, 0.01f);

			DrawCard(playerHand[i], x, y, z);
			glPopMatrix();
		}

		glPopMatrix();
	}

	void SetupLighting() {
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);   

		GLfloat ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		glEnable(GL_LIGHT0);
		GLfloat pos0[] = { 0.5f, 2.0f, 1.5f, 1.0f }; 
		GLfloat diffuse0[] = { 1.0f, 0.95f, 0.85f, 1.0f }; 
		GLfloat specular0[] = { 1.0f, 1.0f,  1.0f,  1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, pos0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

		glEnable(GL_LIGHT1);
		GLfloat pos1[] = { -1.0f, -1.0f, 0.5f, 1.0f };
		GLfloat diffuse1[] = { 0.15f, 0.2f,  0.3f, 1.0f }; 
		GLfloat zero[] = { 0.0f,  0.0f,  0.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_POSITION, pos1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, zero);
	}

	void DrawAll() {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		SetupLighting();

		DrawCasinoTable();
		DrawOpponent();

		glDisable(GL_LIGHTING);

		DrawRemainingCards();
		DrawCurrentCards();
		DrawOpponentCards(true);
		DrawPlayedCards();
		DrawOppTurn();
		DrawButtons();
		DrawRemainingCount();
		DrawTrumpSuits();
		DrawPlayerInfo();
	}


	void DrawOpponent() {

		glPushMatrix();
		glColor3f(1, 0, 0);
		gluQuadricNormals(quad, GLU_SMOOTH);

		GLfloat matRed[] = { 0.7f, 0.05f, 0.05f, 1.0f };
		GLfloat specRed[] = { 0.9f, 0.8f,  0.8f,  1.0f };
		GLfloat shinRed = 60.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matRed);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specRed);
		glMaterialf(GL_FRONT, GL_SHININESS, shinRed);
		glColor3f(1, 1, 1);

		glTranslatef(0.0f, 0.7f, -2.5f);
		gluSphere(quad, 0.5, 32, 32);

		glPopMatrix();

	}

	bool CardLess(Card& a, Card& b, Suit trump)
	{
		if (a.suit == trump && b.suit != trump) return true;
		if (a.suit != trump && b.suit == trump) return false;

		if (a.suit == b.suit)
			return a.rank > b.rank;

		return a.suit < b.suit;
	}
	
	void SortHand(std::vector<Card>& hand, int count, Suit trump)
	{
		for (int i = 0; i < count - 1; i++) {
			for (int j = 0; j < count - i - 1; j++) {
				if (!CardLess(hand[j], hand[j + 1], trump)) {
					Card tmp = hand[j];
					hand[j] = hand[j + 1];
					hand[j + 1] = tmp;
				}
			}
		}
	}

	void StartPLayerHand() {
		for (int i = 0; i < 6; i++) {
			playerHand[i] = GetCard(i);
		}

		SortHand(playerHand, playerCount, GetCard(DECKSIZE - 1).suit);
	};

	void StartOpponentHand() {
		for (int i = 0, j = 6; i < 6; i++, j++) {
			enemyHand[i] = GetCard(j);
		}

		SortHand(enemyHand, enemyCount, GetCard(DECKSIZE - 1).suit);
	};

	void StartGame() {
		InitDeck();                                   
		discardCount = 0;                             
		pairCount = 0;                                
		playerCount = 6;
		enemyCount = 6;

		shuffle.FisherYates(GetDeck(), GetDeckSize()); 
		shuffle.Overhand(GetDeck(), GetDeckSize());
		shuffle.Riffle(GetDeck(), GetDeckSize());

		StartPLayerHand();
		StartOpponentHand();

		Rank minTrumpPlayer = Ace;
		Rank minTrumpEnemy = Ace;
		bool playerHasTrump = false;
		bool enemyHasTrump = false;
		trump = GetCard(DECKSIZE - 1).suit;
		trumpCardTaken = false;

		deckTop = DECKSIZE - 1;
		deckBottom = 12;

		for (int i = 0; i < 6; i++) {
			if (playerHand[i].suit == trump) {
				playerHasTrump = true;
				if (playerHand[i].rank < minTrumpPlayer)
					minTrumpPlayer = playerHand[i].rank;
			}
			if (enemyHand[i].suit == trump) {
				enemyHasTrump = true;
				if (enemyHand[i].rank < minTrumpEnemy)
					minTrumpEnemy = enemyHand[i].rank;
			}
		}

		if (playerHasTrump && enemyHasTrump) {
			playerStart = (minTrumpPlayer < minTrumpEnemy);
		}
		else if (playerHasTrump) playerStart = true;
		else if (enemyHasTrump) playerStart = false;
		else {
			shuffle.FisherYates(GetDeck(), GetDeckSize());
			shuffle.Overhand(GetDeck(), GetDeckSize());
			shuffle.Riffle(GetDeck(), GetDeckSize());
		}

		if (playerStart) {
			playerTurn = true;
			oppTurn = false;
			canPass = false;
			canTake = false;
			oppHasDefended = false;
			playerWasAttacker = true;
		}
		else {
			playerTurn = false;
			oppTurn = true;
			OppAttack();
			playerDefends = true;
			canTake = true;
			canPass = false;
			oppHasDefended = false;
			playerWasAttacker = false;
		}
	}

	void ContinueGame() {
		if (playerTurn) {
			canPass = true;
			canTake = false;
		}
		else if (playerDefends) {
			DiscardCards();
			playerDefends = false;
			oppHasDefended = false;
			RefillHands();
			canPass = (pairCount < MAX_PAIRS);
			canTake = false;

			if (playerCount == 0 || enemyCount == 0) {
				EndGame();
				return;
			}

			playerTurn = true;
			canPass = false;
			playerWasAttacker = false;
		}
	}

	void EndGame() {
		playerTurn = false;
		oppTurn = false;

		std::string result;
		CString resultUkr;
		int score = 0;

		int diffScore = oppAI.GetDifficulty() == 1 ? 10
			: oppAI.GetDifficulty() == 2 ? 25
			: 50; 

		if (enemyCount == 0 && playerCount == 0) {
			result = "draw";
			resultUkr = L"Нічия!";
			score = 0;
		}
		else if (playerCount == 0) {
			result = "win";
			resultUkr = L"Ви виграли!";
			score = diffScore;
		}
		else if (enemyCount == 0) {
			result = "loss";
			resultUkr = L"Суперник виграв!";
			score = -diffScore;
		}

		if (m_isLoggedIn && m_onGameEnd)
			m_onGameEnd(m_playerId, result, score);

		if (m_onShowEndDialog)
			m_onShowEndDialog(resultUkr);
	}

	void SetGameEndCallback(std::function<void(int, std::string, int)> cb) {
		m_onGameEnd = cb;
	}

	void SetEndDialogCallback(std::function<void(CString)> cb) {
		m_onShowEndDialog = cb;
	}

	void SetWindowSize(int width, int height) {
		windowWidth = width;
		windowHeight = height;
	}

	int GetWindowWidth() {
		return windowWidth;
	}

	int GetWindowHeight() {
		return windowHeight;
	}

	void SetDiff(int d) {
		oppAI.SetDifficulty(d);  
	}

	void OnMouseMove(int x, int y)
	{
		btnTake.hovered = btnTake.active &&
			x >= btnTake.x && x <= btnTake.x + btnTake.w &&
			y >= btnTake.y && y <= btnTake.y + btnTake.h;

		btnPass.hovered = btnPass.active &&
			x >= btnPass.x && x <= btnPass.x + btnPass.w &&
			y >= btnPass.y && y <= btnPass.y + btnPass.h;

		hoveredCard = -1;
		int winW = windowWidth;
		int winH = windowHeight;

		if (y < winH * 0.72f) return;

		float handLeft = winW * 0.30f;
		float handRight = winW * 0.70f;

		if (x < handLeft || x > handRight)
			return;

		float handWidth = handRight - handLeft;
		float cardWidth = handWidth / playerCount;

		int index = int((x - handLeft) / cardWidth);

		if (index >= 0 && index < playerCount)
			hoveredCard = index;
	}

	void OnMouseClick(int x, int y) {

		if (!playerTurn && !playerDefends)
			return;

		//Take
		if (canTake && x >= btnTake.x && x <= btnTake.x + btnTake.w &&
			y >= btnTake.y && y <= btnTake.y + btnTake.h) {

			for (int i = 0; i < pairCount; i++) {
				if (pairs[i].hasAttack)  playerHand[playerCount++] = pairs[i].attack;
				if (pairs[i].hasDefense) playerHand[playerCount++] = pairs[i].defense;
			}
			pairCount = 0;


			canTake = false;
			canPass = false;
			playerDefends = false;
			playerTurn = false;

			RefillHands();

			if (playerCount == 0 || enemyCount == 0) {
				EndGame();
				return;
			}

			if (playerWasAttacker) {
				playerTurn = true;
				canPass = false;
				canTake = false;
				oppHasDefended = false;
			}
			else {
				OppAttack();
				playerDefends = true;
				canTake = true;
				playerWasAttacker = false;
			}

			return;
		}

		//Pass
		if (canPass &&
			x >= btnPass.x && x <= btnPass.x + btnPass.w &&
			y >= btnPass.y && y <= btnPass.y + btnPass.h) {

			canPass = false;
			canTake = false;
			playerTurn = false;

			bool hasUnbeaten = false;
			for (int i = 0; i < pairCount; i++)
				if (pairs[i].hasAttack && !pairs[i].hasDefense) { hasUnbeaten = true; break; }

			if (hasUnbeaten) {
				for (int k = 0; k < pairCount; k++) {
					if (pairs[k].hasAttack)  enemyHand[enemyCount++] = pairs[k].attack;
					if (pairs[k].hasDefense) enemyHand[enemyCount++] = pairs[k].defense;
				}

				if (playerCount == 0 || enemyCount == 0) {
					EndGame();
					return;
				}

				pairCount = 0;
				RefillHands();
				playerTurn = true;
				playerWasAttacker = true;
				return;
			}

			DiscardCards();
			RefillHands();

			if (playerCount == 0 || enemyCount == 0) { EndGame(); return; }

			OppAttack();
			playerDefends = true;
			canTake = true;
			playerWasAttacker = false;
			return;
		}
		
		selectedCard = -1;
		int winW = windowWidth;
		int winH = windowHeight;

		if (y < winH * 0.72f) return;

		float handLeft = winW * 0.30f;
		float handRight = winW * 0.70f;

		if (x < handLeft || x > handRight) return;

		float handWidth = handRight - handLeft;
		float cardWidth = handWidth / playerCount;

		int index = int((x - handLeft) / cardWidth);

		if (index >= 0 && index < playerCount) {
			if (playerDefends) {
				for (int i = 0; i < pairCount; i++) {
					if (pairs[i].hasAttack && !pairs[i].hasDefense) {
												

						if (CanDefend(pairs[i].attack, playerHand[index], trump)) {
							pairs[i].defense = playerHand[index];
							pairs[i].hasDefense = true;

							for (int j = index; j < playerCount - 1; j++)
								playerHand[j] = playerHand[j + 1];
							playerCount--;
							selectedCard = -1;

							// Перевіряємо чи всі пари закриті
							bool allDefended = true;
							for (int j = 0; j < pairCount; j++)
								if (!pairs[j].hasDefense) { allDefended = false; break; }

							if (allDefended) {
								ContinueGame(); // гравець відбився
							}
						}
						return;
					}
				}
				return;
			}

			// Атака гравця
			if (pairCount < MAX_PAIRS) {
				if (!CanThrowCard(playerHand[index])) return;

				selectedCard = index;
				CardPair& p = pairs[pairCount];
				p.attack = playerHand[selectedCard];
				p.hasAttack = true;
				p.hasDefense = false;
				pairCount++;

				for (int i = selectedCard; i < playerCount - 1; i++)
					playerHand[i] = playerHand[i + 1];
				playerCount--;
				selectedCard = -1;
				playerWasAttacker = true;

				OppDefend();

				bool hasUnbeaten = false;
				for (int i = 0; i < pairCount; i++) {
					if (pairs[i].hasAttack && !pairs[i].hasDefense) {
						hasUnbeaten = true;
						break;
					}
				}

				canPass = true;
				canTake = false;
				playerTurn = true;
				
			}
		}
	}

	void OppAttack() {
		if (pairCount >= MAX_PAIRS) return;
		int idx = oppAI.ChooseAttackCard(enemyHand, enemyCount, trump);
		if (idx == -1) return;

		CardPair& p = pairs[pairCount];
		p.attack = enemyHand[idx];
		p.hasAttack = true;
		p.hasDefense = false;
		pairCount++;

		for (int i = idx; i < enemyCount - 1; i++)
			enemyHand[i] = enemyHand[i + 1];
		enemyCount--;
	}

	void OppDefend() {
		std::vector<Card> tempHand = enemyHand;

		int tempCount = enemyCount;

		for (int i = 0; i < pairCount; i++) {
			if (!pairs[i].hasAttack || pairs[i].hasDefense) continue;

			int idx = oppAI.ChooseDefenseCard(tempHand, tempCount, pairs[i].attack, trump);
			if (idx == -1) return; 

			for (int j = idx; j < tempCount - 1; j++)
				tempHand[j] = tempHand[j + 1];
			tempCount--;
		}

		for (int i = 0; i < pairCount; i++) {
			if (!pairs[i].hasAttack || pairs[i].hasDefense) continue;

			int idx = oppAI.ChooseDefenseCard(enemyHand, enemyCount, pairs[i].attack, trump);
			if (idx == -1) return;

			pairs[i].defense = enemyHand[idx];
			pairs[i].hasDefense = true;

			for (int j = idx; j < enemyCount - 1; j++)
				enemyHand[j] = enemyHand[j + 1];
			enemyCount--;
		}
	}
		
	void DrawOppTurn() {
		bool anyAttack = false;
		for (int i = 0; i < pairCount; i++)
			if (pairs[i].hasAttack) { anyAttack = true; break; }
		if (!anyAttack) return;

		float x = 0.1f, y = 0.12f;

		glPushMatrix();
		glRotatef(90.0f, -0.5f, 0.0f, 0.0f);

		glTranslatef(-0.6f, 0.0f, -0.1f);

		for (int i = 0; i < pairCount; i++) {
			const CardPair& p = pairs[i];
			if (!p.hasAttack) continue;

			DrawCard(p.attack, x, y, 0.1f);

			if (p.hasDefense) {
				glPushMatrix();
				glTranslatef(0.05f, 0.0f, 0.01f);
				DrawCard(p.defense, x, y, 0.1f);
				glPopMatrix();
			}

			glTranslatef(0.25f, 0.0f, 0.0f);
		}

		glPopMatrix();
	}

	void DrawButton(Button& btn, const char* label) {
		// фон 
		glColor3f(1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		glVertex2f(btn.x, btn.y);
		glVertex2f(btn.x + btn.w, btn.y);
		glVertex2f(btn.x + btn.w, btn.y + btn.h);
		glVertex2f(btn.x, btn.y + btn.h);
		glEnd();

		// рамка
		if (!btn.active) {
			glColor3f(0.3f, 0.3f, 0.3f); // сіра — неактивна
		}
		else if (btn.hovered) {
			glColor3f(0.9f, 0.8f, 0.1f); // жовта — hover
		}
		else {
			glColor3f(1.f, 0.0f, 0.0f); // зелена — активна
		}

		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(btn.x, btn.y);
		glVertex2f(btn.x + btn.w, btn.y);
		glVertex2f(btn.x + btn.w, btn.y + btn.h);
		glVertex2f(btn.x, btn.y + btn.h);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		float textX = btn.x + btn.w / 2.0f - strlen(label) * 4.0f;
		float textY = btn.y + btn.h / 2.0f + 5.0f;
		DrawText2D(label, textX, textY);

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}


	void DrawButtons() {
		int btnW = 100;
		int btnH = 40;
		int margin = 20;

		btnTake.x = windowWidth - btnW - margin;
		btnTake.y = windowHeight - btnH - margin;
		btnTake.w = btnW;
		btnTake.h = btnH;
		btnTake.active = canTake;

		btnPass.x = margin;
		btnPass.y = windowHeight - btnH - margin;
		btnPass.w = btnW;
		btnPass.h = btnH;
		btnPass.active = canPass;

		DrawButton(btnTake, "Take"); // Беру
		DrawButton(btnPass, "Pass"); // Пас
	}

	void DrawRemainingCount() {
		int count = deckTop - deckBottom;
		if (count < 0) count = 0;
		char text[16];
		sprintf_s(text, "%d", count);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		//float w = 50.0f;
		float w = 70.0f;
		//float h = 35.0f;
		float h = 50.0f;
		float x = windowWidth - w - 10.0f;
		float y = windowHeight / 2.0f - h / 2.0f;


		// фон
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		// рамка
		glColor3f(0.8f, 0.1f, 0.1f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		// текст
		glColor3f(0.0f, 0.0f, 0.0f);
		DrawText2D("Cards:", x + w / 2.0f - 18.0f, y + 20.0f);      
		DrawText2D(text, x + w / 2.0f - strlen(text) * 4.0f, y + 42.0f); 

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void DrawTrumpSuits() {
		float u0, v0, u1, v1;

		switch (trump) {
		case Spades:   
			u0 = 0.5f; v0 = 0.5f; u1 = 1.0f; v1 = 1.0f;
			break;
		case Hearts:   
			u0 = 0.0f; v0 = 0.5f; u1 = 0.5f; v1 = 1.0f;
			break;
		case Diamonds: 
			u0 = 0.5f; v0 = 0.0f; u1 = 1.0f; v1 = 0.5f;
			break;
		case Clubs:    
			u0 = 0.0f; v0 = 0.0f; u1 = 0.5f; v1 = 0.5f;
			break;
		}

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		float size = 60.0f;
		float x = windowWidth - size - 10.0f;
		float y = 10.0f;

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + size, y);
		glVertex2f(x + size, y + size);
		glVertex2f(x, y + size);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, suits);
		glColor3f(1, 1, 1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_QUADS);
		glTexCoord2f(u0, v0);
		glVertex2f(x, y);

		glTexCoord2f(u1, v0);
		glVertex2f(x + size, y);

		glTexCoord2f(u1, v1);
		glVertex2f(x + size, y + size);

		glTexCoord2f(u0, v1);
		glVertex2f(x, y + size);
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		glColor3f(0.8f, 0.1f, 0.1f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + size, y);
		glVertex2f(x + size, y + size);
		glVertex2f(x, y + size);
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void DrawPlayerInfo() {
		std::string label = m_isLoggedIn ? m_displayName : "Guest";
		float charWidth = 8.0f;  
		float padding = 20.0f;  
		float w = label.size() * charWidth + padding * 2;
		float h = 30.0f;
		float x = 10.0f;
		float y = 10.0f;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		// Фон
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		// Рамка
		glColor3f(0.8f, 0.1f, 0.1f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		float textX = x + padding;
		float textY = y + h / 2.0f + 5.0f;
		DrawText2D(label.c_str(), textX, textY);

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
		
	HDC GetHDC() { return savedHDC; }

	void InitFont(HDC hdc, int size = 16, int weight = FW_BOLD, const wchar_t* fontName = L"Arial") {
		savedHDC = hdc;
		if (fontBase) glDeleteLists(fontBase, 96);
		fontBase = glGenLists(96);
		HFONT font = CreateFont(
			-size, 0, 0, 0, weight,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			FF_DONTCARE | DEFAULT_PITCH, fontName
		);
		HFONT oldFont = (HFONT)SelectObject(hdc, font);
		wglUseFontBitmaps(hdc, 32, 96, fontBase);
		SelectObject(hdc, oldFont);
		DeleteObject(font);
	}

	void DrawText2D(const char* text, float x, float y) {
		glRasterPos2f(x, y);
		glListBase(fontBase - 32);
		glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	}

	void RefillHands() {
		std::vector<Card>& first = playerWasAttacker ? playerHand : enemyHand;
		int& firstN = playerWasAttacker ? playerCount : enemyCount;
		std::vector<Card>& second = playerWasAttacker ? enemyHand : playerHand;
		int& secondN = playerWasAttacker ? enemyCount : playerCount;

	
		while (firstN < 6 && deckTop > deckBottom)
			first[firstN++] = shuffleDeck[--deckTop];

		if (firstN < 6 && !trumpCardTaken && deckTop == deckBottom) {
			first[firstN++] = shuffleDeck[DECKSIZE - 1];
			trumpCardTaken = true;
		}

		while (secondN < 6 && deckTop > deckBottom)
			second[secondN++] = shuffleDeck[--deckTop];

		if (secondN < 6 && !trumpCardTaken && deckTop == deckBottom) {
			second[secondN++] = shuffleDeck[DECKSIZE - 1];
			trumpCardTaken = true;
		}

		SortHand(playerHand, playerCount, trump);
		SortHand(enemyHand, enemyCount, trump);
	}

	void SetPlayer(const std::string& displayName, int playerId) {
		m_displayName = displayName;
		m_playerId = playerId;
		m_isLoggedIn = true;
	}

	void Logout() {
		m_displayName = "";
		m_isLoggedIn = false;
	}

	bool IsLoggedIn() { return m_isLoggedIn; }
	int GetPlayerId() { return m_playerId; }
};