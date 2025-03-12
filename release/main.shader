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



d_id get_dist(vec3 _p)
{
	d_id box = d_id( sdBox(_p, 2), 0);
	d_id sphere = d_id( sdSphere(_p-vec3(0, -3, 0), 1.5), 1);

	return (box.d < sphere.d) ? box : sphere;
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

	vec3 col[2] = {vec3(1, 0, 0), vec3(0, 0, 1)};


	vec3 finalCol = vec3(0.);
	if (did.id == -1)
		finalCol += vec3(.25);
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
