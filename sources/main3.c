#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <mystructs.h>
#include <mymath.h>
#include <fileutils.h>
#include <sdffont.h>

const char *v_sh = "#version 430 core\n"
	    "layout (location = 0) in vec4 pos;\n"
	    "layout (location = 1) in vec2 in_t_uv;\n"
	    "out vec2 uv;\n"
	    "out vec2 tex_uv;\n"
	    "void main() {\n"
	    "    gl_Position = pos;\n"
	    "    uv = pos.xy;\n"
	    "	 tex_uv = in_t_uv;\n"
	    "}";
	
//const char *f_sh = "#version 430 core\n"
//		"layout(std430, binding=0) buffer CellBuffer {\n"
//    		"uint cellData[];\n" 
//		"};\n"
//	    "out vec4 FragColor;\n"
//	    "in vec2 uv;\n"
//	    "in vec2 tex_uv;\n"
//	    "struct bez_pts {\n"
//	    "	vec2 a, c, b;\n"
//	    "};\n"
//	    //"uniform sampler2D u_text;\n"
//	    "uniform float u_size;\n"
//	    "uniform vec2 u_cursor;\n"
//	    //"subroutine float char_sdf(vec2);\n"
//	    //"subroutine uniform char_sdf u_sub;\n"
//	    //"subroutine (char_sdf) float sda(vec2 _p) {\n"
//	    //"	 return abs(length(_p) - .3) - .05;\n"
//	    //"}\n"
//	    //"subroutine (char_sdf)float sdb(vec2 _p) {\n"
//	    //"	 _p = abs(_p) - vec2(.5)\n;"
//	    //"    vec2 n_p = max(_p, vec2(0.));\n"
//	    //"    float d = length(n_p) + min(0, max(_p.x, _p.y));\n"
//	    //"	 return d;\n"
//	    //"}\n"
//	    //"subroutine (char_sdf) float sdc(vec2 _p) {\n"
//	    //"	 return length(_p) - .5;\n"
//	    //"}\n"
//	    "float dot2(vec2 _p)\n"
//	    "{\n"
//	    "	return dot(_p, _p);\n"
//	    "}\n"
//	    "float sdCircle(vec2 _p, float _r) {\n"
//	    "	 return length(_p) - _r;\n"
//	    "}\n"
//	    "bez_pts get_spline_points(int _index) {\n"
//	    "	float px1 = float(cellData[_index]) / 1024.0;\n"
//	    "	float py1 = float(cellData[_index + 1]) / 1024.0;\n"
//	    "	float px2 = float(cellData[_index + 2]) / 1024.0;\n"
//	    "	float py2 = float(cellData[_index + 3]) / 1024.0;\n"
//	    "	float px3 = float(cellData[_index + 4]) / 1024.0;\n"
//	    "	float py3 = float(cellData[_index + 5]) / 1024.0;\n"
//	    "	return bez_pts(vec2(px1, py1), vec2(px2, py2), vec2(px3, py3));\n"
//	    "}\n"
//	"float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C )\n"
//	"{    \n"
//	"vec2 a = B - A;\n"
//	"vec2 b = A - 2.0*B + C;\n"
//	"vec2 c = a * 2.0;\n"
//	"vec2 d = A - pos;\n"
//	"float kk = 1.0/dot(b,b);\n"
//	"float kx = kk * dot(a,b);\n"
//	"float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;\n"
//	"float kz = kk * dot(d,a);      \n"
//	"float res = 0.0;\n"
//	"float p = ky - kx*kx;\n"
//	"float p3 = p*p*p;\n"
//	"float q = kx*(2.0*kx*kx-3.0*ky) + kz;\n"
//	"float h = q*q + 4.0*p3;\n"
//	"if( h >= 0.0) \n"
//	"{ \n"
//	"	h = sqrt(h);\n"
//	"	vec2 x = (vec2(h,-h)-q)/2.0;\n"
//	"	vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));\n"
//	"	float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );\n"
//	"	res = dot2(d + (c + b*t)*t);\n"
//	"}\n"
//	"else\n"
//	"{\n"
//	"	float z = sqrt(-p);\n"
//	"	float v = acos( q/(p*z*2.0) ) / 3.0;\n"
//	"	float m = cos(v);\n"
//	"	float n = sin(v)*1.732050808;\n"
//	"	vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);\n"
//	"	res = min( dot2(d+(c+b*t.x)*t.x),\n"
//	"               dot2(d+(c+b*t.y)*t.y) );\n"
//	"}\n"
//	"return sqrt( res );\n"
//	"}\n"
//	    "float sdBox(vec2 _p, vec2 _r) {\n"
//	    "	 _p = abs(_p) - _r\n;"
//	    "    vec2 n_p = max(_p, vec2(0.));\n"
//	    "    float d = length(n_p) + min(0, max(_p.x, _p.y));\n"
//	    "	 return d;\n"
//	    "}\n"
//	    "float sdSeg(vec2 _p, vec2 _a, vec2 _b) {\n"
//		"	vec2 lv = _b-_a;\n"
//		"	vec2 pv = _p-_a;\n"
//		"	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);\n"
//		"	return length(_p-(_a + dt*(_b-_a)));\n"
//	    "}\n"
//	    "float construct_sdf(int _index) {\n"
//	    "	if (cellData[_index] == 0)	return 100;\n"
//	    "	bez_pts bez_p1 = get_spline_points(_index + 1);\n"
//	    "	bez_pts bez_p2 = get_spline_points(_index + 7);\n"
//	    "	bez_pts bez_p3 = get_spline_points(_index + 13);\n"
//	    "	float d1 = sdBezier(fract(tex_uv*u_size) ,bez_p1.a, bez_p1.c, bez_p1.b);\n"
//	    "	float d2 = sdBezier(fract(tex_uv*u_size) ,bez_p2.a, bez_p2.c, bez_p2.b);\n"
//	    "	float d3 = sdBezier(fract(tex_uv*u_size) ,bez_p3.a, bez_p3.c, bez_p3.b);\n"
//	    "	return min(min(d1, d2), d3);\n"
//	    "}\n"
//	    "void main() {\n"
//	    "	 int ns = int(u_size);\n"
//	    "	 ivec2 nuv = ivec2(int(tex_uv.x * u_size), int(tex_uv.y * u_size));\n"
//	    "	 int ind =  nuv.x + nuv.y * ns;\n"
//	    "	 float sd_bez_a = smoothstep(0.1, -0.05, construct_sdf(ind * 19));\n"
//	    "    vec3 ssbo = vec3(sd_bez_a, 0., 0.);\n"
//	    "    FragColor = vec4(ssbo, 1.);\n"
//	    "}";
#define uint unsigned int
int get_program()
{
	char* frag = read_file("C:\\Users\\adnane\\Desktop\\C-Projects\\ShaderApp\\sources\\Shaders\\frag.shader");
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &v_sh, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&frag, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	free(frag);
	return shaderProgram;
}

