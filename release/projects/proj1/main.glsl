#version 330 core

out vec4 FragColor;

in vec2 uv;


uniform vec3 u_light1Pos;
uniform vec3 u_light2Pos;
uniform vec3 u_light3Pos;

uniform vec3 u_camPos;
uniform vec3 u_screenPos;

uniform vec3 u_right;
uniform vec3 u_up;

//uniform vec2 u_mouse;

struct mat
{
	vec4 c;
	float d;
};

struct d_id
{
	float d;
	int id;
};
//
float sdSeg(vec3 _p, vec3 _a, vec3 _b)
{
	vec3 lv = _b-_a;
	vec3 pv = _p-_a;

	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);

	return length(_p-(_a + dt*(_b-_a)));
}

float dot2(vec2 p)
{
	return dot(p, p);
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
	    // the third root cannot be the closest
	    // res = min(res,dot2(d+(c+b*t.z)*t.z));
	}
	return sqrt( res );
}
float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
	vec3 pa = p - a, ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r;
}

float sdBox(vec3 _p, float _r)
{
	vec3 d = max(abs(_p)-vec3(_r, _r, _r), vec3(0., 0., 0.));

	return length(d);
}

float sdTorus( vec3 p, vec2 t )
{
	vec2 q = vec2(length(p.xz)-t.x,p.y);
	return length(q)-t.y;
}

float sdSphere(vec3 _p, float _r)
{
	return length(_p) - _r;
}

