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
   lightDir = -normalize(vec3(1,-1,-1));
   float d = max(dot(norm, lightDir), 0.0) + 0.25f;

   vec4 color = texture(tex,texCoords);

   if (color.a < 0.1)
      discard;


   vec3 final = color.rgb;

   final = texture(tex,vec2(0,1)).rgb;

   if (texture(tex,vec2(0,0)).r == 1) {
      final = vec3(0,1,0);
   }

   FragColor = vec4(final, color.a);
}
