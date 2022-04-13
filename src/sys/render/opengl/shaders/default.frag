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
uniform sampler2D tex1;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 pointLight()
{
   // Light vector
   vec3 lightVec = lightPos - currentPos;

   // Intensity of the light
   float dist = length(lightVec);
   float a = 1.0;
   float b = 0.04;
   float inten = 1.0 / (a * dist * dist + b * dist + 1.0);

   // Ambient lighting constant
   float ambient = 0.2;

   // Calculate diffuse lighting
   vec3 nNormal = normalize(normal);
   vec3 lightDir = normalize(lightVec);
   float diffuse = max(dot(nNormal, lightDir), 0.0);

   // Calculate specular lighting
   float specularLight = 0.5;
   vec3 viewDir = normalize(camPos - currentPos);
   vec3 reflectDir = reflect(-lightDir, nNormal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 16);
   float specular = specAmount * specularLight;

   return (texture(tex0, texCoords) * (diffuse * inten + ambient) + texture(tex1, texCoords).r * specular * inten) * lightColor;
}

vec4 directionalLight()
{
   // Ambient lighting constant
   float ambient = 0.2;

   // Calculate diffuse lighting
   vec3 nNormal = normalize(normal);
   vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));
   float diffuse = max(dot(nNormal, lightDir), 0.0);

   // Calculate specular lighting
   float specularLight = 0.5;
   vec3 viewDir = normalize(camPos - currentPos);
   vec3 reflectDir = reflect(-lightDir, nNormal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 16);
   float specular = specAmount * specularLight;

   return (texture(tex0, texCoords) * (diffuse + ambient) + texture(tex1, texCoords).r * specular) * lightColor;
}

vec4 spotLight()
{
   // Using two cones for a smooth gradient
   float outerCone = 0.9;
   float innerCone = 0.95;

   // Light vector
   vec3 lightVec = lightPos - currentPos;

   // Ambient lighting constant
   float ambient = 0.2;

   // Calculate diffuse lighting
   vec3 nNormal = normalize(normal);
   vec3 lightDir = normalize(lightVec);
   float diffuse = max(dot(nNormal, lightDir), 0.0);

   // Calculate specular lighting
   float specularLight = 0.5;
   vec3 viewDir = normalize(camPos - currentPos);
   vec3 reflectDir = reflect(-lightDir, nNormal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 16);
   float specular = specAmount * specularLight;

   float angle = dot(vec3(0.0, -1.0, 0.0), -lightDir);
   float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);

   return (texture(tex0, texCoords) * (diffuse * inten + ambient) + texture(tex1, texCoords).r * specular * inten) * lightColor;
}

void main()
{
   // Add lighting to texture
   FragColor = pointLight();
}