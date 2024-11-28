#shader vertex
#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 vNormal;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model_view;

void main()
{
	FragPos = vec3(model_view*vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model_view))) * vNormal;
	gl_Position = proj*view*model_view*vec4(position, 1.0);

	// for texture mapping
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
};

#shader fragment
#version 330 core

out vec4 FragColor;

struct Directional_light
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Point_light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float linear;
	float quadratic;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shineness;
};

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform vec3 view_pos;
uniform Material material;
uniform Directional_light d_light;
uniform Point_light p_light;

uniform sampler2D diffuseMap; // diffuse 하는 물체의 color * reflect coefficient
uniform sampler2D normalMap; // Normal mapping 하는 map
uniform bool diffuseToggle; // diffuse Mapping Toggle을 위한 변수
uniform bool normalToggle; // normal Mapping Toggle을 위한 변수

vec3 dir_light_shading(Directional_light light, vec3 normal, vec3 view_dir, vec3 ambient_material, vec3 diffuse_material)
{
	vec3 light_direction = normalize(-light.direction);

	//diffuse
	float diff = max(dot(normal, light_direction), 0.0);

	//specular
	vec3 reflect_dir = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shineness);

	vec3 ambient = light.ambient * ambient_material;
	vec3 diffuse = light.diffuse * diff * diffuse_material;
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}

vec3 point_light_shading(Point_light light, vec3 position, vec3 normal, vec3 view_dir, vec3 ambient_material, vec3 diffuse_material)
{
	vec3 light_dir = normalize(light.position - position);

	//diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	//specular
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shineness);
	//athenuation
	float distance = length(light.position - position);
	float athenuation = 1.0 / (1.0 + light.linear *  distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * ambient_material;
	vec3 diffuse = light.diffuse * diff * diffuse_material;
	vec3 specular = light.specular * spec * material.specular;
	
	ambient *= athenuation;
	diffuse *= athenuation;
	specular *= athenuation;
	
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 ambient_material;
    vec3 diffuse_material;

    // diffuse토글이 on인 경우에만 diffuse Map이 적용된다.
    if(diffuseToggle){ 
        ambient_material = texture(diffuseMap, TexCoord).rgb;
        diffuse_material = texture(diffuseMap, TexCoord).rgb;
    }
    else{
        ambient_material = material.ambient;
        diffuse_material = material.diffuse;
    }
    
    vec3 normal;

    // normal토글이 on인 경우에만 normal Map이 적용된다.
    if(normalToggle){
        normal = texture(normalMap, TexCoord).rgb; // obtain normal from normal map in range [0,1]
        normal = normalize(normal * 2.0 - 1.0); // transform normal vector to range [-1,1]
    }
    else{
        normal = normalize(Normal);
    }

	vec3 viewDir = normalize(view_pos - FragPos);
	
	vec3 result = dir_light_shading(d_light, normal, viewDir, ambient_material, diffuse_material);
	result += point_light_shading(p_light, FragPos, normal, viewDir, ambient_material, diffuse_material);
    FragColor = vec4(result, 1.0);
};
