#include <stdio.h>
#include <math.h>
#include "myincludes.h"
#include <glad/glad.h>
#include <SDL3/SDL.h>

#define uint unsigned int

#define ADD(a, b) (vec3){ a.x + b.x, a.y + b.y , a.z + b.z }
#define SCA(s, a) (vec3){ s * a.x, s * a.y , s * a.z }
#define SIGN(s) (s < 0) ? -1.0f : 1.0f

	const char *vertexShaderSource = "#version 330 core\n"
	    "layout (location = 0) in vec4 pos;\n"
	    "out vec2 uv;\n"
	    "void main() {\n"
	    "    gl_Position = pos;\n"
	    "    uv = pos.xy;\n"
	    "}";
	
	const char *fragmentShaderSourceDefault = "#version 330 core\n"
	    "out vec4 FragColor;\n"
	    "void main() {\n"
	    "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
	    "}";

typedef struct
{
	float x, y, z;
} vec3;

typedef struct
{
	float w, x, y, z;
} quaternion;

quaternion quaternion_multiplication(quaternion _a, quaternion _b)
{
	quaternion q;

	///  (_a.w + _a.x * i + _a.y * j + _a.z * k) * (_b.w + _b.x * i + _b.y * j + _b.z * k)

	///  _a.w * _b.w + _a.w * _b.x * i + _a.w * _b.y * j + _a.w * _b.z * k 
	///  _a.x * i * _b.w + _a.x * i * _b.x * i + _a.x * i * _b.y * j + _a.x * i * _b.z * k = 
	///  _a.y * j * _b.w + _a.y * j * _b.x * i + _a.y * j * _b.y * j + _a.y * j * _b.z * k
	///  _a.z * k * _b.w + _a.z * k * _b.x * i + _a.z * k * _b.y * j + _a.z * k * _b.z * k


	///  _a.w * _b.w + _a.w * _b.x * i + _a.w * _b.y * j + _a.w * _b.z * k 
	///  _a.x * i * _b.w - _a.x * _b.x + _a.x  * _b.y * k - _a.x * i * _b.z * j 
	///  _a.y * j * _b.w - _a.y * _b.x * k - _a.y * _b.y + _a.y * _b.z * i
	///  _a.z * k * _b.w + _a.z *  _b.x * j - _a.z * _b.y * i - _a.z * _b.z

	///  ( _a.w * _b.w      ,    _a.w * _b.x    ,    _a.w * _b.y     ,    _a.w * _b.z  )
	///  ( -_a.x * _b.x     ,    _a.x * _b.w    ,    -_a.x * _b.z    ,     _a.x * _b.y )
	///  ( -_a.y * _b.y     ,    _a.y * _b.z    ,    _a.y  * _b.w    ,    -_a.y * _b.x )
	///  ( -_a.z * _b.z     ,    -_a.z * _b.y   ,    _a.z *  _b.x    ,    _a.z * _b.w  )

	q.w = _a.w * _b.w - _a.x * _b.x - _a.y * _b.y - _a.z * _b.z;
	q.x = _a.w * _b.x + _a.x * _b.w + _a.y * _b.z - _a.z * _b.y;
	q.y = _a.w * _b.y - _a.x * _b.z + _a.y * _b.w + _a.z * _b.x;
	q.z = _a.w * _b.z + _a.x * _b.y - _a.y * _b.x + _a.z * _b.w;

	return q;
}

float dot(vec3 _a, vec3 _b)
{
	return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
}



vec3 quaternion_rotation(vec3 _p, float _ang, vec3 _axis)
{
	float c = cos(_ang * 0.5f);
	float s = sin(_ang * 0.5f);

	quaternion p  = { 0, _p.x, _p.y, _p.z };
	quaternion q  = { c, s * _axis.x, s * _axis.y, s * _axis.z };
	quaternion qi = { c, -1 * s * _axis.x, -1 * s * _axis.y, -1 * s * _axis.z };

	quaternion qp   = quaternion_multiplication(q, p);
	quaternion qpqi = quaternion_multiplication(qp, qi);

	vec3 res = { qpqi.x, qpqi.y, qpqi.z };

	return res;
}

