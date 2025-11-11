#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;

uniform float _internalTime;

uniform vec3 color;

uniform sampler2DArray idleSprite;
uniform sampler2DArray walkSprite;
uniform sampler2DArray attackSprite;
uniform sampler2DArray guardSprite;

uniform vec2 moveDir;

uniform bool guarding;
uniform float attackTime;
uniform bool flip;

void main()
{
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(vec3(sin(_internalTime)*3,2,cos(_internalTime)*3) - pos);
   lightDir = -normalize(vec3(1,-1,-1));
   float d =max(dot(norm, lightDir), 0.0) + 0.25f;

   vec2 coords = texCoords;

   if (flip)
      coords.x *= -1; 

   vec4 color = vec4(1.0);

   if (length(moveDir) > 0.001) {
      color = texture(walkSprite,vec3(coords,mod(_internalTime*10.0f, 6)));
   } else {
      color = texture(idleSprite,vec3(coords,mod(_internalTime*10.0f, 6)));

      if (guarding) {
         color = texture(guardSprite,vec3(coords,mod(_internalTime*10.0f, 6)));
      }

   }
   float diff = _internalTime - attackTime;
   if (diff*10.0f < 4.0f) {
      color = texture(attackSprite,vec3(coords,mod(diff*10.0f, 4)));
   }

   vec3 final = color.rgb * d;

   FragColor = vec4(final, color.a);
}
