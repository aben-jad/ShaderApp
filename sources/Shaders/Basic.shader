#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;

out vec2 uv;

void main()
{
       gl_Position = pos;
       uv = pos.xy;
}



#end

#shader frag
#version 330 core

layout(location = 0) out vec4 col;

uniform vec4 u_Color;
in vec2 uv;

uniform float u_cameraDist;
uniform float u_alpha;
uniform float u_beta;

uniform vec3 u_origin;

struct mat
{
	vec4 c;
	float d;
};

float sdSeg(vec3 _p, vec3 _a, vec3 _b)
{
	vec3 lv = _b-_a;
	vec3 pv = _p-_a;

	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);

	return length(_p-(_a + dt*(_b-_a)));
}

//float sdBox(vec3 _p, float _r)
//{
//	vec3 d = max(_p-_r, 0.);
//
//	return length(d);
//}

mat get_dist(vec3 _p)
	//float d1 = sdBox(_p, .3);
	float d1 = _p.y + .5;
	float d2 = sdSeg(_p, vec3(u_alpha, u_beta, u_cameraDist+1), u_origin);
	//return mat(vec4(0, 0, 1, 1), d2);
	return (d1 > d2) ? mat(vec4(1, 0, 0, 1), d2) : mat(vec4(0, 0, 1, 1), d1);
}

mat ray_march()
{
	//vec3 cp = vec3(0.,sin(u_alpha), u_cameraDist);
	vec3 cp = vec3(0.,0, u_cameraDist);
	vec3 rd = normalize(vec3(uv.x, uv.y, 1));

	float d = 0.;
	vec4 c = vec4(0., 0., 0., 1.);
	for (int i = 0; i < 100; i++)
	{
		mat m = get_dist(cp + rd*d);
		float ds = m.d;
		d += ds;

		if (ds < .01)
		{
			c = m.c;
			break;
		}
		if( d > 1000.0)
			break;
	}

	return mat(c, d);
}

vec4 draw()
{
	mat a = ray_march();
	return a.c;
}

void main()
{
       col = draw();
}
#end

