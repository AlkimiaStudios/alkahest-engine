#version 330 core

// output RGBA color
out vec4 FragColor;

// input from vert shader
in vec3 color;
in vec2 texCoords;

// Get texture from the CPU
uniform sampler2D tex0;

void main()
{
   FragColor = texture(tex0, texCoords);
}