#version 430 core

layout(std430, binding=0) buffer CellBuffer {
	uint cellData[]; 
};

out vec4 FragColor;
in vec2 uv;
in vec2 tex_uv;

struct bez_pts {
	vec2 a, c, b;
};

uniform float u_size;
uniform vec2 u_cursor;

float dot2(vec2 _p)
{
	return dot(_p, _p);
}
float sdCircle(vec2 _p, float _r) {
	 return length(_p) - _r;
}
bez_pts get_spline_points(int _index) {
	float px1 = float(cellData[_index]) / 1024.0;
	float py1 = float(cellData[_index + 1]) / 1024.0;
	float px2 = float(cellData[_index + 2]) / 1024.0;
	float py2 = float(cellData[_index + 3]) / 1024.0;
	float px3 = float(cellData[_index + 4]) / 1024.0;
	float py3 = float(cellData[_index + 5]) / 1024.0;
	return bez_pts(vec2(px1, py1), vec2(px2, py2), vec2(px3, py3));
}
float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C )
{    
	vec2 a = B - A;
	vec2 b = A - 2.0*B + C;
	vec2 c = a * 2.0;
	vec2 d = A - pos;
	float kk = 1.0/dot(b,b);
	float kx = kk * dot(a,b);
	float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
	float kz = kk * dot(d,a);      
	float res = 0.0;
	float p = ky - kx*kx;
	float p3 = p*p*p;
	float q = kx*(2.0*kx*kx-3.0*ky) + kz;
	float h = q*q + 4.0*p3;
	if( h >= 0.0) 
	{ 
		h = sqrt(h);
		vec2 x = (vec2(h,-h)-q)/2.0;
		vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
		float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
		res = dot2(d + (c + b*t)*t);
	}
	else
	{
		float z = sqrt(-p);
		float v = acos( q/(p*z*2.0) ) / 3.0;
		float m = cos(v);
		float n = sin(v)*1.732050808;
		vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);
		res = min( dot2(d+(c+b*t.x)*t.x),
	               dot2(d+(c+b*t.y)*t.y) );
	}
	return sqrt( res );
}

float sdBox(vec2 _p, vec2 _r) {
	 _p = abs(_p) - _r;
    vec2 n_p = max(_p, vec2(0.));
    float d = length(n_p) + min(0, max(_p.x, _p.y));
	 return d;
}

float sdSeg(vec2 _p, vec2 _a, vec2 _b) {
   	vec2 lv = _b-_a;
   	vec2 pv = _p-_a;
   	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);
   	return length(_p-(_a + dt*(_b-_a)));
}

float construct_sdf(int _index) {
	if (cellData[_index] == 0)	return 100.0;
	bez_pts bez_p1 = get_spline_points(_index + 1);
	bez_pts bez_p2 = get_spline_points(_index + 7);
	bez_pts bez_p3 = get_spline_points(_index + 13);
	float d1 = sdBezier(fract(tex_uv*u_size) ,bez_p1.a, bez_p1.c, bez_p1.b);
	float d2 = sdBezier(fract(tex_uv*u_size) ,bez_p2.a, bez_p2.c, bez_p2.b);
	float d3 = sdBezier(fract(tex_uv*u_size) ,bez_p3.a, bez_p3.c, bez_p3.b);
	return min(min(d1, d2), d3);
}

void main() {
	int ns = int(u_size);
	ivec2 nuv = ivec2(int(tex_uv.x * u_size), int(tex_uv.y * u_size));
	int ind =  nuv.x + nuv.y * ns;
	float sd_bez_a = smoothstep(0.1, -0.05, construct_sdf(ind * 19));
    	vec3 ssbo = vec3(sd_bez_a, 0., 0.);
    	FragColor = vec4(ssbo, 1.);
}
