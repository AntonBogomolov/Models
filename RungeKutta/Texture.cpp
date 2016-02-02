#include <iostream>
#include <string>
#include <stdexcept>

#include <SDL/SDL_image.h>
#include <GL/glew.h>

#pragma warning( disable :4290)

#include "Texture.h"

using std::string;
using std::invalid_argument;

typedef unsigned char byte;

CTexture::CTexture() : _id(0)
{
	glGenTextures(1, &_id);
}


CTexture::CTexture(string filename) : _id(0)
{
	glGenTextures(1, &_id);
	Bind();
	Load(filename);
}


CTexture::CTexture(SDL_Surface *image) : _id(0)
{
	glGenTextures(1, &_id);
	Bind();
	Load(image);
}


CTexture::CTexture(GLenum iformat, int width, int height, GLfloat *pixels) : _id(0)
{
	glGenTextures(1, &_id);
	Bind();
	Load(iformat, width, height, pixels);
}


CTexture::CTexture(GLenum iformat, int width, int height, GLubyte *pixels) : _id(0)
{
	glGenTextures(1, &_id);
	Bind();
	Load(iformat, width, height, pixels);
}


CTexture::~CTexture()
{
	glDeleteTextures(1, &_id);
}

CTexture::CTexture(string* filenames) : _id(0)
{
	Uint8 bpp;
	GLenum iformat, format;

	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _id);
       
    SDL_Surface  *tex;

	if (!(tex = (SDL_Surface *)IMG_Load(filenames[0].c_str()))) {
		throw invalid_argument("CTexture::Load - loading error");
	}

	bpp = tex->format->BitsPerPixel;
	if (bpp == 32) {
		iformat = GL_RGBA8;
		if (tex->format->Rmask == 0x000000ff ) {
			format = GL_RGBA;
		}
		else {
			format = GL_BGRA;
		}
	}
	else if (bpp  == 24) {
		iformat = GL_RGB8;
		if (tex->format->Rmask == 0x000000ff) {
			format = GL_RGB;
		}
		else {
			format = GL_BGR;
		}
	}
	else {
		throw invalid_argument("CTexture::Load - Not a truecolor image");
	}

    int count = 2;        
//	while (!filenames[count].empty() ) count++;
        
	int     w    = tex ->w;
    int     h    = tex ->h;	
	size_t  sz   = tex ->w * tex ->format->BytesPerPixel * h;
    byte* buf  = (byte *) malloc (  sz * count );
    	
    if(buf)memcpy(buf,tex->pixels, sz );
	SDL_FreeSurface(tex);
    
    for (int i = 1; i < count; i++ )
    {
		if (!(tex = (SDL_Surface *)IMG_Load(filenames[i].c_str()))) {
			throw invalid_argument("CTexture::Load - loading error");
		}
        
        if ( tex == NULL )
            break;            
                                                    // check that all textures have same foramt/size
        if ( tex ->w != w || tex ->h != h )
            break;

       if(buf) memcpy ( buf + i*sz, tex->pixels, sz );
    
        SDL_FreeSurface(tex);
    }
        
    glGenTextures   ( 1, &_id );
    glBindTexture   ( GL_TEXTURE_2D_ARRAY, _id );   

    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,     GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,     GL_REPEAT );

	glTexImage3D (    GL_TEXTURE_2D_ARRAY, 0, iformat, w, h, count, 0, format, GL_UNSIGNED_BYTE, buf );
    
    free ( buf );	
}

void CTexture::Bind(int mode)
{
	glBindTexture(mode, _id);
}

void CTexture::Load(string filename) throw(invalid_argument)
{
	SDL_Surface *image;

	std::cout << "Loading " << filename << std::endl;

	if (!(image = (SDL_Surface *)IMG_Load(filename.c_str()))) {
		throw invalid_argument("CTexture::Load - loading error");
	}

	Load(image);

	SDL_FreeSurface(image);
}


void CTexture::Load(SDL_Surface *surface) throw(invalid_argument)
{
	Uint8 bpp;
	GLenum iformat, format;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	bpp = surface->format->BitsPerPixel;
	if (bpp == 32) {
		iformat = GL_RGBA8;
		if (surface->format->Rmask == 0x000000ff ) {
			format = GL_RGBA;
		}
		else {
			format = GL_BGRA;
		}
	}
	else if (bpp  == 24) {
		iformat = GL_RGB8;
		if (surface->format->Rmask == 0x000000ff) {
			format = GL_RGB;
		}
		else {
			format = GL_BGR;
		}
	}
	else {
		throw invalid_argument("CTexture::Load - Not a truecolor image");
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, iformat, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4,surface->w, surface->h, format, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	w = surface->w;
	h = surface->h;
	bpp = bpp/8;
}


void CTexture::Load(GLenum iformat, int width, int height, GLfloat *pixels)
{
	GLenum format;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (iformat == GL_RGB16F_ARB || iformat == GL_RGB32F_ARB) {
		format = GL_RGB;
	}
	else if (iformat == GL_RGBA16F_ARB || iformat == GL_RGB32F_ARB) {
		format = GL_RGBA;
	}
	else if (iformat == GL_RGBA8 || iformat == GL_RGBA || iformat == 4) {
		format = GL_RGBA;
	}
	else if (iformat == GL_RGB8 || iformat == GL_RGB || iformat == 3) {
		format = GL_RGB;
	}
	else if (iformat == GL_LUMINANCE8_ALPHA8 || iformat == GL_LUMINANCE_ALPHA || iformat == 2) {
		format = GL_LUMINANCE_ALPHA;
	}
	else if (iformat == GL_LUMINANCE8 || iformat == GL_LUMINANCE || iformat == 1) {
		format = GL_LUMINANCE;
	}
	else if (iformat == GL_DEPTH_COMPONENT || iformat == GL_DEPTH_COMPONENT16_ARB || 
		iformat == GL_DEPTH_COMPONENT24_ARB || iformat == GL_DEPTH_COMPONENT32_ARB) {
		format = GL_DEPTH_COMPONENT;
	}
	else {
		throw invalid_argument("CTexture::Load - Unknown internal format");
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_FLOAT, pixels);
	gluBuild2DMipmaps(GL_TEXTURE_2D, iformat, width, height, format, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	w = width;
	h = height;
	bpp = 4;
}


void CTexture::Load(GLenum iformat, int width, int height, GLubyte *pixels)
{
	GLenum format;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (iformat == GL_RGB8) {
		format = GL_RGB;
	}
	else if (iformat == GL_RGBA8) {
		format = GL_RGBA;
	}
	else {
		throw invalid_argument("CTexture::Load - Unknown internal format");
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);	
	gluBuild2DMipmaps(GL_TEXTURE_2D, iformat, width, height, format, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	
	w = width;
	h = height;
	bpp = 4;
}


void CTexture::SetFiltering(GLenum filter)
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}
