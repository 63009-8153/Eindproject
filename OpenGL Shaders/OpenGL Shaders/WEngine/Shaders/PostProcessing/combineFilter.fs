#version 400

in vec2 textureCoords;

out vec4 out_Colour;

uniform sampler2D colourTexture;
uniform sampler2D anotherTexture;

uniform float highlightAmount;

void main(void){
	
	vec4 sceneColour = texture(colourTexture, textureCoords);
	vec4 highlightColour = texture(anotherTexture, textureCoords);

	out_Colour = sceneColour + (highlightColour * highlightAmount);
	//out_Colour = highlightColour;
}