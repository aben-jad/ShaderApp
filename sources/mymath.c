#include <math.h>
#include <mystructs.h>
#include <mymacros.h>

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

//float abs(float _x)
//{
//	return fmaxf(_x, -_x);
//}

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


float compute_angle(vec3 _a, vec3 _b, vec3 _N)
{
	float ab = dist(_a) * dist(_b);
	float c = dot(_a, _b) / ab;
	if (c > 1.0f)
		c = 1.0f;
	else if (c < -1.0f)
		c = -1.0f;
	vec3 cr = cross(_a, _b);
	float dt = dot(cr, _N);
	float s = SIGN(dt);
	return acos(c) * s;
}

float clamp(float _x, float _min, float _max)
{
	if (_x < _min)
		return _min;

	if (_x > _max)
		return _max;

	return _x;
}

float fabsf(float _x)
{
	return fmaxf(_x, -_x);
}

int ipow(int _v, int _p)
{
	int i = 0;
	int res = 1;

	while (i < _p)
	{
		res *= _v;
		i++;
	}

	return res;
}
