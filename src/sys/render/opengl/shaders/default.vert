#version 330 core

// Positions
layout (location = 0) in vec3 aPos;
// Texture
layout (location = 1) in vec2 aTex;
// Normals
layout (location = 2) in vec3 aNormal;

// Output color to the frag shader
out vec3 color;
// Output texture coordinates to the frag shader
out vec2 texCoords;
// Output normals to the frag shader
out vec3 normal;
// Output position to the frag shader for light calcs
out vec3 currentPos;

// Matrices for 3D viewing
uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
    currentPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = camMatrix * vec4(currentPos, 1.0);
    
    // Set color and texCoords to pass to frag shader
    texCoords = aTex;
    normal = aNormal;
}