#version 330 core

uniform sampler2D meshTexture;

uniform vec3 cubeColor;
uniform mat4 matWorld;
uniform mat4 projection;

in vec3 pos;
in vec2 texUV;
out vec4 color;

void main()
{
    vec4 sampled = texture(meshTexture, texUV);
    color = vec4(cubeColor, sampled.a);
    //color = vec4(cubeColor, 1.0)*0.5 + sampled*0.5;
    //color = vec4(cubeColor, 1.0)*0.5 + sampled*0.5;
    color = vec4(min(1.0, cubeColor.r*(1/(1+sampled.a)) + sampled.r*(sampled.a)/(1+sampled.a)),
    min(1.0, cubeColor.g*(1/(1+sampled.a)) + sampled.g*(sampled.a)/(1+sampled.a)),
    min(1.0, cubeColor.b*(1/(1+sampled.a)) + sampled.b*(sampled.a)/(1+sampled.a)),
    sampled.a);
    //color = sampled;
    //color = vec4(cubeColor, 1.0);
}
 