int virtual_grid(int _x, int _y, int _u)
{
	int l_s = _x % _u;
	int r_s = _u - l_s;

	int u_s = _y % _u;
	int d_s = _u - u_s;

	int lr_d = (l_s < r_s) ? l_s : r_s;
	int ud_d = (u_s < d_s) ? u_s : d_s;

	return (lr_d < ud_d) ? lr_d : ud_d;
}

void construct_i64(void* _p, unsigned int _a1, unsigned int _a2, unsigned int _a3, unsigned int _a4, unsigned int _a5, unsigned int _a6)
{
	unsigned int a = _a1 & 0x3FF; // Mask to 10 bits
	unsigned int b = _a2 & 0x3FF;
    	unsigned int c = _a3 & 0x3FF;

	unsigned int packed = (c << 20) | (b << 10) | a;
	((unsigned int*)_p)[0] = packed;
	//int cur = 0;
	//int B8 = ipow(2, 10) -1;
	//int B10 = ipow(2, 10) -1;
	//int B20 = ipow(2, 20) -1;
	//int B30 = ipow(2, 30) -1;
	//int B32 = -1;

	//int a1 = _a1;
	//int a2 = (_a2 << 10) & (B20-B10);
	//int a3 = (_a3 << 20) & (B30-(B20+B10));
	//int a4 = ((_a4 & 3) << 30);
	//int a42 = (_a4 >> 2) & B8;
	//int a5 = (_a5 << 12);
	//int a6 = (_a6 << 22);

	//((int*)_p)[0] = a1 | a2 | a3 | a4;
	//((int*)_p)[1] = a42 | a5 | a6;
}


