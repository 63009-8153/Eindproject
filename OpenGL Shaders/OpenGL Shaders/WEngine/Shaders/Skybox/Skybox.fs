#version 400

in vec3 textureCoords;

out vec4 out_Colour;

uniform samplerCube textureSampler0;

void main(void){

	out_Colour = texture(textureSampler0, textureCoords);
}