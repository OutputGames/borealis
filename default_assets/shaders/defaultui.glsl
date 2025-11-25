#define __STARTSHADER
#define __SHADERTYPE VTX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
out vec2 texCoords;

uniform mat4 _internalModel;
uniform mat4 _internalProj;

void main()
{
   texCoords = aUV;
   gl_Position = _internalProj * _internalModel * vec4(aPos,0, 1.0);
}
#define __ENDSHADER



#define __STARTSHADER
#define __SHADERTYPE FRAG
#version 330 core
out vec4 FragColor;
in vec2 texCoords;
uniform sampler2D _sourceTexture;
void main()
{
   FragColor = vec4(texture(_sourceTexture,texCoords).rgb,1);
   FragColor = vec4(vec3(1.0),1);
}

#define __ENDSHADER