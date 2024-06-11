#version 420 compatibility
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 u_PM;
uniform mat4 u_VM;
uniform mat4 u_MM; 

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	FragPos = vec3(u_MM * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_MM))) * aNormal;  
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	gl_Position = u_PM * u_VM * u_MM * vec4(aPos, 1.0);
}