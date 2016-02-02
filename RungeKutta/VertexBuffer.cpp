#include "VertexBuffer.h"

CVertexBuffer::~CVertexBuffer()
{
	if (glIsBuffer(_vert_id)) {
		glDeleteBuffers(1, &_vert_id);
	}	
	if (glIsBuffer(_col_id)) {
		glDeleteBuffers(1, &_col_id);
	}	
}


void CVertexBuffer::SetVertices(GLint pointDim, GLsizeiptr count, GLvoid *data, GLenum usage)
{
	if (!glIsBuffer(_vert_id)) {
		glGenBuffers(1, &_vert_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vert_id);
	int size =  (count*pointDim)*sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER,size, data, usage);
	_vert_len = count;

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer( GL_ARRAY_BUFFER, _vert_id );
	glVertexPointer( pointDim, GL_FLOAT, 0, 0 );
}

void CVertexBuffer::SetVerticesColor(GLint colorDim, GLsizeiptr count, GLvoid *data, GLenum usage)
{
	if (!glIsBuffer(_col_id)) {
		glGenBuffers(1, &_col_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _col_id);
	int size =  (count*colorDim)*sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER,size, data, usage);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer( GL_ARRAY_BUFFER, _col_id );
	glColorPointer( colorDim, GL_FLOAT, 0, 0 );
}

void CVertexBuffer::BindVertices(GLsizeiptr count, GLenum usage)
{
	if (!glIsBuffer(_vert_id)) {
		glGenBuffers(1, &_vert_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vert_id);
	int size =  (count*2)*sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER,size, 0, usage);
	_vert_len = count;
}

void CVertexBuffer::SetSubVertices(GLsizeiptr count, GLvoid *data)
{
	if (!glIsBuffer(_vert_id)) {
		glGenBuffers(1, &_vert_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vert_id);
	int size =  (count*2)*sizeof(GLfloat);	
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	_vert_len = count;
}

void CVertexBuffer::DrawArrays(GLsizei count) 
{
	if (count == 0) {
		count = _vert_len;
	}
	else if (count > _vert_len) {
		//throw out_of_range("CVertexBuffer::DrawArrays - number of vertices exceeded");
	}	

	glDrawArrays(GL_TRIANGLES, 0, count);
}


void CVertexBuffer::DrawArrays(GLint first, GLsizei count, GLenum mode, bool useColor ) 
{	
	glEnableClientState( GL_VERTEX_ARRAY );
	if(useColor)  glEnableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );

	glDrawArrays(mode, first, count);

	if(useColor)  glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

//	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
