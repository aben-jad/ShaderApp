#include <glad/glad.h>
#include <myglobals.h>
#include <mystructs.h>
#include <fileutils.h>
#include <configutils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystring.h>

int generate_main_program()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &g_mainVertexShaderSource, NULL);
	//printf("%s\n", g_mainVertexShaderSource);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSource = read_main_file();
	//printf("%s\n", fragmentShaderSource);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	free((void*)fragmentShaderSource);

	int res;
	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE)
	{
		glShaderSource(fragmentShader, 1, &g_fragmentShaderError, NULL);
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

void cleanseps(char* _str, int* _indices, char _s)
{
	int i = 0;
	int sep = 1;

	while (_str[i])
	{
		if (_str[i] == _s)
		{
			_str[i] = 0;
			_indices[sep] = i+1;
			sep++;
		}
		i++;
	}
}

void setup_ui_uniforms(void* _data, unsigned int _shaderProgram)
{
	int len = *(int*)(_data);
	char* startingData = (char*)(_data + sizeof(int) + sizeof(char));
	int startsstrings[len * 3];
	startsstrings[0] = 0;
	cleanseps(startingData, startsstrings, '|');

	char** p_names 		  = (char**)malloc(len * sizeof(char*));
	sh_loc_slider* p_sh_loc   = (sh_loc_slider*)malloc(len * sizeof(sh_loc_slider));
	float* p_fmin  		  = (float*)malloc(len * sizeof(float));
	float* p_fval  		  = (float*)malloc(len * sizeof(float));
	float* p_fmax  		  = (float*)malloc(len * sizeof(float));
	float* p_uv_loc  	  = (float*)malloc(len * sizeof(float));

	for (int i = 0; i < len * 3; i++)
	{
		//printf("%s\n", startingData + startsstrings[i]);

		if (i % 3 == 0)
		{
			p_names[i / 3] = strdup(startingData + startsstrings[i]);
			char* str_rad = concatenate(p_names[i / 3], "_radius");
			GLint value_location = glGetUniformLocation(_shaderProgram, p_names[i / 3]);
			GLint rad_location = glGetUniformLocation(_shaderProgram, str_rad);
			free(str_rad);
			p_sh_loc[i / 3].value = value_location;
			p_sh_loc[i / 3].radius = rad_location;
			//printf("%s %d %d\n", p_names[i], value_location, rad_location);
		}

		if (i % 3 == 1)
		{
			//float f1, f2, f3;
			int fsep[3];
			fsep[0] = 0;
			//get_floats(&f1, &f2, &f3);
			cleanseps(startingData + startsstrings[i], fsep, ' ');
			p_fmin[i / 3] = atof(startingData + startsstrings[i] + fsep[0]);
			p_fval[i / 3] = atof(startingData + startsstrings[i] + fsep[1]);
			p_fmax[i / 3] = atof(startingData + startsstrings[i] + fsep[2]);
			//printf("%f\n", v);
			//printf("%f\n", p_fmin[i]);
			//printf("%f\n", p_fval[i]);
			//printf("%f\n", p_fmax[i]);
		}

		if (i % 3 == 2)
		{
			float f4 = atof(startingData + startsstrings[i]);
			//printf("%f\n", f4);
			p_uv_loc[i / 3] = f4;
		}
	}

	g_ui_sliders = (sliders){ len, -1, p_names,p_sh_loc, p_fmin, p_fval, p_fmax, p_uv_loc };


	//char* sep1 = strchr(startingData, '|');
	//char* sep2 = strchr(sep1 + sizeof(char), '|');
	//int size1 = sep1 - startingData;
	//int size2 = sep2 - sep1;

	//char str[size1+1];
	//char str_rad[size1+8];
	//char val[size2];
	//strncpy(str, startingData, size1);
	//strncpy(val, sep1 + sizeof(char), size2);
	//str[size1] = 0;
	//strncpy(str_rad, str, size1);
	//strncpy(str_rad + size1, "_radius", 7);
	//str_rad[size1+7] = 0;
	//val[size2-1] = 0;
	//char* sep01 = strchr(sep1 + sizeof(char), ' ');
	//char* sep02 = strchr(sep01 + sizeof(char), ' ');
	//int size01 = sep01 - sep1;
	//int size02 = sep02 - sep01;
	//val[size01-1] = 0;
	//val[size02-1] = 0;

	//float f1 = atof(val);
	//float f2 = atof(val+size01);
	//float f3 = atof(val+size01+size02);
	//float f4 = atof(sep2+1);
	//GLint value_location = glGetUniformLocation(_shaderProgram, str);
	//GLint rad_location = glGetUniformLocation(_shaderProgram, str_rad);
	//printf("%s %d , %s %d\n", str, value_location, str_rad, rad_location);


	//g_uniform1f = (u_1f){ len, l, f};

	//int len;
	//char** name;
	//GLint* shader_location;
	//float* min_value;
	//float* current_value;
	//float* max_value;
	//float* uv_location;

	//printf("len : %d\n", len);
	//printf("str : %s\n", str);
	//printf("val : %s\n", val);
	//printf("f1 : %f\n", f1);
	//printf("f2 : %f\n", f2);
	//printf("f3 : %f\n", f3);
	//printf("f4 : %f\n", f4);
}

int generate_ui_program()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &g_uiVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	void* slider_data = generate_ui_shader();
	glShaderSource(fragmentShader, 1, &g_fragmentShaderConfig, NULL);
	glCompileShader(fragmentShader);

	int res;
	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE)
	{
		glShaderSource(fragmentShader, 1, &g_fragmentShaderError, NULL);
		glCompileShader(fragmentShader);
	}


	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	setup_ui_uniforms(slider_data, shaderProgram);
	free(slider_data);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}
