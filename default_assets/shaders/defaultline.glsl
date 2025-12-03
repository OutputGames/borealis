#define __STARTSHADER
#define __SHADERTYPE VTX

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
uniform mat4 _internalModel;
uniform mat4 _internalView;
uniform mat4 _internalProj;

uniform vec3 startPos;
uniform vec3 endPos;

out vec2 texCoords;
out vec3 normal;
out vec3 pos;
void main()
{

    if (gl_VertexID == 0) {
        pos = vec3(_internalModel[0]);
    } else {
        pos = vec3(_internalModel[1]);
    }

    texCoords = aUV;
    gl_Position = _internalProj * _internalView * vec4(pos, 1.0);
}

#define __ENDSHADER



#define __STARTSHADER
#define __SHADERTYPE FRAG

#version 330 core
out vec4 FragColor;
in vec2 texCoords;

#definclude "shaders/util.shdinc"

uniform vec3 _mainColor;

void main()
{

   FragColor = vec4(1);
}

#define __ENDSHADER