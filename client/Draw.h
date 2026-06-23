#pragma once
class Draw
{
private:
	GLUquadric* quad = gluNewQuadric();

	// карти - 24(9-тузи), 36(6-тузи), 52(2-тузи), 54(с джокерами)
	//(пики) (♠), черви (♥), бубны (♦) и трефы (♣).
	GLuint textures[52];

	GLuint texture1 = 0;

	//textures[i] = LoadBMPTexture("card" + i + ".bmp");

	GLuint LoadBMPTexture(const char* filename){
		BITMAP bmp;
		HBITMAP hBmp = (HBITMAP)LoadImageA(
			NULL, filename, IMAGE_BITMAP,
			0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION
		);

		if (!hBmp) {
			MessageBoxA(NULL, "BMP NOT LOADED", "ERROR", MB_OK);
			return 0;
		}

		GetObject(hBmp, sizeof(BITMAP), &bmp);

		GLuint texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB,
			bmp.bmWidth, bmp.bmHeight,
			0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits
		);

		DeleteObject(hBmp);
		return texID;
	}

public:
	Draw() {
		texture1 = LoadBMPTexture("C:\\Users\\kulik\\OneDrive\\Робочий стіл\\Диплом\\MFCOpenGLCards\\MFCOpenGLCards\\res\\card.bmp");
		//texture1 = LoadBMPTexture("..\\..\\res\\card.bmp");
	}
	~Draw() {
		gluDeleteQuadric(quad);
		
	}

	void Table(){
		glColor3ub(50, 168, 56);

		gluCylinder(quad,1,1,1,30,100);
	}

	void Card(float x1,float y1, float x2, float y2) {

		//GLuint texture1 = LoadBMPTexture("card.bmp");
		
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture1);
		
		glColor3f(1,1,1);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x1, y1);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(x2, y1);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x2, y2);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x1, y2);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		//glRectf(x1, y1, x2, y2);
	};

	void Cards(){}

	void Opponent(){
	
		glColor3f(1, 1, 0);
		gluSphere(quad, 1, 10, 10);
	}

	
};

