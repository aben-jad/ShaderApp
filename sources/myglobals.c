#include <mystructs.h>

const char *g_mainVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec4 pos;\n"
		"out vec2 uv;\n"
		"void main() {\n"
		"    gl_Position = pos;\n"
		"    uv = pos.xy;\n"
		"}";

const char *g_uiVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec4 pos;\n"
		"layout (location = 1) in vec2 i_texCoord;\n"
		"out vec2 texCoord;\n"
		"out vec2 uv;\n"
		"void main() {\n"
		"    gl_Position = pos;\n"
		"    uv = pos.xy*vec2(.5, 1);\n"
		"    texCoord = i_texCoord;\n"
		"}";

const char *g_fragmentShaderError = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
		"}";

const char *g_fragment_begin =  "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 uv;\n"
		"in vec2 texCoord;\n"
		"uniform sampler2D u_text;\n";

const char *g_fragment_sdBox = "float sdBox(vec2 _p, vec2 _r) {\n"
	        "vec2 d = abs(_p)-_r;\n"
		"return length(max(d, 0)) + min(max(d.x, d.y), 0);\n"
		"}\n";

const char *g_fragment_sdSeg = "float sdSeg(vec2 _p, vec2 _a, vec2 _b) {\n"
		"vec2 lv = _b-_a;\n"
        	"vec2 pv = _p-_a;\n"
        	"float dt = clamp(dot(lv, pv) / dot(lv, lv), 0., 1.);\n"
        	"return length(_p-(_a + dt*(_b-_a)));\n"
		"}\n";

const char *g_fragment_sdCircle = "float sdCircle(vec2 _p, float _r) {\n"
		"return length(_p) - _r;\n"
		"}\n";

const char *g_fragment_slider = "vec3 slider(vec2 _p, vec2 _a, vec2 _b, float _r, float _size) {\n"
		"vec2 c = _a + _r * (_b-_a);\n"
		"float segD = sdSeg(_p, _a, _b);\n"
		"float cirD = sdCircle(_p-c, _size);\n"
		//"float cirD = sdCircle(_p, _size);\n"
		"float segA = smoothstep(0.01, 0., segD);\n"
		"float cirA = smoothstep(0.01, 0., cirD);\n"
		"vec3 col = .7*vec3(segA);\n"
		"col.x += .7*cirA;\n"
		"return col;\n"
		"}\n";


const char *g_fragment_draw = "vec4 draw() {\n"
		"vec3 col = vec3(0.0);\n";

const char *g_fragment_end = "return vec4(col ,1);\n"
		"}\n"
		"void main() {\n"
		"float d = sdBox(uv, vec2(.5, 1.));\n"
		"float a = smoothstep(0., 0.01, abs(d) - .01);\n"
		"vec4 drawCol = draw();\n"
		"vec4 col = texture(u_text, texCoord);\n"
		"FragColor = mix(mix(vec4(.7, .7, .7, 1), drawCol, a), col, col.a);\n"
		"float d_t = sdBox(texCoord-vec2(.5), vec2(.5));\n"
		//"float d_t = sdSeg(texCoord, vec2(-.5, .5), vec2(.5));\n"
		"float a_t = smoothstep(0.01, 0.0, abs(d_t))-.01;\n"
		//"float a_t = smoothstep(0.01, 0.0, d_t);\n"
		"FragColor = mix(FragColor, vec4(1), a_t);\n"
		"}\n";

const float g_slider_half_length = 0.35f;
const char *g_fragmentShaderConfig = "";
char* g_project_path = "";
u_1f g_uniform1f;
sliders g_ui_sliders;
const float g_slider_radius_off = 0.01f;
const float g_slider_radius_on = 0.05f;
