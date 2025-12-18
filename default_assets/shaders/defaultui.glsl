#define __STARTSHADER
#define __SHADERTYPE VTX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aUV;
out vec2 texCoords;

uniform mat4 _internalModel;
uniform mat4 _internalProj;

void main()
{
   texCoords = aUV;

   texCoords.y = 1.0 - texCoords.y;

   vec4  p = _internalProj * _internalModel * vec4(aPos,0, 1.0);

   p.z = 0;

   gl_Position = p;
}
#define __ENDSHADER



#define __STARTSHADER
#define __SHADERTYPE FRAG

#version 330 core
out vec4 FragColor;
in vec2 texCoords;

#definclude "shaders/util.shdinc"

uniform sampler2D _mainTexture;
uniform vec3 _mainColor;

void main()
{
   vec4  col = texture(_mainTexture,texCoords);
   col.rgb *= color_vec3(_mainColor);

   FragColor = vec4(col);
}

#define __ENDSHADER