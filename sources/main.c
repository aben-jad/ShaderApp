#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <mystructs.h>
#include <mymath.h>
#include <myglobals.h>
#include <mymacros.h>
#include <fileutils.h>
#include <shaderutils.h>
#include <configutils.h>
#include <uiutils.h>


vec3 zoom(vec3 _v, float _dm, float _min, float _max)
{
	float m = dist(_v);
	vec3 n = SCA((1.0f/m), _v);

	float nm = fmaxf(m+_dm, _min);
	nm = fminf(nm, _max);
	vec3 res = SCA(nm, n);

	return res;
}

int mouse_on_viewport(vec3 _mousePos, rect _vp)
{
	return (_mousePos.x > _vp.x && _mousePos.x < (_vp.x + _vp.w) && _mousePos.y > _vp.y && _mousePos.y < (_vp.y + _vp.h));
}

void pass_ui_uniforms(GLint* _uni)
{
	glUniform1f(_uni[g_ui_sliders.hovered], g_ui_sliders.current_value[g_ui_sliders.hovered]);
}

GLint* get_config_uniform_locations(GLuint _shaderId)
{
	int length = g_ui_sliders.len;
	GLint* res = (GLint*)malloc(length * sizeof(GLint));

	for (int i = 0; i < length; i++)
	{
		res[i] = glGetUniformLocation(_shaderId, g_ui_sliders.name[i]);
	}
	return res;
}