float sdSolidAngle( vec3 p, vec2 c, float ra )
{
	// c is the sin/cos of the angle
	vec2 q = vec2( length(p.xz), p.y );
	float l = length(q) - ra;
	float m = length(q - c*clamp(dot(q,c),0.0,ra) );
	return max(l,m*sign(c.y*q.x-c.x*q.y));
}
float sdCappedCylinder( vec3 p, vec3 a, vec3 b, float r )
{
	vec3  ba = b - a;
	vec3  pa = p - a;
	float baba = dot(ba,ba);
	float paba = dot(pa,ba);
	float x = length(pa*baba-ba*paba) - r*baba;
	float y = abs(paba-baba*0.5)-baba*0.5;
	float x2 = x*x;
	float y2 = y*y*baba;
	float d = (max(x,y)<0.0)?-min(x2,y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
	return sign(d)*sqrt(abs(d))/baba;
}

d_id foot(vec3 _p)
{
	d_id l_capsule_up = d_id( sdCapsule(_p, vec3(-2, -5, 0), vec3(-2, -5, 1.5), 2), 0);
	d_id l_capsule_down = d_id( sdCapsule(_p, vec3(-2, -5.25, 0), vec3(-2, -5.25, 1.5), 1.8), 1);
	d_id l_torus = d_id(sdTorus(_p-vec3(-2, -3, 0), vec2(1.2, .6)), 2);
	d_id lf = (l_capsule_up.d < l_capsule_down.d && l_capsule_up.d < l_torus.d) ? l_capsule_up : (l_capsule_down.d < l_torus.d) ? l_capsule_down : l_torus;

	d_id r_capsule_up = d_id( sdCapsule(_p, vec3(2, -5, 0), vec3(2, -5, 1.5), 2), 0);
	d_id r_capsule_down = d_id( sdCapsule(_p, vec3(2, -5.25, 0), vec3(2, -5.25, 1.5), 1.8), 1);
	d_id r_torus = d_id(sdTorus(_p-vec3(2, -3, 0), vec2(1.2, .6)), 2);
	d_id rf = (r_capsule_up.d < r_capsule_down.d && r_capsule_up.d < r_torus.d) ? r_capsule_up : (r_capsule_down.d < r_torus.d) ? r_capsule_down : r_torus;

	return (lf.d < rf.d) ? lf : rf;

}

d_id torso(vec3 _p)
{
	_p *= vec3(.3, .3, .7);
	float d = sdBox(_p-vec3(0, 1, 0), 1);
	return d_id(d, 0);

	//float ld = abs(sdBezier(_p.xy, vec2(-3, 2), vec2(-1, 5), vec2(-3, 8)));
	//float rd = abs(sdBezier(_p.xy, vec2(3, 2), vec2(1, 5), vec2(3, 8)));
	//float dd = abs(sdBezier(_p.xy, vec2(-3, 2), vec2(0, 5), vec2(3, 2)));
	//float ud = abs(sdBezier(_p.xy, vec2(-3, 8), vec2(0, 10), vec2(3, 8)));

	//float d = min(min(ld, rd), min(dd, ud));
	//float h = 0.01;

	//vec2 w = vec2( d, abs(_p.z) - h );
	//return d_id(min(max(w.x,w.y),0.0) + length(max(w,0.0)), 0);
	
	//return d_id(bez_d, 0);
}

d_id legs(vec3 _p)
{
	d_id ll = d_id(sdCappedCylinder(_p, vec3(-2, -3, 0), vec3(-2, 0, 0), 1), 3);
	d_id rl = d_id(sdCappedCylinder(_p, vec3(2, -3, 0), vec3(2, 0, 0), 1), 3);

	return (ll.d < rl.d) ? ll : rl;
}

d_id get_dist(vec3 _p)
{
	d_id f = foot(_p);
	d_id l = legs(_p);
	d_id t = torso(_p);

	return (f.d < l.d && f.d < t.d) ? f : (l.d < t.d) ? l : t;
}

vec3 get_normal(vec3 _p)
{
	float d = get_dist(_p).d;
	vec2 e = vec2(0.01, 0.);

	vec3 n = vec3( d - get_dist(_p - e.xyy).d,
		d - get_dist(_p - e.yxy).d,
		d - get_dist(_p - e.yyx).d );

	return normalize(n);
}

float get_light(vec3 _p)
{
	//vec3 light1Pos = vec3(0, 5, 0);
	//vec3 light2Pos = vec3(4, -5, 4);
	//vec3 light3Pos = vec3(-4, -5, -4);

	vec3 l1 = normalize(u_light1Pos-_p);
	vec3 l2 = normalize(u_light2Pos-_p);
	vec3 l3 = normalize(u_light3Pos-_p);
	vec3 n = get_normal(_p);

	return (max(dot(l1, n), 0) + max(dot(l2, n), 0) + max(dot(l3, n), 0));
}

d_id ray_march(vec3 _cp, vec3 _rd)
{

	float d = 0.;
	int id = -1;
	for (int i = 0; i < 100; i++)
	{
		d_id ds_id = get_dist(_cp + _rd*d);
		d += ds_id.d;

		if (ds_id.d < .01 || d > 1000.0)
		{
			if (ds_id.d < 0.01)
				id = ds_id.id;
			break;
		}
	}

	return d_id (d, id);
}

vec4 draw()
{
	vec3 cp = u_camPos;
	vec3 sp = u_screenPos;
	vec3 R = u_right;
	vec3 U = u_up;

	vec3 is = sp + uv.x * R + uv.y * U;
	vec3 rd = normalize(is-cp);

	d_id did = ray_march(cp, rd);

	vec3 red_col = vec3(0.8, 0.0, 0.0);
	vec3 yel_col = vec3(0.8, 0.8, 0.0);
	vec3 gry_col = vec3(0.4, 0.4, 0.4);

	vec3 col[4] = { 
		red_col, 
		yel_col, 
		red_col,
		gry_col}; 


	vec3 finalCol = vec3(0.);
	if (did.id == -1)
		finalCol += vec3(.75);
	else
	{
		vec3 p = cp + rd*did.d;
		float a = get_light(p);
		finalCol += a*col[did.id];
	}
	//vec3 col = vec3(.8, .6, 0.) * a + vec3(.3);
	//col = vec3(1) * a;

	return vec4(finalCol, 1);
}

void main()
{
	FragColor = draw();
}
