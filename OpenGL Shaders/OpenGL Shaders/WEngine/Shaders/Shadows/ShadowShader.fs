#version 330 core

in vec2 pass_textureCoords;

out vec4 out_Color;

uniform sampler2D textureSampler0;

void main(void){

	float alpha = texture(textureSampler0, pass_textureCoords).a;
	if(alpha < 0.5){
		discard;
	}

	out_Color = vec4(1.0);
}