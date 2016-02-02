#include "global.h"
#include "VideoManeger.h"

wchar_t * MultiCharToUniChar(char* mbString){ 
  setlocale(LC_CTYPE,"rus");
  int len = 0; 
  len = (int)strlen(mbString) + 1; 
  wchar_t *ucString = new wchar_t[len]; 
  mbstowcs(ucString, mbString, len); 
  return ucString; 
}

// --------------------------------------------------------------------------------
TTF_Font *fnt_12pt = NULL;
TTF_Font *fnt_16pt = NULL;
TTF_Font *fnt_24pt = NULL;
TTF_Font *fnt_32pt = NULL;
TTF_Font *fnt_42pt = NULL;

void InitFonts()
{
	if(!(fnt_12pt = TTF_OpenFont("Courier_New.ttf", 12))) {
		printf("Error loading font: %s", TTF_GetError());
		return;
	}
	if(!(fnt_16pt = TTF_OpenFont("Courier_New.ttf", 16))) {
		printf("Error loading font: %s", TTF_GetError());
		return;
	}
	if(!(fnt_24pt = TTF_OpenFont("Courier_New.ttf", 24))) {
		printf("Error loading font: %s", TTF_GetError());
		return;
	}
	if(!(fnt_32pt = TTF_OpenFont("Courier_New.ttf", 32))) {
		printf("Error loading font: %s", TTF_GetError());
		return;
	}
	if(!(fnt_42pt = TTF_OpenFont("Courier_New.ttf", 42))) {
		printf("Error loading font: %s", TTF_GetError());
		return;
	}
}

void DestroyFonts()
{
	TTF_CloseFont( fnt_12pt );
	TTF_CloseFont( fnt_16pt );
	TTF_CloseFont( fnt_24pt );
	TTF_CloseFont( fnt_32pt );
	TTF_CloseFont( fnt_42pt );
}

void SDL_GL_RenderTextCyrillic(char *text, 
                      int fontId,
                      SDL_Color color,
                      SDL_Rect *location)
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;

	int w,h;
	GLuint texture;

	TTF_Font *fnt = NULL;
	switch (fontId)
	{
		case 12:
			fnt = fnt_12pt;
		break;
		case 16:
			fnt = fnt_16pt;
		break;
		case 24:
			fnt = fnt_24pt;
		break;
		case 32:
			fnt = fnt_32pt;
		break;
		case 42:
			fnt = fnt_42pt;
		break;
		default:
			fnt = fnt_16pt;
		break;
	}
		
	 wchar_t *utext = MultiCharToUniChar(text);

	/* Use SDL_TTF to render our text */
	Uint16* stext = new Uint16[strlen(text)+1];
		for (size_t i = 0; i < strlen(text); ++i) {
			stext[i] = utext[i]; 
		}
	stext[strlen(text)] = 0;

	initial = TTF_RenderUNICODE_Blended(fnt,stext,color);
	delete[] stext;

	/* Convert the rendered text to a known format */
	w = nextpoweroftwo(initial->w);
	h = nextpoweroftwo(initial->h);
	
	intermediary = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
			GL_UNSIGNED_BYTE, intermediary->pixels );
	
	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	/* Draw a quad at location */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); 
			glVertex2f(location->x    , location->y);
		glTexCoord2f(1.0f, 1.0f); 
			glVertex2f((float)(location->x + w), location->y);
		glTexCoord2f(1.0f, 0.0f); 
			glVertex2f((float)(location->x + w), (float)(location->y - h));
		glTexCoord2f(0.0f, 0.0f); 
			glVertex2f(location->x    , (float)(location->y - h));
	glEnd();
		
	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

void SDL_GL_RenderText(char *text, int fontId, SDL_Color color, SDL_Rect *location)
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;

	int w,h;
	GLuint texture;

	TTF_Font *fnt = NULL;
	switch (fontId)
	{
		case 12:
			fnt = fnt_12pt;
		break;
		case 16:
			fnt = fnt_16pt;
		break;
		case 24:
			fnt = fnt_24pt;
		break;
		case 32:
			fnt = fnt_32pt;
		break;
		case 42:
			fnt = fnt_42pt;
		break;
		default:
			fnt = fnt_16pt;
		break;
	}

	initial = TTF_RenderText_Blended(fnt,text,color);

	/* Convert the rendered text to a known format */
	w = nextpoweroftwo(initial->w);
	h = nextpoweroftwo(initial->h);
	
	intermediary = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
			GL_UNSIGNED_BYTE, intermediary->pixels );
	
	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f); 
			glVertex2f(location->x    , location->y);
		glTexCoord2f(1.0f, 1.0f); 
			glVertex2f((float)(location->x + w), location->y);
		glTexCoord2f(1.0f, 0.0f); 
			glVertex2f((float)(location->x + w), (float)(location->y - h));
		glTexCoord2f(0.0f, 0.0f); 
			glVertex2f(location->x    , (float)(location->y - h));
	glEnd();
	
	/* Bad things happen if we delete the texture before it finishes */
