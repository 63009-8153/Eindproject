#version 330 core

in vec2 position;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCameraVector;

out vec3 toLightVector[4];

out float visibility;
out vec4 FragPosLightSpace;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec3 cameraPosition;
uniform vec3 lightPosition[4];

uniform mat4 lightSpaceMatrix;

const float density = 0.0005;
const float gradient = 4.5;

const float tiling = 16.0;

void main(void){

	vec4 worldPosition = transformationMatrix * vec4(position.x, 0.0, position.y, 1.0);
	FragPosLightSpace = lightSpaceMatrix * vec4(vec3(worldPosition), 1.0);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	clipSpace = projectionMatrix * positionRelativeToCam;
	gl_Position = clipSpace;

	textureCoords = vec2(position.x / 2.0 + 0.5, position.y / 2.0 + 0.5) * tiling;
	toCameraVector = cameraPosition - worldPosition.xyz;

	for(int i = 0; i < 4; i++){
		toLightVector[i] = worldPosition.xyz - lightPosition[i];
	}

	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}