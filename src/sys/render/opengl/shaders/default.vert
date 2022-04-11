#version 330 core

// Positions
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
// Texture
layout (location = 2) in vec2 aTex;

// Output color to the frag shader
out vec3 color;
// Output texture coordinates to the frag shader
out vec2 texCoords;

// Matrices for 3D viewing
uniform mat4 viewMatrix;

void main()
{
    gl_Position = viewMatrix * vec4(aPos, 1.0);
    // Set color and texCoords to pass to frag shader
    color = aColor;
    texCoords = aTex;
}