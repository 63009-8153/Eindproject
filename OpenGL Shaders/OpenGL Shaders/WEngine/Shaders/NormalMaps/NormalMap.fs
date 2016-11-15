#version 400 core

in vec2 pass_textureCoords;
in vec3 surfaceNormal;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;
in vec4 FragPosLightSpace;

out vec4 out_Color;

uniform sampler2D textureSampler0;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform sampler2D specularMap;
uniform float usesSpecularMap;

uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

uniform float ambientLight;

const float levels = 3.0;
const float useCelShading = 0;

//Uneven numbers. So 1, 3, 5, etc.
const float PCFFilterCount = 5.0;

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

	// Model normal
	vec3 unitNormal = normalize(surfaceNormal);
	
    // Get tangent space normal
    vec3 tangent_normal = normalize(texture(normalMap, pass_textureCoords).xyz * 2.0 - 1.0);
		
	// Calculate tangent
	vec3 tangent = normalize( max( cross(unitNormal, vec3(0.0, 0.0, 1.0)) , cross(unitNormal, vec3(0.0, 1.0, 0.0)) ) );
		
	// Calculate binormal
	vec3 binormal = normalize( cross(tangent_normal, tangent) );
	
	// Transform tangent space normal to world space
	unitNormal = tangent * tangent_normal.x + binormal * tangent_normal.y + unitNormal * tangent_normal.z;
	unitNormal = normalize(unitNormal);

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

	vec4 textureColour = texture(textureSampler0, pass_textureCoords);
	if(textureColour.a < 0.7){
		discard;
	}

	if(usesSpecularMap > 0.5){
		vec4 mapInfo = texture(specularMap, pass_textureCoords);
		totalSpecular *= mapInfo.r;
		if(mapInfo.g > 0.5){
			totalDiffuse = vec3(1.0);
		}
	}

	out_Color = (vec4(totalDiffuse, 1.0) * textureColour);

	if(shadow < 1.0) totalSpecular = vec3(0.0);
	if(shineDamper >= 0 && reflectivity >= 0) out_Color += (vec4(totalSpecular, 1.0));

	//out_Color = vec4(vec3(shadow), 1.0);
	out_Color = mix(vec4(skyColour, 1.0), out_Color, visibility);
	//out_Color = vec4(vec3(tangent),1.0);
}