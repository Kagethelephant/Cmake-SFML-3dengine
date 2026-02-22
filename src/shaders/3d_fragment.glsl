#version 330 core
#define MAX_LIGHTS 32

uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightCol[MAX_LIGHTS];
uniform vec3 objCol;
uniform sampler2D diffuseTex;

in vec3 fragPos;
in vec2 TexCoord;
in vec3 vecNorm;

out vec4 FragColor;

void main()
{

   float ambientStrength = 0.2;
   vec3 sum = vec3(0.0);

   for(int i = 0; i < lightCount; i ++){

      vec3 ambient = ambientStrength * lightCol[i];

      vec3 norm = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

      // vec3 norm = normalize(normal);
      vec3 lightDir = normalize(lightPos[i] - fragPos); 
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * lightCol[i];
      sum += diffuse + ambient;
   }
   // vec3 result = (ambient + diffuse) * objCol;

   vec4 newCol = texture(diffuseTex, TexCoord);


   vec4 result = vec4(sum,1.0) * newCol;
   // FragColor = vec4(result,1.0);
   FragColor = result;
}
