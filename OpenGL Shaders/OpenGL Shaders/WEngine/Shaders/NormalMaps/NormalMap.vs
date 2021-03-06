#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;

out vec2 pass_textureCoords;
out vec3 surfaceNormal;
out vec3 toLightVector[4];
out vec3 toCameraVector;
out float visibility;
out vec3 pass_tangent;
out vec4 FragPosLightSpace;
out vec3 viewVector;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float useFakeLighting;

uniform vec3 lightPosition[4];

uniform float numberOfRows;
uniform vec2 offset;

uniform vec4 plane;

uniform mat4 lightSpaceMatrix;

uniform vec3 cameraPosition;

const float lowerLimit = 80.0;
const float upperLimit = 190.0;

// The larger this is, the denser the fog is
const float density = 0.006;
// The lower this is the larger the gradient is
const float gradient = 3.5;

void main(void){

	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);
	FragPosLightSpace = lightSpaceMatrix * vec4(vec3(worldPosition), 1.0);

	gl_ClipDistance[0] = dot(worldPosition, plane);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_textureCoords = (textureCoords / numberOfRows) + offset;

	vec3 originalNormal = normal;
	if(useFakeLighting > 0.5) originalNormal = vec3(0.0, 1.0, 0.0);

	surfaceNormal = (transformationMatrix * vec4(originalNormal, 0.0f)).xyz;
	for(int i = 0; i < 4; i++){
		toLightVector[i] = lightPosition[i] - worldPosition.xyz;
	}
	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;

	float factor = (worldPosition.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, factor, 1.0);

	viewVector = normalize(position - cameraPosition);
}