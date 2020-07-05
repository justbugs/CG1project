#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D ourTexture;
uniform sampler2D normalMap;
uniform bool nm;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main()
{    
    
   // Properties
    // vec3 norm = texture(normalMap, fs_in.TexCoords).rgb;
    // norm = normalize(norm * 2.0 - 1.0);   
    // norm = normalize(fs_in.TBN * norm);
    vec3 norm;
    if (!nm)
        norm = normalize(fs_in.Normal);
    else{
        norm = texture(normalMap, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);   
        norm = normalize(fs_in.TBN * norm);
    }
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // Phase 2: Point lights
    for(int i = 0; i <1; i++)
        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);        
    
    FragColor = vec4(result, 1.0);
    
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Fill these functions. And use them in main.                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// calculates the color when using a directional light.

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)

{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float NdotL = max(0,dot(viewDir,normal));
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    if (NdotL < 0.26){
        return vec3(0,0,0);
    }
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    // vec3 ambient = light.ambient*material.ambient*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2)) ;
    // vec3 diffuse = light.diffuse * diff*material.diffuse*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2)) ;
    // vec3 specular = light.specular * spec*material.specular*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2));
    vec3 ambient = light.ambient*material.ambient*vec3(texture(ourTexture, fs_in.TexCoords));
    diff= ((smoothstep(0.206,0.226,diff))+(smoothstep(0.706,0.726,diff)))/2 ;
    vec3 diffuse = light.diffuse * diff*material.diffuse*vec3(texture(ourTexture, fs_in.TexCoords)) ;
    vec3 specular = light.specular * spec*material.specular*vec3(texture(ourTexture, fs_in.TexCoords));
    // vec3 ambient = light.ambient*material.ambient;
    // vec3 diffuse = light.diffuse * diff*material.diffuse;
    // vec3 specular = light.specular * spec*material.specular;
    return (ambient + diffuse + specular);
}



// calculates the color when using a point light.

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float NdotL = max(0,dot(viewDir,normal));
    if (NdotL < 0.26){
        return vec3(0,0,0);
    }
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    // vec3 ambient = light.ambient*material.ambient*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2)) ;
    // vec3 diffuse = light.diffuse * diff*material.diffuse*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2)) ;
    // vec3 specular = light.specular * spec*material.specular*vec3(mix(texture(ourTexture, fs_in.TexCoords), texture(normalMap, fs_in.TexCoords), 0.2));
    vec3 ambient = light.ambient*material.ambient*vec3(texture(ourTexture, fs_in.TexCoords));
    diff = ((smoothstep(0.206,0.226,diff))+(smoothstep(0.706,0.726,diff)))/2 ;
    vec3 diffuse = light.diffuse * diff*material.diffuse*vec3(texture(ourTexture, fs_in.TexCoords)) ;
    vec3 specular = light.specular * spec*material.specular*vec3(texture(ourTexture, fs_in.TexCoords));
    // vec3 ambient = light.ambient*material.ambient;
    // vec3 diffuse = light.diffuse * diff*material.diffuse;
    // vec3 specular = light.specular * spec*material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse );
}


