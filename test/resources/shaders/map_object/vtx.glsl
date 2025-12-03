#version 330 core
#property INSTANCING
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 _internalView;
uniform mat4 _internalProj;
out vec2 texCoords;
out vec3 normal;
out vec4 pos;
void main()
{
   mat4 model = instanceMatrix;

   texCoords = aUV;   
   normal = mat3(transpose(inverse(model))) * aNorm;   
   pos = vec4(vec3(model * vec4(aPos, 1.0)), gl_InstanceID);   
   gl_Position = _internalProj * _internalView * model * vec4(aPos, 1.0);
}