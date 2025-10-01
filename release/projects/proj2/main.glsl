#version 330 core

out vec4 FragColor;
in vec2 uv;

uniform float u_box_circle_mix;
//uniform float u_cir_rad;
 

float sdSeg(vec2 _p, vec2 _a, vec2 _b)
{
	vec2 lv = _b-_a;
	vec2 pv = _p-_a;

	float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);

	return length(_p-(_a + dt*(_b-_a)));
}

float dot2(vec2 _p)
{
	return dot(_p, _p);
}

float sdBox(vec2 _p, vec2 _r)
{
	vec2 d = abs(_p)-_r;
	return length(max(d, 0)) + min(max(d.x, d.y), 0);
}

float sdCircle(vec2 _p, float _r)
{
	return length(_p)-_r;
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

void main()
{
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//vec2 new_texCoord = texCoord;// * u_text_size - .5*vec2((u_text_size - 1)) ;
	//if (new_texCoord.y > .8 ) new_texCoord.y -= .6; 
	//if (new_texCoord.y < -.6 ) new_texCoord.y += 1; 
	//vec4 col = texture(u_text, new_texCoord);
	//float d = sdBox(texCoord-vec2(.5), vec2(.4));
	//float a = smoothstep(0.001, 0., d);
	//FragColor = mix(vec4(1, 1.0, 1.0, 1.0), col, col.a);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec2 new_uv = uv;
	////new_uv.y += .2*new_uv.x * new_uv.x;
	float db = sdBox(new_uv - vec2(0., .3), vec2(.4, .3));
	//float dc = sdCircle(new_uv, u_cir_rad);
	float dc = sdCircle(new_uv, .12);
	float d = mix(db, dc, u_box_circle_mix);
	float ab = smoothstep(0.0, -0.01, db);
	float ac = smoothstep(0.0, -0.01, dc);
	float a = smoothstep(0.0, -0.01, d);
	FragColor = vec4(ab*0, a, ac*0, 1.0);
}

