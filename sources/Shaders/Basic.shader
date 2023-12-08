#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
void main()
{
       gl_Position = pos;
}
#end

#shader frag
#version 330 core

layout(location = 0) out vec4 col;

uniform vec4 u_Color;

void main()
{
       col = u_Color;
}
#end

