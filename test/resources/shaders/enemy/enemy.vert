#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
uniform mat4 _internalModel;
uniform mat4 _internalView;
uniform mat4 _internalProj;
out vec2 texCoords;
out vec3 normal;
out vec3 pos;
void main()
{
   texCoords = aUV;
      normal = mat3(transpose(inverse(_internalModel))) * aNorm;
         pos = vec3(_internalModel * vec4(aPos, 1.0));
            gl_Position = _internalProj * _internalView * _internalModel * vec4(aPos, 1.0);
}