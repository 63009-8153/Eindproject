#version 400 core

in vec2 pass_textureCoords;
in vec3 surfaceNormal;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;
in vec4 FragPosLightSpace;

out vec4 out_Color;

uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;
uniform sampler2D textureSampler3;
uniform sampler2D textureSampler4;
uniform sampler2D shadowMap;
uniform sampler2D specularMap;
uniform float usesSpecularMap;

uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

uniform float ambientLight;

const float tileAmount = 200.0;

//Uneven numbers. So 1, 3, 5, etc.
const float PCFFilterCount = 3.0;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; 

	float currentDepth = projCoords.z;  

	float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.006);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	int FilterSize = int(floor(PCFFilterCount / 2));
	for(int x = -FilterSize; x <= FilterSize; ++x){
		for(int y = -FilterSize; y <= FilterSize; ++y){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			if(currentDepth - bias > pcfDepth){
				shadow += (1.0 - 0.4);
			}else{
				shadow += 1.0;
			}
		}    
	}
	shadow /= (PCFFilterCount * PCFFilterCount);

	return shadow;
}

void main(void){

	vec4 blendMapColour = texture(textureSampler4, pass_textureCoords);

	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = pass_textureCoords * tileAmount;

	vec4 texture0Colour = texture(textureSampler0, tiledCoords) * backTextureAmount;
	vec4 textureRColour = texture(textureSampler1, tiledCoords) * blendMapColour.r;
	vec4 textureGColour = texture(textureSampler2, tiledCoords) * blendMapColour.g;
	vec4 textureBColour = texture(textureSampler3, tiledCoords) * blendMapColour.b;

	vec4 totalColour = texture0Colour + textureRColour + textureGColour + textureBColour;

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitVectorToCamera = normalize(toCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for(int i = 0; i < 4; i++){
		float distance = length(toLightVector[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);

		vec3 unitLightVector = normalize(toLightVector[i]);

		float nDot1 = dot(unitNormal, -unitLightVector);
		float brightness = max(nDot1, 0.0);
		brightness /= 2;
		brightness *= -1;
		brightness += 1;

		vec3 lightDirection = unitLightVector;
		vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);

		float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);
		totalDiffuse = totalDiffuse + (brightness * lightColour[i]) / attFactor;
		totalSpecular = totalSpecular + (dampedFactor * reflectivity * lightColour[i]) / attFactor;
	}
	
	float shadow = ShadowCalculation(FragPosLightSpace, unitNormal, normalize(toLightVector[0]));
	totalDiffuse = max(totalDiffuse, ambientLight) * shadow;
	
	if(usesSpecularMap > 0.5){
		vec4 mapInfo = texture(specularMap, pass_textureCoords);
		totalSpecular *= mapInfo.r;
		if(mapInfo.g > 0.5){
			totalDiffuse = vec3(1.0);
		}
	}	

	out_Color = vec4(totalDiffuse, 1.0) * totalColour;
	if(shineDamper >= 0 && reflectivity >= 0) out_Color += vec4(totalSpecular, 1.0);

	//out_Color = vec4(vec3(shadow), 1.0);
	out_Color = mix(vec4(skyColour, 1.0), out_Color, visibility);
	//out_Color = vec4(vec3(visibility),1.0);
}