//	glFinish();
	
	/* return the deltas in the unused w,h part of the rect */
//	location->w = initial->w;
//	location->h = initial->h;
	
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

//void RenderText(char*s,float ofsx, float ofsy, float ofsz, float sizex,float sizey)
//{
//	glActiveTextureARB(GL_TEXTURE0_ARB); // не нужно если без мультитекстуринга
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(ofsx,ofsy,ofsz);
//	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_ALPHA);
//	glEnable(GL_ALPHA_TEST);
//	glAlphaFunc(GL_GREATER,0.02f);
////	FontTexture.Bind();           // Как нибудь биндим текстуру шрифта. У меня текстура это класс. У вас - как хотите. Например: glBindTexture(GL_TEXTURE_2D, TexID);
//  
//	glBegin(GL_QUADS);
//	for(unsigned int i = 0; i < strlen(s); i++)
//	{
//		float tx = 0;
//		float ty = 0;
//		tx = (float)((int)(s[i]%16));
//		tx = tx / 16.0f;
//		ty = (float)((int)((s[i]-s[i]%16)/16+1));
//		ty = 1 - ty / 16.0f;
//
//		glTexCoord2d(tx            ,ty+1.0f/16.0f );	glVertex3f(i*sizex,sizey,0);        
//		glTexCoord2d(tx+1.0f/16.0f ,ty+1.0f/16.0f );	glVertex3f((i+1)*sizex,sizey,0);	
//		glTexCoord2d(tx+1.0f/16.0f ,ty            );	glVertex3f((i+1)*sizex,0,0);
//		glTexCoord2d(tx            ,ty			  );	glVertex3f(i*sizex,0,0);  
//	}
//	glEnd();
//
//	glPopMatrix();
//	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_ALPHA);
//	glDisable(GL_ALPHA_TEST);
//}

void RenderNumber(double Number,double X, double Y, double Z, double SizeX,double SizeY,float colR,float colG,float colB)
{	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	//glLoadIdentity();
	glTranslated(X,Y,Z);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.01f);
		
	CVideoManeger::BindFontTexture();
  
	char numStr[255];	
	Number = ceil(Number);
	sprintf(numStr,"%.0f",Number);

	const float chSize = 1.0f/14.0f; 
	float tx = 0;
	float ty = 0;

	glColor3f(colR,colG,colB);

	glBegin(GL_QUADS);
	for(unsigned int i = 0; i < strlen(numStr); i++)
	{		
		switch(numStr[i])
		{
			case '-':
				tx = 12 * chSize;
			break;
			case '.':
				tx = 10 * chSize;
			break;
			case '0':
				tx = 0 * chSize;
			break;
			case '1':
				tx = 1 * chSize;
			break;
			case '2':
				tx = 2 * chSize;
			break;
			case '3':
				tx = 3 * chSize;
			break;
			case '4':
				tx = 4 * chSize;
			break;
			case '5':
				tx = 5 * chSize;
			break;
			case '6':
				tx = 6 * chSize;
			break;
			case '7':
				tx = 7 * chSize;
			break;
			case '8':
				tx = 8 * chSize;
			break;
			case '9':
				tx = 9 * chSize;
			break;
		}

		glTexCoord2f(tx        ,1.0f );	glVertex3d(i*SizeX,SizeY,0);        
		glTexCoord2f(tx+chSize ,1.0f );	glVertex3d((i+1)*SizeX,SizeY,0);	
		glTexCoord2f(tx+chSize ,0.0f );	glVertex3d((i+1)*SizeX,0,0);
		glTexCoord2f(tx        ,0.0f );	glVertex3d(i*SizeX,0,0);  
	}
	glEnd();

	CVideoManeger::UnbindFontTexture();
	glPopMatrix();
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA);
	glDisable(GL_ALPHA_TEST);
	glPopAttrib();
}

inline int round(double x)
{
	return (int)(x + 0.5);
}

inline int nextpoweroftwo(int x)
{
	double logbase2 = log(double(x)) / log(2.0);
	return round(pow(2,ceil(logbase2)));
}