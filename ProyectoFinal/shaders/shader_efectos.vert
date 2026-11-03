#version 330
// shader de los efectos transparentes (escudos de Reach, humo y llama del cohete): textura con color, transparencia y desplazamiento, sin luces
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec2 offsetEfecto;	// recorre los cuadros de una textura animada (humo y llama)

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	TexCoord = tex + offsetEfecto;
}
