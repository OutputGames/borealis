#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;
uniform vec3 color;
uniform sampler2D tex;
uniform float _internalTime;
void main()
{
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(vec3(sin(_internalTime)*3,2,cos(_internalTime)*3) - pos);
    float d =max(dot(norm, lightDir), 0.0) + 0.25f;
   FragColor = vec4(d*texture(tex,texCoords));
}
