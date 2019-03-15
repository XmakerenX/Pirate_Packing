#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCords;

uniform vec3 cubeColor;
uniform mat4 matWorld;
uniform mat4 projection;


out vec3 pos; 
out vec2 texUV;

void main()
{
	pos = position;
	texUV = texCords;
	gl_Position = projection * vec4(position, 1.0);
}  
