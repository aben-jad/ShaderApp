#include <stdio.h>
#include "mydefines.h"
#include "myincludes.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>

uint CompileShader(char* _shaderSrc, uint _type)
{
	uint id = glCreateShader(_type);

	const char* s = _shaderSrc;

	glShaderSource(id, 1, &s, NULL);

	glCompileShader(id);

	int res;

	glGetShaderiv(id,GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE)
	{
		printf(_shaderSrc);
		exit(0);
	}

	return id;
}

uint CreateShader(char* _vertShader, char* _fragShader)
{
	uint prog = glCreateProgram();

	uint vs = CompileShader(_vertShader, GL_VERTEX_SHADER);
	uint fs = CompileShader(_fragShader, GL_FRAGMENT_SHADER);

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);
	glValidateProgram(prog);

	glDeleteProgram(vs);
	glDeleteProgram(fs);

	return prog;
}

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("error in Init");
		return 0;
	}


	char *winName = "my win";

	SDL_Window* win = SDL_CreateWindow(winName, WIN_PARS, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (win == NULL)
	{
		printf("error in win");
		return 0;
	}

	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GLContext glCtx;

	glCtx = SDL_GL_CreateContext(win);


	if (glCtx == NULL)
	{
		printf("error in glCtx");
		return 0;
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint bufPosId;

	glGenBuffers(1, &bufPosId);

	glBindBuffer(GL_ARRAY_BUFFER, bufPosId);

	float pos[] = { -0.5f, -0.5f,
			  0.5f, -0.5f,
			  0.5f, 0.5f,
			  -0.5f, 0.5f };


	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), pos, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2,0);


	uint indices[] = { 0, 1, 2,
			  3, 2, 0};

	uint bufIndId;

	glGenBuffers(1, &bufIndId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIndId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint), indices, GL_STATIC_DRAW);

	char **shaderFile = ParseShader("C:/Users/pc/Desktop/SDL_Projects/SDL2_NewProject/sources/SHaders/Basic.shader");
	if (shaderFile == NULL)
	{
		printf("er");
		return -1;
	}

	//printf("%s\n%s",shaderFile[0], shaderFile[1]);

		//ClearError();
	uint sh = CreateShader(shaderFile[0], shaderFile[1]);

	glUseProgram(sh);
		//GetError();

	glUniform4f(glGetUniformLocation(sh, "u_Color"), 1.0f, 0.2f, 0.7f, 1.0f);

	FreeShaderParserHeap(shaderFile);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;

		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		SDL_GL_SwapWindow(win);
	}

	return 0;
}
