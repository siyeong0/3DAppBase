#version 330 core

uniform sampler2D albedo;

in vec3 normalWorld;
in vec2 pixTexcoord;
in vec3 tangentWorld;

out vec3 color;

void main()
{
	color = texture(albedo, pixTexcoord).rgb;
}