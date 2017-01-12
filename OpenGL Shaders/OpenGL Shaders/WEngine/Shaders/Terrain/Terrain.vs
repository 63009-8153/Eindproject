#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;

out vec2 pass_textureCoords;
out vec3 surfaceNormal;
out vec3 toLightVector[4];
out vec3 toCameraVector;
out float visibility;
out vec4 FragPosLightSpace;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec4 plane;

const float lowerLimit = 80.0;
const float upperLimit = 190.0;

// The larger this is, the denser the fog is
const float density = 0.006;
// The lower this is the larger the gradient is
const float gradient = 3.5;

uniform vec3 lightPosition[4];

uniform mat4 lightSpaceMatrix;

void main(void){

	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);
	FragPosLightSpace = lightSpaceMatrix * vec4(vec3(worldPosition), 1.0);

	gl_ClipDistance[0] = dot(worldPosition, plane);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_textureCoords = textureCoords;

	surfaceNormal = (transformationMatrix * vec4(normal, 0.0f)).xyz;
	for(int i = 0; i < 4; i++){
		toLightVector[i] = lightPosition[i] - worldPosition.xyz;
	}
	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;

	float factor = (worldPosition.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, factor, 1.0);
}