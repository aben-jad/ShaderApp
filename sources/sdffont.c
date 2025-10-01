#include <math.h>
#include <mystructs.h>
#include <mymath.h>
#include <mymacros.h>
#include <stdio.h>
#include <stdlib.h>

static float sd_seg(vec3 _p, vec3 _a, vec3 _b)
{
	vec3 ba = ADD(_b, SCA(-1, _a));
	vec3 pa = ADD(_p, SCA(-1, _a));
	float dt = clamp(dot(ba, pa) / dot(ba, ba), 0.0f, 1.0f);

	vec3 h = ADD(_a, SCA(dt, ba));

	return dist(ADD(_p, SCA(-1, h)));
}

static float sd_circle(vec3 _p, float _r)
{
	return dist(_p) - _r;
}

static float sd_o(vec3 _uvs)
{
	vec3 uv = { _uvs.x, _uvs.y, 0.0f};
	float cir = sd_circle(uv, 0.9f);

	return fabsf(cir) - 0.01f;
}

static float sd_a(vec3 _uvs)
{
	vec3 a = { -1.0f, -1.0f, 0.0f};
	vec3 b = { 0.0f, 1.0f, 0.0f};
	vec3 c = { 1.0f, -1.0f, 0.0f};

	vec3 h1 = ADD(a, SCA(.5f, ADD(b,SCA(-1, a))));
	vec3 h2 = ADD(c, SCA(.5f, ADD(b,SCA(-1, c))));

	vec3 uv = { _uvs.x, _uvs.y, 0.0f};

	float seg1 = sd_seg(uv, a, b);
	float seg2 = sd_seg(uv, b, c);
	float seg3 = sd_seg(uv, h1, h2);

	 return fminf(fminf(seg1, seg2), seg3);
}

static float sd_l(vec3 _uvs)
{
	vec3 a = { -1.0f, -1.0f, 0.0f};
	vec3 b = { -1.0f, 1.0f, 0.0f};
	vec3 c = { 1.0f, -1.0f, 0.0f};


	vec3 uv = { _uvs.x, _uvs.y, 0.0f};

	float seg1 = sd_seg(uv, a, b);
	float seg2 = sd_seg(uv, a, c);

	 return fminf(seg1, seg2);
}
static float sd_h(vec3 _uvs)
{
	vec3 a = { -1.0f, -1.0f, 0.0f};
	vec3 b = { -1.0f, 1.0f, 0.0f};
	vec3 c = { 1.0f, -1.0f, 0.0f};
	vec3 d = { 1.0f, 1.0f, 0.0f};

	vec3 h1 = ADD(a, SCA(.5f, ADD(b,SCA(-1, a))));
	vec3 h2 = ADD(c, SCA(.5f, ADD(d,SCA(-1, c))));

	vec3 uv = { _uvs.x, _uvs.y, 0.0f};

	float seg1 = sd_seg(uv, a, b);
	float seg2 = sd_seg(uv, d, c);
	float seg3 = sd_seg(uv, h1, h2);

	 return fminf(fminf(seg1, seg2), seg3);
}

float sdf_char(char _c, vec3 _uvs)
{
	float (*funcs[4])(vec3);
	funcs[0] = &sd_a;
	funcs[1] = &sd_o;
	funcs[2] = &sd_h;
	funcs[3] = &sd_l;

	int c = 0;
	if (_c == 'h')
		c = 2;
	else if(_c == 'o')
		c = 1;
	else if(_c == 'l')
		c = 3;

	return funcs[(int)c](_uvs);
}

unsigned int* sdf_params_h()
{
	int size = 18 * sizeof(int);
	int* res = (int*)malloc(size);

	res[0] = 10;
	res[1] = 1000;
	res[2] = 500;
	res[3] = 800;
	res[4] = 100;
	res[5] = 20;

	res[6] = 600;
	res[7] = 20;
	res[8] = 400;
	res[9] = 500;
	res[10] = 1000;
	res[11] = 1000;

	res[12] = 200;
	res[13] = 500;
	res[14] = 100;
	res[15] = 400;
	res[16] = 700;
	res[17] = 500;

	return res;
}

unsigned int* sdf_params_l()
{
	int size = 18 * sizeof(int);
	int* res = (int*)malloc(size);

	res[0] = 10;
	res[1] = 1000;
	res[2] = 500;
	res[3] = 800;
	res[4] = 100;
	res[5] = 20;

	res[6] = 10;
	res[7] = 1000;
	res[8] = 400;
	res[9] = 500;
	res[10] = 1000;
	res[11] = 1000;

	res[12] = 100;
	res[13] = 20;
	res[14] = 200;
	res[15] = 15;
	res[16] = 50;
	res[17] = 10;

	return res;
}

unsigned int* sdf_params_o()
{
	int size = 18 * sizeof(int);
	int* res = (int*)malloc(size);

	res[0] = 500;
	res[1] = 20;
	res[2] = 20;
	res[3] = 500;
	res[4] = 500;
	res[5] = 1000;

	res[6] = 500;
	res[7] = 20;
	res[8] = 1500;
	res[9] = 500;
	res[10] = 500;
	res[11] = 1000;

	res[12] = 100;
	res[13] = 20;
	res[14] = 200;
	res[15] = 15;
	res[16] = 50;
	res[17] = 10;

	return res;
}

unsigned int* sdf_params_a()
{
	int size = 18 * sizeof(int);
	int* res = (int*)malloc(size);

	res[0] = 10;
	res[1] = 1000;
	res[2] = 500;
	res[3] = 800;
	res[4] = 500;
	res[5] = 20;

	res[6] = 500;
	res[7] = 20;
	res[8] = 900;
	res[9] = 500;
	res[10] = 1000;
	res[11] = 1000;

	res[12] = 400;
	res[13] = 500;
	res[14] = 300;
	res[15] = 400;
	res[16] = 1000;
	res[17] = 500;

	return res;
}

unsigned int* sdf_params_char(char _c)
{
	if (_c == 'h')
		return sdf_params_h();
	if (_c == 'o')
		return sdf_params_o();
	if (_c == 'l')
		return sdf_params_l();

	return sdf_params_a();
}
