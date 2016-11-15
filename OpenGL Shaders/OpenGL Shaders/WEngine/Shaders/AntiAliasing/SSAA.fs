#version 400 core
//https://www.youtube.com/watch?v=Z9bYzpwVINA

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D colourTexture;

void main(void){

	out_Color = vec4(result2, texture2D(colourTexture, textureCoords.xy).a);
}