#pragma once

#include "Button.h"
#include "Game.h"
#include "OpponentAI.h"

class Menu
{
private:
	Button btnStart;
	Button btnSettings;
	Button btnExit;

	Button btnDifficulty;
	Button btnDeckSize;
	Button btnBack;

	int btnW = 200;
	int btnH = 50;
	int gap = 20;

	Game* gm;
	OpponentAI opp;

	int difficulty = 1;       // 1 = Easy, 2 = Medium, 3 = Hard
	int deckSizeIndex = 0;    
	int deckSizes[3] = { 52, 36, 24 };

	enum class MenuState { None, StartGame, Settings, Exit };
	MenuState state = MenuState::None;

private:
	bool HitTest(const Button& btn, int mx, int my) {
		return mx >= btn.x && mx <= btn.x + btn.w &&
			my >= btn.y && my <= btn.y + btn.h;
	}

public:
	Menu(Game* g) : gm(g) {};
	~Menu() {}

	MenuState GetState() { return state; }
	void SetState(MenuState s) { state = s; }
	void ResetState() { state = MenuState::None; }
	bool IsInGame() { return state == MenuState::StartGame; }

	void DrawMenu() {

		int w = gm->GetWindowWidth();
		int h = gm->GetWindowHeight();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, w, h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);

		// Заголовок
		gm->InitFont(gm->GetHDC(), 48, FW_BOLD, L"Arial");
		glColor3f(1.0f, 0.85f, 0.2f);
		gm->DrawText2D("Fool", w / 2.0f - strlen("Fool") * 14.0f, h / 4.0f);

		gm->InitFont(gm->GetHDC(), 18, FW_BOLD, L"Arial");

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// Роутинг по состоянию
		if (state == MenuState::Settings)
			DrawSettingsScreen();
		else if (state == MenuState::StartGame)
			DrawGameScreen();
		else
			DrawMenuScreen();
	}

	void StartButton() {
		SetState(MenuState::StartGame);
		gm->StartGame();
	}
	void SettingsButton() {
		SetState(MenuState::Settings);
	}
	void ExitButton() {
		SetState(MenuState::Exit);
		PostQuitMessage(0);
	}

	void DrawButton(Button& btn, const char* label) {
		int w = gm->GetWindowWidth();
		int h = gm->GetWindowHeight();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, w, h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		// Фон кнопки
		if (btn.hovered)
			glColor3f(0.2f, 0.6f, 0.2f);
		else
			glColor3f(0.15f, 0.45f, 0.15f);

		glBegin(GL_QUADS);
		glVertex2f(btn.x, btn.y);
		glVertex2f(btn.x + btn.w, btn.y);
		glVertex2f(btn.x + btn.w, btn.y + btn.h);
		glVertex2f(btn.x, btn.y + btn.h);
		glEnd();

		// Рамка
		glColor3f(0.8f, 0.8f, 0.2f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(btn.x, btn.y);
		glVertex2f(btn.x + btn.w, btn.y);
		glVertex2f(btn.x + btn.w, btn.y + btn.h);
		glVertex2f(btn.x, btn.y + btn.h);
		glEnd();

		// Текст
		glColor3f(1.0f, 1.0f, 1.0f);
		float textX = btn.x + btn.w / 2.0f - strlen(label) * 5.0f;
		float textY = btn.y + btn.h / 2.0f + 6.0f;
		gm->DrawText2D(label, textX, textY);

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void DrawMenuScreen() {
		int w = gm->GetWindowWidth();
		int h = gm->GetWindowHeight();

		int centerX = w / 2 - btnW / 2;
		int startY = h / 2 - 10;

		btnStart.x = centerX;    
		btnStart.y = startY;
		btnStart.w = btnW;
		btnStart.h = btnH;
		btnStart.active = true;

		btnSettings.x = centerX;
		btnSettings.y = startY + btnH + gap;
		btnSettings.w = btnW;
		btnSettings.h = btnH;
		btnSettings.active = true;

		btnExit.x = centerX;
		btnExit.y = startY + (btnH + gap) * 2;
		btnExit.w = btnW;
		btnExit.h = btnH;
		btnExit.active = true;

		DrawButton(btnStart, "Start"); 
		DrawButton(btnSettings, "Settings"); 
		DrawButton(btnExit, "Exit"); 
	}

	void DrawSettingsScreen() {
		int w = gm->GetWindowWidth();
		int h = gm->GetWindowHeight();
		
		int cx = w / 2 - btnW / 2;
		int sy = h / 2 - 60;

		btnDifficulty.x = cx;
		btnDifficulty.y = sy;
		btnDifficulty.w = btnW;
		btnDifficulty.h = btnH;
		btnDifficulty.active = true;

		btnDeckSize.x = cx;
		btnDeckSize.y = sy + (btnH + gap);
		btnDeckSize.w = btnW;
		btnDeckSize.h = btnH;
		btnDeckSize.active = true;

		btnBack.x = cx;
		btnBack.y = sy + (btnH + gap) * 2;
		btnBack.w = btnW;
		btnBack.h = btnH;
		btnBack.active = true;

		char lblDiff[32], lblDeck[32];
		const char* diffNames[] = { "Easy", "Medium", "Hard" };
		sprintf_s(lblDiff, "Difficulty: %s", diffNames[difficulty - 1]);
		sprintf_s(lblDeck, "Deck: %d cards", deckSizes[deckSizeIndex]);

		DrawButton(btnDifficulty, lblDiff);
		DrawButton(btnDeckSize, lblDeck);
		DrawButton(btnBack, "Back");
	}

	void DrawGameScreen() {}

	void DifficultyButton() {
		difficulty = difficulty % 3 + 1;   
		gm->SetDiff(difficulty);
	}

	void DeckSizeButton() {
		deckSizeIndex = (deckSizeIndex + 1) % 3;   
		gm->SetDeckSize(deckSizes[deckSizeIndex]);
	}

	void BackButton() {
		SetState(MenuState::None);
	}

	void OnMouseClick(int mx, int my) {
		if (state == MenuState::Settings) {
			if (HitTest(btnDifficulty, mx, my)) DifficultyButton();
			else if (HitTest(btnDeckSize, mx, my)) DeckSizeButton();
			else if (HitTest(btnBack, mx, my)) BackButton();
		}
		else {
			if (HitTest(btnStart, mx, my)) StartButton();
			else if (HitTest(btnSettings, mx, my)) SettingsButton();
			else if (HitTest(btnExit, mx, my)) ExitButton();
		}
	}

	void OnMouseMove(int mx, int my) {
		if (state == MenuState::Settings) {
			btnDifficulty.hovered = HitTest(btnDifficulty, mx, my);
			btnDeckSize.hovered = HitTest(btnDeckSize, mx, my);
			btnBack.hovered = HitTest(btnBack, mx, my);
		}
		else {
			btnStart.hovered = HitTest(btnStart, mx, my);
			btnSettings.hovered = HitTest(btnSettings, mx, my);
			btnExit.hovered = HitTest(btnExit, mx, my);
		}
	}
};

