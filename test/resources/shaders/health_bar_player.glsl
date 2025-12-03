#define __STARTSHADER
#define __SHADERTYPE VTX

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

   texCoords.y = 1.0 - texCoords.y;

   normal = mat3(transpose(inverse(_internalModel))) * aNorm;
   pos = vec3(_internalModel * vec4(aPos, 1.0));
   gl_Position = _internalProj * _internalModel * vec4(aPos, 1.0);
}

#define __ENDSHADER



#define __STARTSHADER
#define __SHADERTYPE FRAG

#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 normal;
in vec3 pos;

#definclude "shaders/util.shdinc"

uniform sampler2D _mainTexture;
uniform vec3 _mainColor;

uniform float health;

void main()
{
   vec4 color = texture(_mainTexture,texCoords);

   vec3 final = color.rgb;

   if (color.r > 0.5f) {
		final = 0.5*color_vec3(_mainColor);
	} else {
		if (texCoords.x > health) {
			final = 0.25*color_vec3(_mainColor);
		} else {
			final = color_vec3(_mainColor);
			final *= clamp(texCoords.y+0.5f,0,1);
		}
	}

   FragColor = vec4(final,color.a);
}

#define __ENDSHADER