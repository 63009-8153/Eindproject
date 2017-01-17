#version 400

in vec3 textureCoords;

out vec4 out_Colour;

uniform samplerCube textureSampler0;
uniform vec3 skyColour;

const float lowerLimit = 80.0;
const float upperLimit = 120.0;

void main(void){
	vec4 finalColour = texture(textureSampler0, textureCoords);
	
	float factor = (textureCoords.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	out_Colour = mix(vec4(skyColour, 1.0), finalColour, factor);
}