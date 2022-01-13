/*
* Team Work
* Student Name:Wang Hong Bo
* Student ID:18098532-I011-0018
* 
* Student Name:Liu Jun Feng
* Student ID:18098537-I011-0067
* 
* Data:2020/12/12
*/
#version 430


layout (location = 1) in vec2 tex_coord;
layout (location = 0) in vec3 vertPos;
layout (location = 2) in vec3 vertNormal;
out vec2 tc;
out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec3 varyingHalfVector;
out vec4 varyingColor;

struct PositionalLight
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
struct Material
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

layout (binding=0) uniform sampler2D s;

void main(void)
{	varyingVertPos = (mv_matrix * vec4(vertPos,1.0)).xyz;
	varyingLightDir = light.position - varyingVertPos;
	varyingNormal = (norm_matrix * vec4(vertNormal,1.0)).xyz;
	
	//varyingHalfVector =normalize(normalize(varyingLightDir) + normalize(-varyingVertPos)).xyz;

	varyingHalfVector=normalize(normalize(varyingLightDir) + (-varyingVertPos)).xyz;

	gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);
	//varyingColor = vec4(vertPos,1.0)*0.5+vec4(0.5,0.5,0.5,0.5);gyn
    tc = tex_coord;
}
