#version 330
in vec2 TexCoord;
out vec4 color;

uniform sampler2D theTexture;
uniform vec3 colorEfecto;
uniform float alphaEfecto;	// el main la cambia en cada cuadro (el escudo pulsa)

void main()
{
	color = texture(theTexture, TexCoord) * vec4(colorEfecto, 1.0);
	color.a = color.a * alphaEfecto;
}
