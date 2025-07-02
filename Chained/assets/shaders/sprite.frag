#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec4 uvRect;

void main()
{
    vec2 finalUV = uvRect.xy + TexCoords * uvRect.zw;
    FragColor = texture(image, finalUV) * vec4(spriteColor, 1.0);
}
