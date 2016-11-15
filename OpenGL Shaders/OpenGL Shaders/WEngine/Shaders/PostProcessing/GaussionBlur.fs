#version 140

in vec2 textureCoords[11];

out vec4 out_Colour;

uniform sampler2D colourTexture;

void main(void){
	
	//Blur pixels and multiply them by their weight
	out_Colour = vec4(0.0);
	out_Colour += texture(colourTexture, textureCoords[0]) * 0.0093;
    out_Colour += texture(colourTexture, textureCoords[1]) * 0.028002;
    out_Colour += texture(colourTexture, textureCoords[2]) * 0.065984;
    out_Colour += texture(colourTexture, textureCoords[3]) * 0.121703;
    out_Colour += texture(colourTexture, textureCoords[4]) * 0.175713;
    out_Colour += texture(colourTexture, textureCoords[5]) * 0.198596;
    out_Colour += texture(colourTexture, textureCoords[6]) * 0.175713;
    out_Colour += texture(colourTexture, textureCoords[7]) * 0.121703;
    out_Colour += texture(colourTexture, textureCoords[8]) * 0.065984;
    out_Colour += texture(colourTexture, textureCoords[9]) * 0.028002;
    out_Colour += texture(colourTexture, textureCoords[10]) * 0.0093;
}