char* get_ui_titles()
{
	int ui_len = g_ui_sliders.len;

	int final_str_len = ui_len ;

	int strs_len[ui_len];

	for (int i = 0; i < ui_len; i++)
	{
		strs_len[i] = strlen(g_ui_sliders.name[i]);
		final_str_len += strs_len[i];
	}

	char* res = (char*)malloc(final_str_len * sizeof(char));
	int cur_index = 0;
	for (int i = 0; i < ui_len; i++)
	{
		strncpy(res + cur_index, g_ui_sliders.name[i], strs_len[i]);
		cur_index += strs_len[i];
		res[cur_index++] = '\n';
		//res[cur_index++] = '\n';
	}

	res[cur_index] = 0;
	return res;
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);


	char* shaderPath = NULL;

	int winNormalWidth = 1000, winHeight = 1000, winConfigWidth = 1500;

	rect main_viewport = { 0, 0, winNormalWidth, winHeight};
	rect ui_viewport = { winNormalWidth, 0, winConfigWidth-winNormalWidth, winHeight};

	SDL_Window* win = SDL_CreateWindow("mywindow", winNormalWidth, winHeight, SDL_WINDOW_OPENGL);

	if (win == NULL)
	{
		printf("error in win");
		return 0;
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	//uint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	float main_vertices2[] = { -1.0f, -1.0f, 0.0f, 0.0f, 
			  -1.0f, 1.0f, 0.0, 1.0f,
			  1.0f, 1.0f, 1.0f, 1.0f,
			  1.0f, -1.0f, 1.0f, 0.0f };

	float main_vertices[] = { -1.0f, -1.0f,
			  -1.0f, 1.0f,
			  1.1f, 1.0f,
			  1.1f, -1.0f };


	float main_tex_uv[] = { -1.0f, -2.0f,
			  -1.0f, 3.0f,
			  2.0f, 3.0f,
			  2.0f, -2.0f };



	unsigned int indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};

	uint main_buf_verts;
	glGenBuffers(1, &main_buf_verts);
	glBindBuffer(GL_ARRAY_BUFFER, main_buf_verts);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), main_vertices, GL_STATIC_DRAW);

	uint indices_buf;
	glGenBuffers(1, &indices_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2,0);
	glEnableVertexAttribArray(0);

	uint main_buf_uvs;
	glGenBuffers(1, &main_buf_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, main_buf_uvs);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), main_tex_uv, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//uint ui_buf_verts;
	//glGenBuffers(1, &ui_buf_verts);
	//glBindBuffer(GL_ARRAY_BUFFER, ui_buf_verts);
	//glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), ui_vertices, GL_STATIC_DRAW);


	if (argc == 2)
	{
		g_project_path = argv[1];
		if (!shader_file_exist())
		{
			printf("shader file not found\n");
			return 1;
		}
		if (!config_file_exist())
		{
			printf("config file not found\n");
			return 1;
		}
	}

	else
	{
		printf("enter exact args\n");
		return 1;
	}





	GLuint ui_shader_program = generate_ui_program();
	glUseProgram(ui_shader_program);
	init_ui();

	GLuint main_shader_program = generate_main_program();
	glUseProgram(main_shader_program);
	GLint* config_1f_uniforms = get_config_uniform_locations(main_shader_program);




	//drawing text

	if (!TTF_Init())
	{
        	printf("Couldn't initialise SDL_ttf\n");
        	return 1;
	}

	TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 1024);
	
	if (!(font))
	{
		printf("Failed to load font\n");
		return 1;
	}

	SDL_Color text_color = {255, 255, 255, 255};
	char* ui_text = get_ui_titles();
	SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, ui_text, 0, text_color, 0);
	SDL_Surface* formatted = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(textSurface);
	textSurface = formatted;
	SDL_FlipSurface(textSurface, SDL_FLIP_VERTICAL);
	GLuint textTexture;
	glGenTextures(1, &textTexture);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, textSurface->pixels);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBindTexture(GL_TEXTURE_2D, textTexture);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	GLint textute_location = glGetUniformLocation(main_shader_program, "u_text");
	glUniform1i(textute_location, 0);

	SDL_DestroySurface(textSurface);

	//end
	



	// Once finished with OpenGL functions, the SDL_GLContext can be destroyed.

	bool focus_lost = false;
	bool config_shown = false;
	bool ui_element_selected = false;
	const float minDist = 2.0f;
	const float maxDist = 18.0f;
	const float rotation_speed = 2.0f;
	const float small_rot = 0.05f;

	const vec3 light1Pos = { 0.0f, 5.0f, 0.0f };
	const vec3 light2Pos = { 4.0f, -5.0f, 4.0f };
	const vec3 light3Pos = { -4.0f, -5.0f, -4.0f };

	vec3 camPos = {	 0, 0 , 10};
	vec3 screenPos = { 0, 0 , 9};

	vec3 camRight = { 1, 0, 0 };
	vec3 camUp = { 0, 1, 0 };

	unsigned int timmer = 0;


	GLint light1Location = glGetUniformLocation(main_shader_program, "u_light1Pos");
	GLint light2Location = glGetUniformLocation(main_shader_program, "u_light2Pos");
	GLint light3Location = glGetUniformLocation(main_shader_program, "u_light3Pos");

	GLint camLocation = glGetUniformLocation(main_shader_program, "u_camPos");
	GLint screenLocation = glGetUniformLocation(main_shader_program, "u_screenPos");

	GLint upLocation = glGetUniformLocation(main_shader_program, "u_up");
	GLint rightLocation = glGetUniformLocation(main_shader_program, "u_right");

	glUseProgram(main_shader_program);
	//glBindVertexArray(vao);

	glUniform3f(camLocation, camPos.x, camPos.y, camPos.z);
	glUniform3f(screenLocation, screenPos.x, screenPos.y, screenPos.z);

	glUniform3f(light1Location, light1Pos.x, light1Pos.y, light1Pos.z);
	glUniform3f(light2Location, light2Pos.x, light2Pos.y, light2Pos.z);
	glUniform3f(light3Location, light3Pos.x, light3Pos.y, light3Pos.z);

	glUniform3f(upLocation, camUp.x, camUp.y, camUp.z);
	glUniform3f(rightLocation, camRight.x, camRight.y, camRight.z);


	vec3 currentRawMousePos = {0.0f, 0.0f, 0.0f};
	vec3 currentMainMousePos = {0.0f, 0.0f, 0.0f};
	vec3 currentUIMousePos = {0.0f, 0.0f, 0.0f};
	SDL_GetMouseState(&currentRawMousePos.x, &currentRawMousePos.y);
	currentMainMousePos.x = 2.0f * currentRawMousePos.x / (float)winNormalWidth - 1.0f; 
	currentMainMousePos.y = 1.0f - 2.0f * currentRawMousePos.y / (float)winHeight;
	vec3 oldRawMousePos = currentRawMousePos;
	vec3 oldMainMousePos = currentMainMousePos;
	//vec3 oldUIMousePos = currentUIMousePos;


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

		if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL] == true)
		{
			config_shown = !config_shown;
			if (config_shown)
				SDL_SetWindowSize(win, winConfigWidth, winHeight);
			else
				SDL_SetWindowSize(win, winNormalWidth, winHeight);
		}

		SDL_GetMouseState(&currentRawMousePos.x, &currentRawMousePos.y);
		currentMainMousePos.x = 2.0f * currentRawMousePos.x / (float)winNormalWidth - 1.0f; 
		currentMainMousePos.y = 1.0f - 2.0f * currentRawMousePos.y / (float)winHeight;

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
			main_shader_program = generate_main_program();
			glUseProgram(main_shader_program);

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
				float dx = rotation_speed * (-currentMainMousePos.x+oldMainMousePos.x);
				float dy = rotation_speed * (currentMainMousePos.y-oldMainMousePos.y);

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
		if (config_shown)
		{
			glViewport(ui_viewport.x, ui_viewport.y, ui_viewport.w, ui_viewport.h);
			glUseProgram(ui_shader_program);
			if (mouse_on_viewport(currentRawMousePos, ui_viewport))
			{
				currentUIMousePos.x = (currentRawMousePos.x - main_viewport.w)/ui_viewport.w - 0.5f;
				currentUIMousePos.y = 1.0f - 2.0f * currentRawMousePos.y/ui_viewport.h;
		 		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == 1 && g_ui_sliders.hovered > -1)
					ui_element_selected = true;
		 		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == 1)
					ui_element_selected = false;
				if (ui_element_selected)
					manage_selected_ui(currentUIMousePos);
				else
					manage_hovered_ui(currentUIMousePos);
			}

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
			glViewport(main_viewport.x, main_viewport.y, main_viewport.w, main_viewport.h);
			glUseProgram(main_shader_program);
			if (ui_element_selected)
				pass_ui_uniforms(config_1f_uniforms);
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow(win);

		timmer = SDL_GetTicks() % 10;

		if (timmer == 0)
			oldMainMousePos = currentMainMousePos;

	}
	SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
