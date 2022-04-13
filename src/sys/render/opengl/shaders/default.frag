#version 330 core

// output RGBA color
out vec4 FragColor;

// input from vert shader
in vec3 color;
in vec2 texCoords;
in vec3 normal;
in vec3 currentPos;

// Get texture from the CPU
uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
   // Ambient lighting constant
   float ambient = 0.2;

   // Calculate diffuse lighting
   vec3 nNormal = normalize(normal);
   vec3 lightDir = normalize(lightPos - currentPos);
   float diffuse = max(dot(nNormal, lightDir), 0.0);

   // Calculate specular lighting
   float specularLight = 0.5;
   vec3 viewDir = normalize(camPos - currentPos);
   vec3 reflectDir = reflect(-lightDir, nNormal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 8);
   float specular = specAmount * specularLight;

   // Add lighting to texture
   FragColor = texture(tex0, texCoords) * lightColor * (diffuse + ambient + specular);
}