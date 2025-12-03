#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;
uniform sampler2D tex;
uniform vec3 _cameraPosition;

#definclude "shaders/util.shdinc"


void main()
{
   vec4 color = texture(tex,texCoords);

   if (color.a < 0.5f) {
      discard;
   }

   float dist = (distance(pos,_cameraPosition) - 8.f)/32.f;

   color.a = 1.0 - clamp(dist,0,1);

   FragColor = vec4(color.rgb,color.a);
}
