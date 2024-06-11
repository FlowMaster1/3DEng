#version 420 compatibility

#define MAX_TEXTURES 16
#define MAX_LIGHTS 32

struct Material{
vec3 Diffuse;
vec3 Specular;
vec3 Ambient;
};

struct MatTex{
    sampler2D texture_diffuse;
    sampler2D texture_specular;
};

struct DirLight{
    vec3 direction;

    vec3 Diffuse;
    vec3 Specular;
    vec3 Ambient;
};

struct PointLight{
    bool enable;
    vec3 position;
    
    vec3 Diffuse;
    vec3 Specular;
    vec3 Ambient;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    bool enable;
    vec3 position;
    vec3 direction;

    float cutoff;
    float outerCutoff;
  
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
	
    float constant;
    float linear;
    float quadratic;
};


out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform bool UseMaterial;
uniform bool EnableDirL;

uniform int NumOfPoints;
uniform int NumOfSpot;

uniform float Shininess;

uniform vec3 viewPos;

uniform Material material;
uniform DirLight directionL;
uniform PointLight pointL[MAX_LIGHTS];
uniform SpotLight spotL[MAX_LIGHTS];
uniform MatTex materialT;

vec3 CalcDirLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  SpotLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse,specular,ambient;
    vec3 result = vec3(0.0);

    if(UseMaterial){
        diffuse = material.Diffuse;
        specular = material.Specular;
        ambient = material.Ambient;
    }
    else{
        diffuse = vec3(texture(materialT.texture_diffuse,TexCoord));
        specular = vec3(texture(materialT.texture_specular,TexCoord));
        ambient = diffuse;
    }
    
    //Directional Light
    if(EnableDirL){
        result = CalcDirLight(diffuse,specular,ambient,
                          directionL,norm,viewDir);
    }

    for(int i = 0; i < NumOfPoints; i++){
        if(pointL[i].enable){
            result += CalcPointLight(diffuse,specular,ambient,
                                     pointL[i],norm,viewDir); 
        }
    }

    for(int i = 0; i < NumOfSpot; i++){
       if(spotL[i].enable){
       result += CalcSpotLight(diffuse,specular,ambient,
                                spotL[i],norm,viewDir); 
       }
    }
    
    FragColor = vec4(result,1.0);
    
}



vec3 CalcDirLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  DirLight light, vec3 normal, vec3 viewDir){

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 12);
    // combine results
    vec3 ambient = light.Ambient * MatAmbient;
    vec3 diffuse = light.Diffuse * diff * MatDiff;
    vec3 specular = light.Specular * spec * MatSpec;
    return (ambient + diffuse + specular);

}

vec3 CalcPointLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  PointLight light, vec3 normal, vec3 viewDir){
 
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 12);

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                        light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.Ambient * MatAmbient;
    vec3 diffuse = light.Diffuse * diff * MatDiff;
    vec3 specular = light.Specular * spec * MatSpec;

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(vec3 MatDiff, vec3 MatSpec, vec3 MatAmbient, 
                  SpotLight light, vec3 normal, vec3 viewDir){

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                        light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.Ambient * MatAmbient;
    vec3 diffuse = light.Diffuse * diff * MatDiff;
    vec3 specular = light.Specular * spec * MatSpec;

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutoff - light.outerCutoff);
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);


    ambient  *= attenuation; 
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;   
    
    return (ambient + diffuse + specular);
}