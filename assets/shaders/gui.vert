#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iTexCoord;

out vec2 oTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 tcTransform;

void main()
{
	gl_Position = projection * view * model * vec4(iPos, 1.0f);
	oTexCoord = (tcTransform * vec4(iTexCoord, 0, 1)).xy;
}
