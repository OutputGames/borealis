#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex;
uniform float health;
uniform vec3 _color;

uniform float _internalTime;
void main()
{
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(vec3(sin(_internalTime)*3,2,cos(_internalTime)*3) - pos);
   lightDir = -normalize(vec3(1,-1,-1));
   float d = max(dot(norm, lightDir), 0.0) + 0.25f;

   vec4 color = texture(tex,texCoords);

	if (color.a < 0.1) discard;

	if (color.r > 0.5f) {
		color.rgb = vec3(0.125f);
	} else {
		if (texCoords.x > health) {
			color.rgb = vec3(0.25f);
		} else {
			color.rgb = _color;
			color.rgb *= clamp(texCoords.y+0.25f,0,1);
		}
	}
 

   vec3 final = color.rgb;

   FragColor = vec4(final, color.a);
}
