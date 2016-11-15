#version 330 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
in vec3 toLightVector[4];
in vec4 FragPosLightSpace;

in float visibility;

out vec4 out_Color;

uniform vec3 attenuation[4];

uniform vec3 skyColour;
uniform float ambientLight;

uniform sampler2D reflectionTexture0;
uniform sampler2D refractionTexture1;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D shadowMap;

uniform float moveFactor;

uniform vec3 lightColour[4];

uniform float shineDamper;
uniform float reflectivity;

const float waveStrength = 0.04;


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

	vec3 unitVectorToCamera = normalize(toCameraVector);

	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	float colll = texture(reflectionTexture0, reflectTexCoords).r;

	float near = 0.01;
	float far = 1000.0;

	float depth = texture(depthMap, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;

	vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 5.0, 0.0, 1.0);

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColour = texture(reflectionTexture0, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture1, refractTexCoords);

	vec4 normalMapColour = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b * 5.0, normalMapColour.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, 0.5);
	refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for(int i = 0; i < 4; i++){
		float distance = length(toLightVector[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * (distance * distance));

		vec3 unitLightVector = normalize(toLightVector[i]);

		float nDot1 = dot(normal, unitLightVector);
		float brightness = -max(nDot1, 0.0);

		vec3 lightDirection = unitLightVector;
		vec3 reflectedLightDirection = reflect(lightDirection, normal);

		float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);

		totalDiffuse = totalDiffuse + ((brightness * lightColour[i]) / attFactor);
		totalSpecular = totalSpecular + ((dampedFactor * reflectivity * lightColour[i]) / attFactor);
	}

	float shadow = ShadowCalculation(FragPosLightSpace, normal, normalize(toLightVector[0]));
	totalDiffuse = max(min(totalDiffuse, 0.2), ambientLight) * shadow;
	totalSpecular *= clamp(waterDepth / 5.0, 0.0, 1.0);

	out_Color = mix(reflectColour, refractColour, refractiveFactor);
	out_Color = vec4(totalDiffuse, 1.0) * mix(out_Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);

	if(shadow < 1.0) totalSpecular = vec3(0.0);
	if(shineDamper >= 0 && reflectivity >= 0) out_Color += vec4(totalSpecular, 1.0);

	
	out_Color.a = clamp(waterDepth / 3.0, 0.0, 1.0);
	//out_Color = vec4(waterDepth / 50.0);
	//out_Color = mix(vec4(skyColour, 1.0), out_Color, visibility);out_Color = vec4(vec3(visibility), 1.0);
	//out_Color = vec4(colll, colll, colll, 1.0);
}