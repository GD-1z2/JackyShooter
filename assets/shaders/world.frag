#version 330 core
out vec4 FragColor;

in vec3 oPosition;
in vec2 oTexCoord;
in vec3 oNormal;

struct Material {
    float specularExponent;

    bool useTexture;
    vec3 color;
    sampler2D diffuseTexture;

    bool useMetalness;
    sampler2D metalnessTexture;
};

uniform Material material;
uniform vec3 viewPos;
uniform vec3 lightPos;

void main()
{
    vec4 baseColor = material.useTexture
        ? texture(material.diffuseTexture, oTexCoord)
        : vec4(material.color, 1.0);

    vec3 lhColor = vec3(1.0, 1.0, 1.0);

    vec3 lhValAmbient = vec3(.5, .5, .5);

    vec3 norm = normalize(oNormal);
    vec3 lhDir = normalize(lightPos - oPosition);
    vec3 lhValDiffuse = max(dot(norm, lhDir), 0.0) * lhColor;

    vec3 viewDir = normalize(viewPos - oPosition);
    vec3 reflectDir = reflect(-lhDir, norm);
    vec3 lhValSpecular =
        pow(max(dot(viewDir, reflectDir), 0.0), 225)
        * lhColor
        * material.specularExponent;
    if (material.useMetalness)
        lhValSpecular *= texture(material.metalnessTexture, oTexCoord).rrr;

    FragColor = baseColor * vec4(lhValAmbient + lhValDiffuse + lhValSpecular,
        1.0);
}
