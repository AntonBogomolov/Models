#pragma once

#define GLUI_NO_LIB_PRAGMA
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL_image.lib")
#pragma comment(lib, "SDL_ttf.lib")

#pragma comment(lib, "glew32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma warning( disable : 4996 )

#include <GL\glew.h>
#include <SDL\SDL.h>
#include <SDL\SDL_image.h>
#include <SDL\SDL_ttf.h>

#include <iostream>

// C++ runtime headers
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <iostream>

#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <algorithm>

#include "resource.h"
#include <windows.h>

///////////////////////////////////

int nextpoweroftwo(int x);
int round(double x);
void SDL_GL_RenderTextCyrillic(char *text, int fontId, SDL_Color color, SDL_Rect *location);
void SDL_GL_RenderText(char *text, int fontId, SDL_Color color, SDL_Rect *location);
void RenderNumber(double Number,double X, double Y, double Z, double SizeX,double SizeY,float colR,float colG,float colB);

void InitFonts();
void DestroyFonts();
