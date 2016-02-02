#pragma once

#include <GL/glew.h>

#pragma warning( disable :4290)

/**
*	Класс позволяет использовать вершинные буферы для вывода графиков. 
*/
class CVertexBuffer
{
	protected:
		GLuint _vert_id;		
		GLuint _col_id;
		GLsizei _vert_len;	
	public:
		CVertexBuffer() : _vert_id(0), _col_id(0), _vert_len(0)	{ };
		~CVertexBuffer();
		
		void SetVertices(GLint pointDim, GLsizeiptr count, GLvoid *data, GLenum usage = GL_STATIC_DRAW);
		void SetVerticesColor(GLint colorDim, GLsizeiptr count, GLvoid *data, GLenum usage = GL_STATIC_DRAW);
		void BindVertices(GLsizeiptr count, GLenum usage);
		void SetSubVertices(GLsizeiptr count,  GLvoid *data);
		
		void DrawArrays(GLsizei count = 0);		
		void DrawArrays(GLint first, GLsizei count, GLenum mode, bool useColor = false);
};


