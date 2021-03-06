#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include "../Renderer/ShaderProgram.h"
#include "../Camera/camera.h"

extern glm::vec3 clearColor;

class gameobject
{
public:
	//Create or delete model stuff
	gameobject();
	gameobject(gameobject *object);
	gameobject(int ovaoID, int overtexCount);
	gameobject(GLfloat positions[], int posCount, GLuint indices[], int indicesCount, GLfloat uvs[], int uvsSize, GLfloat normals[], int normalsSize, GLfloat tangents[], int tangentsSize);
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

	void setTiledAmount(glm::vec2 amount);
	glm::vec2 getTiledAmount();

	void Draw(ShaderProgram &shader, std::vector<Light*> lights, Camera *camera, glm::vec4 clipPlane);

	bool needsObject = false;

	GLfloat *vertices;
	int vertexCount;
	GLuint *indices;
	int indicesCount;
	GLfloat *uvs;
	int uvsSize;
	GLfloat *normals;
	int normalsSize;
	GLfloat *tangents;
	int tangentsSize;

private:
	int vaoID;

	glm::vec3 position, 
			  rotation,
			  scale;

	glm::vec2 tiledAmount;

	std::vector<GLuint> textureID;
	GLuint normalMapID,
		shadowMapID,
		specularMapID,
		enviromentCubeMap;

	float shineDamper;
	float reflectivity;
};

#endif
