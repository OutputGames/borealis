#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec4 pos;
uniform vec3 color;
uniform sampler2DArray tex;
uniform vec3 _cameraPosition;

uniform float _internalTime;
void main()
{
   vec3 norm = normalize(normal);

   float time = _internalTime;

   time += pos.w * 2.5f;

   vec4 color = texture(tex,vec3(texCoords,mod(time*10.0f,  textureSize(tex, 0).z)));

   if (color.a < 0.1)
      discard;

   vec3 final = color.rgb;

   float dist = (distance(pos.xyz,_cameraPosition) - 8.f)/32.f;

   color.a = 1.0 - clamp(dist,0,1);

   FragColor = vec4(final, color.a);
}
