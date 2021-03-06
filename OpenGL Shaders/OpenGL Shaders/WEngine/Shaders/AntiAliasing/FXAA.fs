#version 400 core
//https://www.youtube.com/watch?v=Z9bYzpwVINA

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D colourTexture;
uniform vec3 inverseFilterTextureSize;

uniform float fxaa_span_max = 8.0;
uniform float fxaa_reduce_min = 1.0 / 128.0;
uniform float fxaa_reduce_mul = 1.0 / 8.0;

void main(void){

	vec3 luma = vec3(0.299, 0.587, 0.114);
	vec2 texCoordOffset = inverseFilterTextureSize.xy;
	
	float lumaTL = dot(luma, texture(colourTexture, textureCoords.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture(colourTexture, textureCoords.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture(colourTexture, textureCoords.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture(colourTexture, textureCoords.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture(colourTexture, textureCoords.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y =  ((lumaTL + lumaBL) - (lumaTR + lumaBR));

	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaa_reduce_mul * 0.25), fxaa_reduce_min);
	float inverseDirAjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(fxaa_span_max, fxaa_span_max), 
	      max(vec2(-fxaa_span_max, -fxaa_span_max), dir * inverseDirAjustment)) * texCoordOffset;

	vec3 result1 = (1.0 / 2.0) * (
			texture(colourTexture, textureCoords.xy + (dir * vec2(1.0 / 3.0 - 0.5))).xyz + 
			texture(colourTexture, textureCoords.xy + (dir * vec2(2.0 / 3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0 / 2.0) + (1.0 / 4.0) * (
			texture(colourTexture, textureCoords.xy + (dir * vec2(0.0 / 3.0 - 0.5))).xyz + 
			texture(colourTexture, textureCoords.xy + (dir * vec2(3.0 / 3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		out_Color = vec4(result1, texture(colourTexture, textureCoords.xy).a);
	else
		out_Color = vec4(result2, texture(colourTexture, textureCoords.xy).a);
}