#version 330
// shader para el menu de teclas: solo dibuja la textura en pantalla, sin luces
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model * vec4(pos, 1.0);
	TexCoord = tex;
}
