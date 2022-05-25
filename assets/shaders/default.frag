#version 330 core
out vec4 FragColor;

in vec3 oColor;
in vec2 oTexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, oTexCoord) * vec4(oColor, 1);
}
