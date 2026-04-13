#version 330 core

in vec3 FragPos;  
in vec3 Normal;

out vec4 FragColor;

uniform vec3 color;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform DirLight dirLight;
uniform vec3 viewPos;
uniform Material material;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 norm = normalize(Normal);

    vec3 ambient = dirLight.ambient * material.diffuse;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * dirLight.diffuse * material.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess*256);
    vec3 specular = spec * dirLight.specular * material.specular;

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec3 lighting = (ambient + diffuse + specular);
    FragColor = vec4(lighting, 1.0);
}
