#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;

uniform mat4 viewProj;
uniform mat4 world;

out vec3 normalWorld;
out vec2 pixTexcoord;
out vec3 tangentWorld;


void main()
{
	mat4 mvp = viewProj * world;
	gl_Position =  mvp * vec4(position, 1.0);
	normalWorld = (mvp * vec4(normal,1.0)).xyz;
	pixTexcoord = texcoord;
	tangentWorld = (mvp * vec4(tangent,1.0)).xyz;
}