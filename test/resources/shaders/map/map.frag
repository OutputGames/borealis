#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;
uniform vec4 color;
uniform sampler2D tex;
uniform sampler2D norm;

void main()
{
   FragColor = vec4(texture(tex, texCoords));
   vec3 norm = normalize(normal);
   vec3 lightDir = -normalize(vec3(1,-1,-1));
    float d =max(dot(norm, lightDir), 0.0) + 0.25f;
   FragColor = vec4(texture(tex,texCoords).rgb,1.0);
}
