#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;

uniform bool useTexture;
uniform bool useColor;
uniform sampler2D texture_;
uniform vec4 color;
uniform vec4 clip;

void main()
{
    if (gl_FragCoord.x < clip.x
        || gl_FragCoord.y < clip.y
        || gl_FragCoord.x > clip.z
        || gl_FragCoord.y > clip.w
    ) {
        discard;
    }

    if (useTexture) {
        FragColor = texture(texture_, oTexCoord);
        if (useColor) FragColor *= color;
    } else {
        FragColor = color;
    }
}
