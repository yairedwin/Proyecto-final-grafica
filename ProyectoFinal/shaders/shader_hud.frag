#version 330
in vec2 TexCoord;
out vec4 color;

uniform sampler2D theTexture;

void main()
{
	color = texture(theTexture, TexCoord);
	if (color.a < 0.02) discard;	// las esquinas redondeadas del panel son transparentes
}
