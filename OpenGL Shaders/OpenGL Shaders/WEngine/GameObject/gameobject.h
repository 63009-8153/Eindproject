#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

class gameobject
{
public:
	//Create or delete model stuff
	gameobject();
	gameobject(gameobject *object);
	gameobject(int ovaoID, int overtexCount);
	~gameobject();

	//Get Model stuff
	GLuint getVaoID();
	int getVertexCount();

	//Texture stuff
	void addTexture(GLuint textID);
	GLuint getTextureID(int position);

	void setNormalMap(GLuint textID);
	GLuint getNormalMapID();

	void setShadowMap(GLuint textID);
	GLuint getShadowMapID();
	
	void setSpecularMap(GLuint textID);
	GLuint getSpecularMapID();

	void setEnviromentCubeMapID(GLuint textID);
	GLuint getEnviromentCubeMapID();

	void setReflectionRatio(float value);
	void setReflectionRefractionRatio(float value);

	void setTextureAtlasTexture(GLuint textID, int nrRows, int textIndex);
	
	float getTextureXOffset();
	float getTextureYOffset();

	int getTextureAmount();

	bool useFakeLighting, cullFaces;
	float ambientLight;
	int numberOfRows, textureIndex;
	bool hasNormalMap, hasShadowMap, hasSpecularMap, 
		 hasReflectionCubeMap, hasRefractionCubeMap;

	float reflectionRatio,
		  reflectionRefractionRatio;

	void init(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal);
	void init();

	void Move(glm::vec3 _movement);
	void Rotate(glm::vec3 _rotation);

	void setAmbientLight(float ambient);

	void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	void setScale(glm::vec3 scal);
	void setShineDamper(float val);
	void setReflectivity(float val);

	float getAmbientLight();

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	float getShineDamper();
	float getReflectivity();

	void setTiledAmount(float amount);
	float getTiledAmount();

private:
	int vaoID;
	int vertexCount;

	glm::vec3 position, 
			  rotation,
			  scale;

	float tiledAmount;

	std::vector<GLuint> textureID;
	GLuint normalMapID,
		shadowMapID,
		specularMapID,
		enviromentCubeMap;

	float shineDamper;
	float reflectivity;
};

#endif