vec3 cross(vec3 _a, vec3 _b)
{
	float s1 = _a.y*_b.z - _a.z*_b.y;
	float s2 = _a.z*_b.x - _b.z*_a.x;
	float s3 = _a.x*_b.y - _b.x*_a.y;

	vec3 res = {s1, s2, s3};

	return res;
}

float dist(vec3 _a)
{
	return sqrt(dot(_a, _a));
}

vec3 normalize(vec3 _a)
{
	float lenght = dist(_a);
	vec3 res = {_a.x / lenght, _a.y / lenght, _a.z / lenght};
	return res;
}

vec3 zoom(vec3 _v, float _dm, float _min, float _max)
{
	float m = dist(_v);
	vec3 n = SCA((1.0f/m), _v);

	float nm = fmaxf(m+_dm, _min);
	nm = fminf(nm, _max);
	vec3 res = SCA(nm, n);

	return res;
}

float compute_angle(vec3 _a, vec3 _b, vec3 _N)
{
	float ab = dist(_a) * dist(_b);
	//if (ab == 0.0f)
	//	printf("0 err\n");
	float c = dot(_a, _b) / ab;
	if (c > 1.0f)
		c = 1.0f;
	else if (c < -1.0f)
		c = -1.0f;
		//printf("%f\n", c);
	//float s = dist(cross(_a, _b)) / ab;
	//vec3 N = {0.0f, 1.0f, 0.0f};
	vec3 cr = cross(_a, _b);
	float dt = dot(cr, _N);
	float s = SIGN(dt);
	//printf("%f\n", dt);
	return acos(c) * s;
}

