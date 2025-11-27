#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;
uniform sampler2D tex;

#definclude "shaders/util.shdinc"


void main()
{
   vec4 color = texture(tex,texCoords);

   if (color.a < 0.5f) {
      discard;
   }

   color.rgb *= (color_vec3(155,185,78))*0.9;

   FragColor = vec4(color.rgb,color.a);
}
