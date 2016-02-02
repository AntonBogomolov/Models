#ifndef CLASS_TEXTURE
#define CLASS_TEXTURE

#include <stdexcept>
#include <SDL/SDL.h>

/**
*	Класс служит для загрузки изображения с символами для вывода текста. 
*/
class CTexture
{
	public:		
		int w;
		int h;
		int bpp;
	private:	
		GLuint _id;
		void Load(std::string filename);
		void Load(SDL_Surface *image);
		void Load(GLenum iformat, int width, int height, GLfloat *pixels);
		void Load(GLenum iformat, int width, int height, GLubyte *pixels);
	public:
		CTexture();
		CTexture(std::string filename);
		CTexture(std::string* filenames);
		CTexture(SDL_Surface *surface);
		CTexture(GLenum iformat, int width, int height, GLfloat *pixels);
		CTexture(GLenum iformat, int width, int height, GLubyte *pixels);
		~CTexture();
		/// Set texture filtering for both magnification and minification
		void SetFiltering(GLenum filter);
		/// Bind the texture to the current unit
		void Bind(int mode=GL_TEXTURE_2D);
		/// Disable texture rendering for the current unit
		static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); };

		GLuint GetID(){return _id;};
};

#endif
