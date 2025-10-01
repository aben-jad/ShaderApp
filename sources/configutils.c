#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <myglobals.h>
#include <mystring.h>
#include <fileutils.h>



unsigned int config_file_exist()
{
	char* filePath = concatenate(g_project_path, "\\.config");
	FILE* fp = fopen(filePath, "r");
	free(filePath);
	fclose(fp);

	return (fp != NULL);
}

char** get_next_attribute(FILE* _f)
{
	char* attName = get_next_line(_f);
	char* startBlock = get_next_line(_f);
	free(attName);
	free(startBlock);

	char* uniform = get_next_line_with_new_line(_f);
	char* vals = get_next_line(_f);

	char* endBlock = get_next_line(_f);
	free(endBlock);

	char** res = (char**)malloc(2 * sizeof(char*));
	res[0] = uniform;
	//printf("%s", uniform);
	res[1] = vals;
	return res;
	//if (strcmp(attName, "slider") == 0)
	//while (strcmp(line, "}\n"))
	//{
	//}
}

void* concatenate_sliders_data(int _len, char** _names, char** _vals, char** _uv_loc)
{
	//int len;
	//char** name;
	//GLint* shader_location;
	//float* min_value;
	//float* current_value;
	//float* max_value;
	//float* uv_location;

	int nlen[_len];
	int vlen[_len];
	int ulen[_len];
	int flen = 1 + 3 * _len + sizeof(int);
	for (int i = 0; i < _len ; i++)
	{
		nlen[i] = strlen(_names[i]);
		vlen[i] = strlen(_vals[i]);
		ulen[i] = strlen(_uv_loc[i]);
		flen += (nlen[i] + vlen[i] + ulen[i]);
	}

	void* res = malloc(flen);
	memcpy(res, (void*)(&_len), sizeof(int));
	int cur = sizeof(int);
	for (int i = 0; i < _len ; i++)
	{
		//printf("%s\n", _uv_loc[i]);
		((char*)res)[cur] = '|';
		cur++;
		memcpy(res + cur, _names[i], nlen[i]);
		cur += nlen[i];
		((char*)res)[cur] = '|';
		cur++;
		memcpy(res + cur, _vals[i], vlen[i]);
		cur += vlen[i];
		((char*)res)[cur] = '|';
		cur++;
		memcpy(res + cur, _uv_loc[i], ulen[i]);
		cur += ulen[i];
	}

	((char*)res)[flen-1] = 0;
	//printf("%s\n", (char*)(res + sizeof(int)));

	return res;

}

void* generate_uniforms(char** _uniforms_names, char** _vals, int _len)
{
	int nlen[_len];
	int vlen[_len];
	int flen = 1 + 2 * _len + sizeof(int);
	for (int i = 0; i < _len ; i++)
	{
		nlen[i] = strlen(_uniforms_names[i]);
		vlen[i] = strlen(_vals[i]);
		flen += (nlen[i] + vlen[i]);
	}

	void* res = malloc(flen);
	memcpy(res, (void*)(&_len), sizeof(int));
	int cur = sizeof(int);
	for (int i = 0; i < _len ; i++)
	{
		((char*)res)[cur] = '|';
		cur++;
		memcpy(res + cur, _uniforms_names[i], nlen[i]);
		cur += nlen[i];
		((char*)res)[cur] = '|';
		cur++;
		memcpy(res + cur, _vals[i], vlen[i]);
		cur += vlen[i];
	}

	((char*)res)[flen-1] = 0;

	return res;
}

int get_attribute_number(FILE* _f)
{
	char* line = get_next_line(_f);
	int len = 0;
	while(line != NULL)
	{
		if (strcmp(line, "slider") == 0)
			len++;
		free(line);
		line = get_next_line(_f);
	}
	return len;
}

char* get_attribute_name(char* _s)
{
	int startCursor = 0;
	while (_s[startCursor] != ' ')
		startCursor++;
	startCursor++;
	while (_s[startCursor] != ' ')
		startCursor++;
	startCursor++;

	int len = 0;
	while (_s[startCursor + len] != ';')
		len++;
	char* res = (char*)malloc(len + 1 * sizeof(char));
	res[len] = 0;
	strncpy(res, _s + startCursor, len);
	return res;
}

