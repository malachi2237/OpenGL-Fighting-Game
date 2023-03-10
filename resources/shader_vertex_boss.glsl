#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 vert_pos;
void main()
{
	gl_Position = P * V * M * vec4(vertPos, 1.0);
	vert_pos = vertPos;
}
