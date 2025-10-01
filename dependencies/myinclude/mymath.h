#ifndef _MYMATH_H_
#define _MYMATH_H_
#include <mystructs.h>

quaternion quaternion_multiplication(quaternion _a, quaternion _b);
float dot(vec3 _a, vec3 _b);
vec3 quaternion_rotation(vec3 _p, float _ang, vec3 _axis);
vec3 cross(vec3 _a, vec3 _b);
float dist(vec3 _a);
vec3 normalize(vec3 _a);
float compute_angle(vec3 _a, vec3 _b, vec3 _N);
float clamp(float _x, float _min, float _max);
float fabsf(float _x);
int ipow(int _v, int _p);

#endif
