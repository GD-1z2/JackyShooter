#version 330 core
layout (location = 0) in vec3 iPos;

out vec3 oTexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    oTexCoords = iPos;
    gl_Position = (projection * view * vec4(iPos, 1.0)).xyww;
}
