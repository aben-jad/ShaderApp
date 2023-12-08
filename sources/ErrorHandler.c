#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

void ClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

void GetError()
{
	GLenum er;
	while ( er = glGetError())
		printf("error : %d ", er);
}
