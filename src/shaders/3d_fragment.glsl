#version 330 core
#define MAX_LIGHTS 32

uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightCol[MAX_LIGHTS];
uniform vec3 objCol;
uniform uint hasTexture;
uniform sampler2D diffuseTex;

in vec3 fragPos;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{

   float ambientStrength = 0.2;
   vec3 sum = vec3(0.0);
   vec4 newCol = vec4(0,0,0,0);


   for(int i = 0; i < lightCount; i ++){

      vec3 ambient = ambientStrength * lightCol[i];

      vec3 norm = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

      vec3 lightDir = normalize(lightPos[i] - fragPos); 
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * lightCol[i];
      sum += diffuse + ambient;
   }

   if (hasTexture == 1u){
      newCol = texture(diffuseTex, TexCoord);
   }
   else {
      newCol = vec4(objCol,1.0);
   }

   vec4 result = vec4(sum,1.0) * newCol;
   FragColor = result;
}
