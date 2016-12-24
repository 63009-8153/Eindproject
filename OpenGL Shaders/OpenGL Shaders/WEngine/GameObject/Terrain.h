#ifndef _TERRAIN_
#define _TERRAIN_

#include "gameobject.h"
#include "../Utils/loader.h"

#include "../Utils/File.h"

#define VERTEX_COUNT 126

#define TERRAINSIZE 800
#define MAX_HEIGHT 80
#define MAX_PIXEL_COLOUR 256*256*256

class Terrain
{
public:
	Terrain();
	~Terrain();

	void create(gameobject * model, glm::vec3 pos, glm::vec3 rot, GLuint textID0, GLuint textID1, GLuint textID2, GLuint textID3, GLuint textID4);
	void create(int gridX, int gridZ, Loader *loader, GLuint textID0, GLuint textID1, GLuint textID2, GLuint textID3, GLuint textID4);
	void createWithHeightmap(const char * heightmapPath, int gridX, int gridZ, Loader *loader, GLuint textID0, GLuint textID1, GLuint textID2, GLuint textID3, GLuint textID4);

	gameobject* getModel();

	gameobject generateTerrain(Loader *loader);
	gameobject generateTerrainWithHeightmap(Loader *loader, const char * heightmapPath);

	float getHeight(float worldX, float worldY);
private:
	gameobject TerrainObject;

	unsigned int heightmapWidth, heightmapHeight;
	std::vector<pixel> pixels;

	//Vector with all the heights.
	//Ordered in row0, row1, row2, etc.
	std::vector<float> heights;

	float getHeightOfPixelData(glm::vec2 pos);
	glm::vec3 calculateNormal(glm::vec2 pos);

	void loadHeightmap(const char * heightmapPath);
};

#endif // _TERRAIN_
