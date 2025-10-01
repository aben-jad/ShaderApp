#include <stdio.h>
#include <math.h>
#include <glad/glad.h>
//#include <SDL3/SDL.h>
#include <mymath.h>
#include <myglobals.h>
#include <mymacros.h>
//#include <fileutils.h>
//#include <shaderutils.h>
//#include <configutils.h>

#include <mystructs.h>

int mouse_on_slider(vec3 _slider, vec3 _mousePos)
{
	vec3 s = {_slider.x, _slider.y, 0.0f};
	float d = dist(ADD(_mousePos, SCA(-1,s)));
	return (d < _slider.z);
}

void init_ui()
{
	for (int i = 0; i < g_ui_sliders.len; i++)
	{
		float min = g_ui_sliders.min_value[i];
		float cur = g_ui_sliders.current_value[i];
		float max = g_ui_sliders.max_value[i];
		float final = clamp((cur-min) / (max-min), 0.0f, 1.0f);
		glUniform1f(g_ui_sliders.location[i].value, final);
		glUniform1f(g_ui_sliders.location[i].radius, g_slider_radius_off);
	}
}

void manage_hovered_ui(vec3 _mousePos)
{
	for (int i = 0; i < g_ui_sliders.len; i++)
	{
		float min = g_ui_sliders.min_value[i];
		float cur = g_ui_sliders.current_value[i];
		float max = g_ui_sliders.max_value[i];
		float final = clamp((cur-min) / (max-min), 0.0f, 1.0f);
		vec3 slider = { g_slider_half_length * (2*final - 1), g_ui_sliders.uv_location[i], 0.05f };
		if (mouse_on_slider(slider, _mousePos))
		{
			if (g_ui_sliders.hovered > -1)
				glUniform1f(g_ui_sliders.location[g_ui_sliders.hovered].radius, g_slider_radius_off);
			g_ui_sliders.hovered = i;
			glUniform1f(g_ui_sliders.location[i].radius, g_slider_radius_on);
			return;
		}
	}
	if (g_ui_sliders.hovered > -1)
		glUniform1f(g_ui_sliders.location[g_ui_sliders.hovered].radius, g_slider_radius_off);

	 g_ui_sliders.hovered = -1;

}

void manage_selected_ui(vec3 _currentUIMousePos)
{
	float a = -g_slider_half_length;
	float b = g_slider_half_length;
	float min = g_ui_sliders.min_value[g_ui_sliders.hovered];
	float max = g_ui_sliders.max_value[g_ui_sliders.hovered];
	float clampedMouse = clamp(_currentUIMousePos.x, a, b);
	float uni = (clampedMouse-a) / (b-a);
	float val = (1.0f-uni) * min + uni * max;

	glUniform1f(g_ui_sliders.location[g_ui_sliders.hovered].value, uni);
	g_ui_sliders.current_value[g_ui_sliders.hovered] = val;
}
