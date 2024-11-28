#shader vertex
#version 330 core

layout (location = 0) in vec3 vPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 vNormal;

out vec3 color;
out vec2 TexCoord;

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

uniform vec3 view_pos;
uniform Directional_light d_light;
uniform Point_light p_light;
uniform Material material;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model_view;

vec3 dir_light_shading(Directional_light light, vec3 normal, vec3 view_dir)
{
	vec3 light_direction = normalize(-light.direction);

	//diffuse
	float diff = max(dot(normal, light_direction), 0.0);

	//specular
	vec3 reflect_dir = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shineness);

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}

vec3 point_light_shading(Point_light light, vec3 position, vec3 normal, vec3 view_dir)
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

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	vec3 specular = light.specular * spec * material.specular;
	
	ambient *= athenuation;
	diffuse *= athenuation;
	specular *= athenuation;
	
	return (ambient + diffuse + specular);
}

void main()
{
	gl_Position = proj * view * model_view * vec4(vPosition, 1.0);

	//gouraud shading
	vec3 position = vec3(model_view * vec4(vPosition, 1.0));
	vec3 norm = mat3(transpose(inverse(model_view))) * vNormal;
	vec3 normal = normalize(norm);
	vec3 view_dir = normalize(view_pos - position);

	color = dir_light_shading(d_light, normal, view_dir);
	color += point_light_shading(p_light, position, normal, view_dir);

	// for texture mapping
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#shader fragment
#version 330 core

in vec3 color;
in vec2 TexCoord;

out vec4 frag_color;

uniform vec3 object_color;
uniform sampler2D diffuseMap; // diffuse 하는 물체의 color * reflect coefficient
uniform bool diffuseToggle; // diffuse Mapping Toggle을 위한 변수

void main()
{
	vec3 c;
	// diffuse토글이 on인 경우에만 diffuse Map이 적용된다.
    if(diffuseToggle){ 
        c = texture(diffuseMap, TexCoord).rgb;
    }
    else{
		c = object_color;
    }
	frag_color = vec4(color * c, 1.0);
}