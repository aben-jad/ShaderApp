#ifndef _MYSTRUCTS_H_
#define _MYSTRUCTS_H_

#include <glad/glad.h>

typedef struct
{
	float x, y, z;
} vec3;

typedef struct
{
	float w, x, y, z;
} quaternion;

typedef struct
{
	int len;
	GLint* location;
	float* value;
} u_1f;

typedef struct
{
	GLint value, radius;
} sh_loc_slider;

typedef struct
{
	int len;
	int hovered;
	char** name;
	sh_loc_slider* location;
	float* min_value;
	float* current_value;
	float* max_value;
	float* uv_location;
} sliders;

typedef struct
{
	GLint x;
	GLint y;
	GLsizei w;
	GLsizei h;
} rect;

#endif