void print_i64(void* _p)
{
	int B8 = ipow(2, 8) -1;
	int B10 = ipow(2, 10) -1;
	int B20 = ipow(2, 20) -1;
	int B30 = ipow(2, 30) -1;
	int B32 = -1;

	int x = *(int*)_p;
	int y = ((int*)_p)[1];

	int a = x & B10;
	int b = (x >> 10) & B10;
	int c = (x >> 20) & B10;
	int d = ((x >> 30) & 3) | ((y & B8) << 2);
	int e = (y >> 12) &B10;
	int f = (y >> 22) &B10;


	printf("%d %d %d %d %d %d\n", a, b ,c, d, e, f);
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);


	int winNormalWidth = 1024, winHeight = 1024;
	bool focus_lost = false;
	float cur_x = 0, cur_y = 0;
	float size = 4.0f;
	int grid_size = (int)size * (int)size;

	int cellule_x = winNormalWidth / (int)size;
	int cellule_y = winHeight / (int)size;

	SDL_Window* win = SDL_CreateWindow("mywindow", winNormalWidth, winHeight, SDL_WINDOW_OPENGL);

	if (win == NULL)
	{
		printf("error in win");
		return 0;
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	float main_vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, 
			  -1.0f, 1.0f, 0.0, 0.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  1.0f, -1.0f, 1.0f, 1.0f };



	unsigned int indices[] = {
	    0, 1, 2,  // First triangle
	    0, 2, 3   // Second triangle
	};

	uint main_buf_verts;
	glGenBuffers(1, &main_buf_verts);
	glBindBuffer(GL_ARRAY_BUFFER, main_buf_verts);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), main_vertices, GL_STATIC_DRAW);

	uint indices_buf;
	glGenBuffers(1, &indices_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	char hola[4] = {'h', 'o', 'l', 'a'};
	int cellCur = 0;
	int cel_Size = 19;
	unsigned int* cellData = calloc(cel_Size * grid_size, sizeof(unsigned int));
	for(int i = 0; i < grid_size; i++)
	{
		if (i > 3)	break;
		unsigned int* splines = sdf_params_char(hola[i]);
		cellData[0 + i*cel_Size] = 1;
		for (int j = 0; j < 18; j++)
			cellData[j +1 + i*cel_Size] = splines[j];
		
		free(splines);
	}


	
	// Allocate and upload data (36 floats)
	glBufferData(GL_SHADER_STORAGE_BUFFER, cel_Size * grid_size * sizeof(unsigned int), cellData, GL_DYNAMIC_COPY);
	
	// Bind the SSBO to a binding point (e.g., 0)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	
	// Unbind to avoid accidental modification
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint main_shader_program = get_program();
	glUseProgram(main_shader_program);

	//unsigned int read_data[2];
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(read_data), read_data);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//print_i64(read_data);
	//printf("%d %d\n", read_data[0], read_data[1]);

	//printf("After compute: a=%u, b=%u, c=%u\n", 
	//read_data[0] & 0x3FF, 
       	//(read_data[0] >> 10) & 0x3FF, 
       	//(read_data[0] >> 20) & 0x3FF);




	//drawing text

	//if (!TTF_Init())
	//{
        //	printf("Couldn't initialise SDL_ttf\n");
        //	return 1;
	//}

	//TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 512);
	//
	//if (!(font))
	//{
	//	printf("Failed to load font\n");
	//	return 1;
	//}

	//SDL_Color text_color = {255, 0, 0, 255};
	////SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, "hello BIGDADY \nthis is a text rendered with \nOpenGL and Shaders \nCan You do that ?", 0, text_color, 0);
	//SDL_Surface *textSurface = TTF_RenderText_Blended(font, "hello", 0, text_color);
	//SDL_Surface* formatted = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
	//SDL_DestroySurface(textSurface);
	//textSurface = formatted;
	//SDL_FlipSurface(textSurface, SDL_FLIP_VERTICAL);

	int len = winNormalWidth * winHeight;
	//float myTexture[len];
	float* myTexture = (float*)malloc(len * sizeof(float));

	//const unsigned int rt = 255 / winNormalWidth;

	for(int i = 0; i < len; i++)
	{

		int x = i % winNormalWidth;
		int y = i / winHeight;
		int c_x = x / cellule_x;
		int c_y = y / cellule_y;
		float c_uv_x = (float)(x % cellule_x) / cellule_x;
		float c_uv_y = (float)(y % cellule_y) / cellule_y;
		int YM = (len-1) / winHeight;
		int j = x + (YM-y) * winHeight;
		int index = c_x + c_y * size;
		float nx = (((float)x / (float)winNormalWidth) * 2.0f -1.0f) ;
		float ny = ((float)y / (float)winHeight) * 2.0f -1.0f;
		vec3 uvs = {nx * size * 2, ny * size * 2, 0.0f};
		vec3 c_uvs = {c_uv_x *2.0 -1.0, 1-c_uv_y *2.0, 0.0f};
		if (index < 4)
			myTexture[j] = sdf_char(hola[index], c_uvs);
		else
			myTexture[j] = 1;
	}

	GLuint textTexture;
	glGenTextures(1, &textTexture);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, winNormalWidth, winHeight, 0,
	             GL_RED, GL_FLOAT, myTexture);
	free(myTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0,
	//             GL_RGBA, GL_UNSIGNED_BYTE, textSurface->pixels);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	GLint textute_location = glGetUniformLocation(main_shader_program, "u_text");
	glUniform1i(textute_location, 0);

	GLint size_location = glGetUniformLocation(main_shader_program, "u_size");
	GLint cursor_location = glGetUniformLocation(main_shader_program, "u_cursor");
	glUniform1f(size_location, size);
	glUniform2f(cursor_location, cur_x, cur_y);

	GLint subLocation = glGetSubroutineUniformLocation(main_shader_program, GL_FRAGMENT_SHADER, "u_sub");
	

	GLuint sub_indices[3];
	sub_indices[0] = glGetSubroutineIndex(main_shader_program, GL_FRAGMENT_SHADER, "sda");
	sub_indices[1] = glGetSubroutineIndex(main_shader_program, GL_FRAGMENT_SHADER, "sdc");
	sub_indices[2] = glGetSubroutineIndex(main_shader_program, GL_FRAGMENT_SHADER, "sdb");
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &sub_indices[2]);

	//SDL_DestroySurface(textSurface);

	//end
    // Generate the SSBO
	
	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.type == SDL_EVENT_QUIT)
			break;


		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_LOST)
			focus_lost = true;
		if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
			focus_lost = false;

		if (focus_lost)
			continue;

		if (event.type == SDL_EVENT_KEY_DOWN)
		{
			//printf("%x\n", event.key.key);
			//printf("%x\n", 0x0000002bu);

			switch (event.key.key)
			{
				case SDLK_UP:
					cur_y++;
					glUniform2f(cursor_location, cur_x, cur_y);
					break;
				case SDLK_DOWN:
					cur_y--;
					glUniform2f(cursor_location, cur_x, cur_y);
					break;
				case SDLK_RIGHT:
					cur_x++;
					glUniform2f(cursor_location, cur_x, cur_y);
					break;
				case SDLK_LEFT:
					cur_x--;
					glUniform2f(cursor_location, cur_x, cur_y);
					break;
				case SDLK_KP_PLUS:
					if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
					{
						size+=2;
						glUniform1f(size_location, size);
					}
					break;
				case SDLK_KP_MINUS:
					if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL])
					{
						size-=2;
						glUniform1f(size_location, size);
					}
					break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow(win);

	}
	SDL_GL_DestroyContext(glcontext);  

	return 0;
	
}
