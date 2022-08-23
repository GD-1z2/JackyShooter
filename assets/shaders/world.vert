#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iTexCoord;
layout (location = 2) in vec3 iNormal;

out vec2 oTexCoord;
out vec3 oNormal;
out vec3 oPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(iPos, 1.0f);
    oTexCoord = iTexCoord;
    oNormal = mat3(transpose(inverse(model))) * iNormal;
    oPosition = vec3(model * vec4(iPos, 1.0));
}
