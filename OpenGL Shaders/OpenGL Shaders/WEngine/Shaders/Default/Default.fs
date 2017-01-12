#version 400 core

in vec2 pass_textureCoords;
in vec3 surfaceNormal;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;
in vec4 FragPosLightSpace;
in vec3 reflectedVector;
in vec3 refractedVector;

out vec4 out_Color;

uniform sampler2D textureSampler0;
uniform sampler2D shadowMap;

uniform sampler2D specularMap;
uniform float usesSpecularMap;

uniform samplerCube enviromentCubeMap;
uniform float usesReflectionCubeMap;
uniform float usesRefractionCubeMap;
uniform float reflectionRefractionRatio;
uniform float reflectionColourRatio;

uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

uniform float ambientLight;

uniform vec2 tileAmount;

const float levels = 3.0;
const float useCelShading = 0;

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

	float level;

	vec2 textureCoords = pass_textureCoords;
	vec2 tiledAmount = tileAmount;
	if(tiledAmount.x < 1.0) tiledAmount.x = 1.0;
	if(tiledAmount.y < 1.0) tiledAmount.y = 1.0;
	textureCoords *= tiledAmount;

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitVectorToCamera = normalize(toCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for(int i = 0; i < 4; i++){
		float distance = length(toLightVector[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * (distance * distance));

		vec3 unitLightVector = normalize(toLightVector[i]);

		float nDot1 = dot(unitNormal, unitLightVector);
		float brightness = max(nDot1, 0.0);

		if(useCelShading > 0){
			level = floor(brightness * levels);
			brightness = level / levels;
		}
		
		vec3 lightDirection = -unitLightVector;
		vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);

		float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);

		if(useCelShading > 0){
			level = floor(dampedFactor * levels);
			dampedFactor = level / levels;
		}

		totalDiffuse = totalDiffuse + ((brightness * lightColour[i]) / attFactor);
		totalSpecular = totalSpecular + ((dampedFactor * reflectivity * lightColour[i]) / attFactor);
	}
	
	float shadow = ShadowCalculation(FragPosLightSpace, unitNormal, normalize(toLightVector[0]));
	totalDiffuse = max(totalDiffuse, ambientLight) * shadow;

	vec4 textureColour = texture(textureSampler0, textureCoords);
	if(textureColour.a < 0.7){
		discard;
	}

	if(usesSpecularMap > 0.5){
		vec4 mapInfo = texture(specularMap, textureCoords);
		totalSpecular *= mapInfo.r;

		if(mapInfo.g > 0.1){
			totalDiffuse = vec3(1.0) * mapInfo.g;
		}
	}

	out_Color = vec4(totalDiffuse, 1.0) * textureColour;

	//Reflection and refraction
	if(shadow < 1.0) totalSpecular = vec3(0.0);
	if(shineDamper >= 0 && reflectivity >= 0) out_Color += vec4(totalSpecular, 1.0);

	vec4 reflecRefractColour;
	if(usesReflectionCubeMap > 0.5){
		reflecRefractColour = texture(enviromentCubeMap, reflectedVector);
	}
	if(usesRefractionCubeMap > 0.5){
		vec4 refractedColour = texture(enviromentCubeMap, refractedVector);
		reflecRefractColour = mix(reflecRefractColour, refractedColour, reflectionRefractionRatio);
	}
	if(usesReflectionCubeMap > 0.5) out_Color = mix(out_Color, reflecRefractColour, reflectionColourRatio);
	//out_Color = reflecRefractColour;

	out_Color = mix(vec4(skyColour, 1.0), out_Color, visibility);
}