int reload_shader(char* _path)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSource = read_file(_path);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	free(fragmentShaderSource);

	int res;
	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE)
	{
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceDefault, NULL);
		glCompileShader(fragmentShader);
	}


	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);


	char *winName = "my win";
	char* shaderPath = NULL;

	int winWidth = 1000, winHeight = 1000;

	SDL_Window* win = SDL_CreateWindow(winName, winWidth, winHeight, SDL_WINDOW_OPENGL);

	if (win == NULL)
	{
		printf("error in win");
		return 0;
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	

	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint bufPosId;
	glGenBuffers(1, &bufPosId);
	glBindBuffer(GL_ARRAY_BUFFER, bufPosId);

	float vertices[] = { -1.0f, -1.0f,
			  -1.0f, 1.0f,
			  1.0f, 1.0f,
			  1.0f, -1.0f };
			  //1.0f, 1.0f,
			  //1.0f, -1.0f };
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);

	unsigned int indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};

	uint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2,0);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// now you can make GL calls.
	//

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (argc == 2)
	{
		shaderPath = argv[1];
		const char* fragmentShaderSource = read_file(shaderPath);

		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		free(fragmentShaderSource);

		int res;
		glGetShaderiv(fragmentShader,GL_COMPILE_STATUS, &res);

		if (res == GL_FALSE)
		{
			glShaderSource(fragmentShader, 1, &fragmentShaderSourceDefault, NULL);
			glCompileShader(fragmentShader);
		}
	}

	else
	{
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceDefault, NULL);
		glCompileShader(fragmentShader);
	}




	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	// Once finished with OpenGL functions, the SDL_GLContext can be destroyed.

	bool focus_lost = false;
	const float minDist = 2.0f;
	const float maxDist = 18.0f;
	const float rotation_speed = 3.0f;

	const vec3 light1Pos = { 0.0f, 5.0f, 0.0f };
	const vec3 light2Pos = { 4.0f, -5.0f, 4.0f };
	const vec3 light3Pos = { -4.0f, -5.0f, -4.0f };

	vec3 camPos = {	 0, 0 , 10};
	vec3 screenPos = { 0, 0 , 9};

	vec3 camRight = { 1, 0, 0 };
	vec3 camUp = { 0, 1, 0 };

	unsigned int timmer = 0;


	GLint light1Location = glGetUniformLocation(shaderProgram, "u_light1Pos");
	GLint light2Location = glGetUniformLocation(shaderProgram, "u_light2Pos");
	GLint light3Location = glGetUniformLocation(shaderProgram, "u_light3Pos");

	GLint camLocation = glGetUniformLocation(shaderProgram, "u_camPos");
	GLint screenLocation = glGetUniformLocation(shaderProgram, "u_screenPos");

	GLint upLocation = glGetUniformLocation(shaderProgram, "u_up");
	GLint rightLocation = glGetUniformLocation(shaderProgram, "u_right");

	glUseProgram(shaderProgram);
	glBindVertexArray(vao);

	glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
	glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);

	glUniform3f(light1Location, light1Pos.x, light1Pos.y, light1Pos.z);
	glUniform3f(light2Location, light2Pos.x, light2Pos.y, light2Pos.z);
	glUniform3f(light3Location, light3Pos.x, light3Pos.y, light3Pos.z);

	glUniform3f(upLocation, camUp.x, camUp.y, camUp.z);
	glUniform3f(rightLocation, camRight.x, camRight.y, camRight.z);


	vec3 currentMousePos = {0.0f, 0.0f, 0.0f};
	SDL_GetMouseState(&currentMousePos.x, &currentMousePos.y);
	currentMousePos.x = 2.0f * currentMousePos.x / (float)winWidth - 1.0f; 
	currentMousePos.y = 1.0f - 2.0f * currentMousePos.y / (float)winHeight;
	vec3 oldMousePos = currentMousePos;


	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_LOST)
			focus_lost = true;
		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
			focus_lost = false;

		if (focus_lost)
			continue;

		SDL_GetMouseState(&currentMousePos.x, &currentMousePos.y);
		currentMousePos.x = 2.0f * currentMousePos.x / (float)winWidth - 1.0f; 
		currentMousePos.y = 1.0f - 2.0f * currentMousePos.y / (float)winHeight;

		if (event.type == SDL_EVENT_QUIT)
			break;

		if (event.type == SDL_EVENT_MOUSE_WHEEL)
		{
			float dm = event.wheel.y * 0.1f;

			camPos = zoom(camPos, dm, minDist, maxDist);
			screenPos = zoom(screenPos, dm, minDist-1, maxDist-1);

			glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
			glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);
		}
		else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_R)
		{
			camPos = (vec3){ 0, 0 , 10};
	 		screenPos = (vec3){ 0, 0 , 9};

	 		camRight = (vec3){ 1, 0, 0 };
	 		camUp = (vec3){ 0, 1, 0 };

			glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
			glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);

			glUniform3f(light1Location, light1Pos.x, light1Pos.y, light1Pos.z);
			glUniform3f(light2Location, light2Pos.x, light2Pos.y, light2Pos.z);
			glUniform3f(light3Location, light3Pos.x, light3Pos.y, light3Pos.z);

			glUniform3f(upLocation, camUp.x, camUp.y, camUp.z);
			glUniform3f(rightLocation, camRight.x, camRight.y, camRight.z);
		}
		else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_S)
		{
			shaderProgram = reload_shader(shaderPath);
			glUseProgram(shaderProgram);

			glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
			glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);

			glUniform3f(light1Location, light1Pos.x, light1Pos.y, light1Pos.z);
			glUniform3f(light2Location, light2Pos.x, light2Pos.y, light2Pos.z);
			glUniform3f(light3Location, light3Pos.x, light3Pos.y, light3Pos.z);

			glUniform3f(upLocation, camUp.x, camUp.y, camUp.z);
			glUniform3f(rightLocation, camRight.x, camRight.y, camRight.z);
		}
		else if (event.type == SDL_EVENT_MOUSE_MOTION)
		{
			float newMouse_x, newMouse_y;
			if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT)
			{
				float dx = (-currentMousePos.x+oldMousePos.x) * rotation_speed;
				float dy = (currentMousePos.y-oldMousePos.y) * rotation_speed;
				
				camPos = quaternion_rotation(camPos, dx,camUp);
				screenPos = quaternion_rotation(screenPos, dx,camUp);
				camRight = quaternion_rotation(camRight, dx, camUp);
                        
				camPos = quaternion_rotation(camPos, dy,camRight);
				screenPos = quaternion_rotation(screenPos, dy,camRight);
				camUp = quaternion_rotation(camUp, dy, camRight);

				glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
				glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);

				glUniform3f(upLocation, camUp.x, camUp.y, camUp.z);
				glUniform3f(rightLocation, camRight.x, camRight.y, camRight.z);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow(win);

		timmer = SDL_GetTicks() % 10;

		if (timmer == 0)
			oldMousePos = currentMousePos;

	}
	SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