char* generate_uniform_radius(char* _u)
{
	int len = strlen(_u);
	int final_len = 2 * len + 8;
	char* res = (char*)malloc(final_len);

	strncpy(res, _u, len);
	strncpy(res + sizeof(char) * (len), _u, len);
	strncpy(res + (final_len - 10), "_radius;\n", 9);

	res[sizeof(char) * (final_len-1)] = 0;

	return res;
}

char* get_vec2(float _a, float _b)
{
	char* fa = ftostr(_a);
	char* fb = ftostr(_b);

	int alen = strlen(fa);
	int blen = strlen(fb);

	char* res = (char*)malloc((alen + blen + 8) * sizeof(char));
	res[0] = 'v';
	res[1] = 'e';
	res[2] = 'c';
	res[3] = '2';
	res[4] = '(';
	strncpy(res + 5, fa, alen);
	res[alen +5 ] = ',';
	res[alen +6 ] = ' ';
	strncpy(res + alen+7, fb, blen);
	res[alen + blen + 6] = ')';
	res[alen + blen + 7] = 0;
	free(fa);
	free(fb);
	return res;
}

void* generate_ui_shader()
{
	char* filePath = concatenate(g_project_path, "\\.config");
	FILE* fp = fopen(filePath, "r");
	int att_number = get_attribute_number(fp);
	fseek(fp, 0, SEEK_SET);
	
	char* atts[att_number];
	char* uniformsname[att_number];
	char* vals[att_number];
	char* uv_loc[att_number];
	int att_index = 0;
	char* shaderLines[3*att_number + 7];
	shaderLines[0] = (char*)g_fragment_begin;
	while (att_index < att_number)
	{
		char** tmp = get_next_attribute(fp);
		atts[att_index] = tmp[0];
		uniformsname[att_index] = get_attribute_name(atts[att_index]);
		float uv_lf = 1.0f - (float)(att_index + 1) / (float)(att_number + 1);
		uv_loc[att_index] = ftostr(uv_lf);
		shaderLines[1 + att_index] = generate_uniform_radius(tmp[0]);
		char* params[5];
		params[0] = "uv";
		params[1] = get_vec2(-g_slider_half_length, uv_lf);
		params[2] = get_vec2(g_slider_half_length, uv_lf);
		params[3] = uniformsname[att_index];
		params[4] = concatenate(params[3], "_radius");
		char* params2[3];
		params2[0] = "uv";
		params2[1] = params[1];
		params2[2] = params[2];
		shaderLines[6 + att_index + att_number] = generate_function_call("col += ", "slider", 5, params);
		shaderLines[6 + att_index + 2*att_number] = generate_function_call("//col += ", "sdSeg", 3, params2);
		vals[att_index] = tmp[1];
		free(params[1]);
		free(params[2]);
		free(params[4]);
		free(atts[att_index]);
		att_index++;
	}
	shaderLines[1 + att_number] = (char*)g_fragment_sdBox;
	shaderLines[2 + att_number] = (char*)g_fragment_sdSeg;
	shaderLines[3 + att_number] = (char*)g_fragment_sdCircle;
	shaderLines[4 + att_number] = (char*)g_fragment_slider;
	shaderLines[5 + att_number] = (char*)g_fragment_draw;
	shaderLines[6 + 3*att_number] = (char*)g_fragment_end; //6

	att_index = 0;
	g_fragmentShaderConfig = generate_text_from_strings(shaderLines, 3*att_number + 7); //7

	//void* uniforms = generate_uniforms(uniformsname, vals, att_number);
	void* sliders = concatenate_sliders_data(att_number, uniformsname, vals, uv_loc);

	while (att_index < att_number)
	{
		free(shaderLines[1 + att_index]);
		free(shaderLines[6 + att_index + att_number]);
		free(shaderLines[6 + att_index + 2*att_number]);
		free(uniformsname[att_index]);
		free(vals[att_index]);
		free(uv_loc[att_index]);
		att_index++;
	}

	printf("%s", g_fragmentShaderConfig);


	free(filePath);
	fclose(fp);

	return sliders;
}
