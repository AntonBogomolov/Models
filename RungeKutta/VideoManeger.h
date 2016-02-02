#pragma once
#include "global.h"

class CTexture;

/**
*	 ласс, который используетс€ дл€ инициализации OpenGL, SDL. 
*/
class CVideoManeger
{
private:
	static CVideoManeger* instance;
public:
	SDL_Surface *screen;
	SDL_Event event;
	const SDL_VideoInfo* info;
	static int width;
	static int height;
	static CTexture* fontTexture;
	int bpp ;
	int flags;	

public:
	static CVideoManeger* GetInstance();

	void Init();
	void ResizeWindow(const int Width,const int Height);
	static void SetClipingRect(const double X1, const double Y1, const double X2, const double Y2);
	static void UnsetClipingRect();

	static void BindFontTexture();
	static void UnbindFontTexture();

	static void Redraw();
	static void SetWindowPos(const int X,const int Y);
	
	static void glEnable2D();
	static void glDisable2D();

	CVideoManeger(void);
	~CVideoManeger(void);
};
