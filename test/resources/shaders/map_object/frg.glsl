#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec4 pos;
uniform vec3 _color;
uniform sampler2D tex;
uniform float _internalTime;

#definclude "shaders/util.shdinc"

void main()
{
   vec4 color = texture(tex,texCoords);

   if (color.a < 0.1)
      discard;


   vec3 final = color.rgb;

    ivec2 textureDimensions = textureSize(tex, 0) - ivec2(1,1);
    float normalizedX = 0 * float(textureDimensions.x);
    float normalizedY = 1 * float(textureDimensions.y);

   vec4 texel = texelFetch(tex, ivec2(normalizedX,normalizedY),0);
   float texelType = texel.r;

   texelType *= (1.0 - texel.g);
   texelType *= (1.0 - texel.b);

   if (texel.g > 0.25 || texel.b > 0.25)
      texelType = 0;

   if (texelType > 0) {
      float mul = 1;

      mul = calculateLuminance(color.rgb);

      final = blendHardLight(vec3(mul), color_vec3(_color),0.75f) * color.r;
   }

   FragColor = vec4(final, color.a);
}
