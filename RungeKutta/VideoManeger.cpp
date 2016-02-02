#include "VideoManeger.h"
#include "Texture.h"

int CVideoManeger::width;
int CVideoManeger::height;

CVideoManeger* CVideoManeger::instance = NULL;
CTexture* CVideoManeger::fontTexture = NULL;

CVideoManeger::CVideoManeger(void)
{
	screen=NULL;
	info = NULL;
	width = 1700;
	height = 800;
	bpp = 0;
	flags = 0;

	fontTexture = NULL;

	width  = GetSystemMetrics(SM_CXSCREEN) - 330;
	height = GetSystemMetrics(SM_CYSCREEN) - 80;
}

CVideoManeger::~CVideoManeger(void)
{
	DestroyFonts();	
	delete fontTexture;
}

void CVideoManeger::Init()
{
	// ----- SDL init ---------------
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}
	
	atexit(SDL_Quit);
	
	info = SDL_GetVideoInfo();
	bpp = info->vfmt->BitsPerPixel;

// ----- OpenGL attribute setting via SDL ---------------
//	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
//	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
//	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
//	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
	
	flags = SDL_OPENGL|SDL_RESIZABLE;//|SDL_FULLSCREEN;
		
	SDL_Surface *g_icon = (SDL_Surface *)IMG_Load("ico.png");	
    SDL_WM_SetIcon(g_icon,NULL);

	// ----- Setting up the screen surface ---------------
	SetWindowPos(320,40);
	if ((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0)
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        exit(-1);
	}
	SDL_WM_SetCaption("Eco Models", NULL);
	
	// ---- GLEW init ---------------
	GLenum err = glewInit();
	
	if (GLEW_OK != err) 
	{
		std::cout << "GLEW error: " << glewGetErrorString(err);
		exit(-1);
	}

	if (!GLEW_VERSION_2_0) 
	{
	//	std::cout << "OpenGL2 is not supported";
	//	exit(-1);
	}
	// ---- TTF ---------------------
	TTF_Init();
	atexit(TTF_Quit);

	InitFonts();
	// ----SDL_image_----------------	
	IMG_Init(IMG_INIT_PNG);
	atexit(IMG_Quit);

	SDL_FreeSurface(g_icon);
	fontTexture = new CTexture("font.png");
}

void CVideoManeger::ResizeWindow(const int Width,const int Height)
{
	width = Width;
	height = Height;

	if (screen)
	{
		SDL_FreeSurface(screen); 		
	}
	if ((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0)
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        exit(-1);
	}
	glViewport ( 0, 0, width, height );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -2000, 2000);
}

void CVideoManeger::glEnable2D()
{  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, width, height,0 , -2000, 2000);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void CVideoManeger::glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}

void CVideoManeger::SetClipingRect(const double X1, const double Y1, const double X2, const double Y2)
{
	GLdouble eqnRight[4]	= { -1.0, 0.0, 0.0, X2 };
	GLdouble eqnLeft[4]		= { 1.0, 0.0, 0.0, -X1 };
	GLdouble eqnTop[4]		= { 0.0, 1.0, 0.0, -Y1 };
	GLdouble eqnBottom[4]	= { 0.0, -1.0, 0.0, Y2 };

	glClipPlane( GL_CLIP_PLANE0, eqnRight );
	glEnable( GL_CLIP_PLANE0 );

	glClipPlane( GL_CLIP_PLANE1, eqnLeft );
	glEnable( GL_CLIP_PLANE1 );

	glClipPlane( GL_CLIP_PLANE2, eqnTop );
	glEnable( GL_CLIP_PLANE2);

	glClipPlane( GL_CLIP_PLANE3, eqnBottom );
	glEnable( GL_CLIP_PLANE3 );
}
void CVideoManeger::UnsetClipingRect()
{
	glDisable( GL_CLIP_PLANE0 );
	glDisable( GL_CLIP_PLANE1 );
	glDisable( GL_CLIP_PLANE2 );
	glDisable( GL_CLIP_PLANE3 );
}

void CVideoManeger::Redraw()
{
	SDL_Event event;

	event.type = SDL_USEREVENT;
	event.user.code = 110;
	event.user.data1 = 0;
	event.user.data2 = 0;
	SDL_PushEvent(&event);
}

void CVideoManeger::SetWindowPos(const int X,const int Y)
{	
	char* buf = new char[255];
	sprintf(buf,"SDL_VIDEO_WINDOW_POS=%d,%d",X,Y);

	SDL_putenv(buf);
	delete [] buf;
}

CVideoManeger* CVideoManeger::GetInstance()
{
	if(instance == NULL)
	{
		instance = new CVideoManeger();		
	}
	return instance;
}

void CVideoManeger::BindFontTexture()
{
	fontTexture->Bind();
}

void CVideoManeger::UnbindFontTexture()
{
	fontTexture->Unbind();
}