#version 330 core

struct Material {
	vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec3 computeDirLight(DirLight light, vec3 normal, vec3 viewDir);

in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform Material material;
uniform DirLight dirLight;
uniform vec3 viewPos;

void main() {
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = computeDirLight(dirLight, norm, viewDir);
	FragColor = vec4(result, 1.0);
}

vec3 computeDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * material.color;
	vec3 diffuse = light.diffuse * diff * material.color;
	vec3 specular = light.specular * spec * material.color;

	return (ambient + diffuse + specular);
}