#version 450 core
in vec3 Normal;
in vec3 fragPos;
in vec2 v_TexCoords;
out vec4 fragColor;
uniform vec3 lampColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform bool lightSwitch;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct directional_light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct spot_light {
	vec3 position;
	vec3 direction;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};
struct point_light {
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};
uniform Material material;
uniform spot_light spotLight;
uniform directional_light directionalLight;
uniform point_light pointLight;

vec3 CalcDirectionalLight(directional_light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(point_light light, vec3 normol, vec3 viewDir);
vec3 CalcSpotLight(spot_light light, vec3 normal, vec3 viewDir, bool light_switch);

void main(){
	vec3 result=vec3(0.0f);
	vec3 viewDir = normalize(vec3(viewPos - fragPos));

	//directional light
	result += CalcDirectionalLight(directionalLight, Normal, viewDir);
	//point light
	result += CalcPointLight(pointLight, Normal, viewDir);
	//spotlight
	result += CalcSpotLight(spotLight, Normal, viewDir,lightSwitch);
	fragColor = vec4(result,1.0f);
}

vec3 CalcDirectionalLight(directional_light light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);

	vec3 ambient = light.ambient*vec3(texture(material.diffuse, v_TexCoords));
	vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse, v_TexCoords));
	vec3 specular = light.specular*spec*vec3(texture(material.specular, v_TexCoords));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(point_light light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(lightDir, Normal), 0.0f);
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic * distance*distance);
	vec3 diffuse = light.diffuse*diff*attenuation*vec3(texture(material.diffuse, v_TexCoords));
	vec3 specular = light.specular*spec*attenuation*vec3(texture(material.specular, v_TexCoords));

	return (diffuse + specular);
}
vec3 CalcSpotLight(spot_light light, vec3 normal, vec3 viewDir,bool light_switch) {
	light_switch = lightSwitch;
	vec3 lightDir = normalize(light.position-fragPos);
	float theta = dot(normalize(light.direction), -lightDir);
	
	if (theta > cos(light.outerCutOff)&&lightSwitch) {
		float diff = max(dot(lightDir, Normal), 0.0f);
		vec3 reflectDir = reflect(-lightDir, Normal);
		float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);

		float distance = length(light.position - fragPos);
		float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic * distance*distance);
		vec3 diffuse = light.diffuse*diff*attenuation*vec3(texture(material.diffuse, v_TexCoords));
		vec3 specular = light.specular*spec*attenuation*vec3(texture(material.specular, v_TexCoords));

		float cutoff_attenuation = min(1.0f, (dot(-lightDir, light.direction) - cos(light.outerCutOff)) / (cos(light.cutOff) - cos(light.outerCutOff)));
		diffuse *= cutoff_attenuation;
		specular *= cutoff_attenuation;
		return (diffuse + specular);
	}
	else return vec3(0.0f);
}