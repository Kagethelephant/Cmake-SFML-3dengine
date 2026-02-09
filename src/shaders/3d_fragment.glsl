#version 330 core

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 objCol;
uniform sampler2D diffuseTex;

in vec3 fragPos;
in vec2 TexCoord;
in vec3 vecNorm;

out vec4 FragColor;

void main()
{

   float ambientStrength = 0.2;
   vec3 ambient = ambientStrength * lightCol;
   
   vec3 norm = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

   // vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos - fragPos); 
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightCol;

   // vec3 result = (ambient + diffuse) * objCol;

   vec4 newCol = texture(diffuseTex, TexCoord);


   vec4 result = vec4(ambient + diffuse,1.0) * newCol;
   // FragColor = vec4(result,1.0);
   FragColor = result;
}
