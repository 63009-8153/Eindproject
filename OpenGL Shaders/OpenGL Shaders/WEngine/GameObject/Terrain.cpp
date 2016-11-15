#include "../header.h"

#include "Terrain.h"

Terrain::Terrain(){}
Terrain::~Terrain(){}

//Create a new flat terrain
void Terrain::create(int gridX, int gridZ, Loader *loader, GLuint textID0, GLuint textID1, GLuint textID2, GLuint textID3, GLuint textID4)
{
	//Create a new flat terrain
	TerrainObject = Terrain::generateTerrain(loader);

	//Set its textures
	TerrainObject.addTexture(textID0);
	TerrainObject.addTexture(textID1);
	TerrainObject.addTexture(textID2);
	TerrainObject.addTexture(textID3);
	TerrainObject.addTexture(textID4);

	//Set the position in the world
	TerrainObject.init(glm::vec3(gridX * SIZE, 0, gridZ * SIZE), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

//Create a new terrain with the heights from a heightmap
void Terrain::createWithHeightmap(const char * heightmapPath, int gridX, int gridZ, Loader *loader, GLuint textID0, GLuint textID1, GLuint textID2, GLuint textID3, GLuint textID4)
{
	//Create a new flat terrain
	TerrainObject = Terrain::generateTerrainWithHeightmap(loader, heightmapPath);

	//Set its textures
	TerrainObject.addTexture(textID0);
	TerrainObject.addTexture(textID1);
	TerrainObject.addTexture(textID2);
	TerrainObject.addTexture(textID3);
	TerrainObject.addTexture(textID4);

	//Set the position in the world
	TerrainObject.init(glm::vec3(gridX * SIZE, 0, gridZ * SIZE), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

//Get the gameobject
gameobject* Terrain::getModel()
{
	return &TerrainObject;
}

//Generate a new terrain
gameobject Terrain::generateTerrain(Loader *loader) {
	std::vector<glm::vec3> vertex, normal;
	std::vector<glm::vec2> uv;

	std::vector<face> faces;
	std::vector<GLuint> indices;

	for (unsigned int z = 0; z < VERTEX_COUNT; z++) {
		//For each vertex in the width
		for (unsigned int x = 0; x < VERTEX_COUNT; x++) {
			//For each vertex in the depth
			vertex.push_back(glm::vec3(
				((float)x / ((float)VERTEX_COUNT - 1.0f)) * SIZE, //x
				0, //y
				((float)z / ((float)VERTEX_COUNT - 1.0f)) * SIZE  //z
				));

			normal.push_back(glm::vec3(0, 1, 0));

			uv.push_back(glm::vec2(
				((float)x / ((float)VERTEX_COUNT - 1.0f)), //x
				((float)z / ((float)VERTEX_COUNT - 1.0f))  //y
			));
		}
	}

	for (unsigned int z = 0; z < VERTEX_COUNT; z++) {
		//For each vertex in the width
		for (unsigned int x = 0; x < VERTEX_COUNT; x++) {
			//For each vertex in the depth

			int tLeft = (z * VERTEX_COUNT) + x,
				tRight = tLeft + 1,
				bLeft = ((z + 1) * VERTEX_COUNT) + x,
				bRight = bLeft + 1;

			face f;

			f.vertices.x = tLeft;
			f.vertices.y = bLeft;
			f.vertices.z = bRight;
			faces.push_back(f);

			f.vertices.x = tLeft;
			f.vertices.y = bRight;
			f.vertices.z = tRight;
			faces.push_back(f);
		}
	}

	std::vector< glm::vec3 > normalsSortedVertex(faces.size() * 3);
	std::vector< glm::vec2 > uvsSortedVertex(faces.size() * 3);

	//Vertex inices to indices vector
	for (unsigned int i = 0; i < faces.size(); i++) {
		for (unsigned int j = 0; j < 3; j++) {
			unsigned int currentVertexPointer = (int)faces[i].vertices[j];
			indices.push_back(currentVertexPointer);
		}
	}

	//printf("Final object result: \n");
	//printf("Vertices %d, Uvs: %d, Normals: %d, Indices: %d, Faces %d\n", temp_vertices.size(), temp_uvs.size(), temp_normals.size(), indices.size(), faces.size());

	//Make vectors into float arrays
	GLuint *indicesArray = &indices[0];

	float *verticesArray = &vertex[0].x;
	float *normalsArray = &normal[0].x;
	float *textureArray = &uv[0].x;

	//File::createObjFile("res/Terrain.obj", verticesArray, (vertex.size() * 3), textureArray, (uv.size() * 2), normalsArray, (normal.size() * 3), indicesArray, indices.size());

	return loader->loadToVAO(verticesArray, (vertex.size() * 3),
							indicesArray, indices.size(),
							textureArray, (uv.size() * 2),
							normalsArray, (normal.size() * 3));
}

//Generate a new terrain using a heightmap
gameobject Terrain::generateTerrainWithHeightmap(Loader *loader, const char * heightmapPath) {
	//Object variables
	std::vector<glm::vec3> vertex, normal;
	std::vector<glm::vec2> uv;

	std::vector<face> faces;
	std::vector<GLuint> indices;

	//Read heightmap data:
	loadHeightmap(heightmapPath);

	//Create the terrain
	//For each vertex in the depth
	for (unsigned int z = 0; z < heightmapHeight; z++) {
		//For each vertex in the width
		for (unsigned int x = 0; x < heightmapHeight; x++) {

			float height = getHeightOfPixelData(glm::vec2((float)x, (float)z));
			heights.push_back(height);

			//Create a vertex
			vertex.push_back(glm::vec3(
				((float)x / ((float)heightmapHeight - 1.0f)) * SIZE, //x
				height - 100.0f, //y
				((float)z / ((float)heightmapHeight - 1.0f)) * SIZE  //z
			));

			//Create normal
			normal.push_back(calculateNormal(glm::vec2(x, z)));

			//Create texture coord
			uv.push_back(glm::vec2(
				((float)x / ((float)heightmapHeight - 1.0f)), //x
				((float)z / ((float)heightmapHeight - 1.0f))  //y
			));
		}//end x
	}//end z

	//Create faces
	for (unsigned int z = 0; z < heightmapHeight - 1; z++) {
		//For each vertex in the width
		for (unsigned int x = 0; x < heightmapHeight - 1; x++) {
			//For each vertex in the depth

			int tLeft = (z * heightmapHeight) + x,
				tRight = tLeft + 1,
				bLeft = ((z + 1) * heightmapHeight) + x,
				bRight = bLeft + 1;

			face f;

			f.vertices.x = (float)tLeft;
			f.vertices.y = (float)bLeft;
			f.vertices.z = (float)bRight;

			f.normals.x = (float)tLeft;
			f.normals.y = (float)bLeft;
			f.normals.z = (float)bRight;

			f.uvs.x = (float)tLeft;
			f.uvs.y = (float)bLeft;
			f.uvs.z = (float)bRight;
			faces.push_back(f);

			f.vertices.x = (float)tLeft;
			f.vertices.y = (float)bRight;
			f.vertices.z = (float)tRight;

			f.normals.x = (float)tLeft;
			f.normals.y = (float)bRight;
			f.normals.z = (float)tRight;

			f.uvs.x = (float)tLeft;
			f.uvs.y = (float)bRight;
			f.uvs.z = (float)tRight;
			faces.push_back(f);
		}
	}

	//Create vector buffers for sorting
	std::vector< glm::vec3 > normalsSortedVertex(faces.size() * 3);
	std::vector< glm::vec2 > uvsSortedVertex(faces.size() * 3);

	//Vertex inices to indices vector
	for (unsigned int i = 0; i < faces.size(); i++) {
		indices.push_back((int)faces[i].vertices.x);
		indices.push_back((int)faces[i].vertices.y);
		indices.push_back((int)faces[i].vertices.z);
	}

	//Make vectors into float arrays
	GLuint *indicesArray = &indices[0];

	float *verticesArray = &vertex[0].x;
	float *normalsArray = &normal[0].x;
	float *textureArray = &uv[0].x;

	/*File::createObjFile("res/terrainObject.obj",
			&vertex[0].x, (vertex.size() * 3),
			&uv[0].x, (uv.size() * 2),
			&normal[0].x, (normal.size() * 3),
			faces, faces.size());*/

	//Return a new gameobject with a filled VAO
	return loader->loadToVAO(verticesArray, (vertex.size() * 3),
		indicesArray, indices.size(),
		textureArray, (uv.size() * 2),
		normalsArray, (normal.size() * 3));
}

float Terrain::getHeight(float worldX, float worldY)
{
	gameobject *T = getModel();
	float terrainX = worldX - T->getPosition().x;
	float terrainZ = worldY - T->getPosition().z;
	float gridSquareSize = SIZE / ((float)heightmapHeight - 1);
	int gridX = (int)floor(terrainX / gridSquareSize);
	int gridZ = (int)floor(terrainZ / gridSquareSize);

	//If the grid is outside the model, return 0
	if (gridX >= (heightmapHeight - 1) || gridZ >= (heightmapHeight - 1) || gridX < 0 || gridZ < 0) return 0.0f;

	float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
	float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize;

	float answer;
	if (xCoord <= (1 - zCoord)) {
		answer = Maths::barryCentric(
			glm::vec3(0, heights[(gridZ * heightmapHeight) + gridX], 0), 
			glm::vec3(1, heights[(gridZ * heightmapHeight) + gridX + 1], 0), 
			glm::vec3(0, heights[((gridZ + 1) * heightmapHeight) + gridX], 1), 
			glm::vec2(xCoord, zCoord));
	}
	else {
		answer = Maths::barryCentric(
			glm::vec3(1, heights[(gridZ * heightmapHeight) + gridX + 1], 0),
			glm::vec3(1, heights[((gridZ + 1) * heightmapHeight) + gridX + 1], 1), 
			glm::vec3(0, heights[((gridZ + 1) * heightmapHeight) + gridX], 1), 
			glm::vec2(xCoord, zCoord));
	}
	return answer - 100.0f;
}

//Gets the height of the model from pixel data
float Terrain::getHeightOfPixelData(glm::vec2 pos)
{
	if (pos.x < 0 || pos.x >= heightmapHeight || pos.y < 0 || pos.y >= heightmapHeight) return 0;

	//int datapos = (heightmapWidth * pos.y) + pos.x; //Start top left
	int datapos = (int)(heightmapWidth * ((heightmapHeight - 1) - (int)pos.y)) + (int)pos.x; //Start bottom left

	glm::vec4 pixelColours = glm::vec4(pixels[datapos].r, pixels[datapos].g, pixels[datapos].b, pixels[datapos].a);
	float height = pixelColours.x * pixelColours.y * pixelColours.z;

	height += MAX_PIXEL_COLOUR / 2.0f;
	height /= MAX_PIXEL_COLOUR / 2.0f;
	height *= MAX_HEIGHT;

	return height;
}

//Calculate the normal of the vertex
glm::vec3 Terrain::calculateNormal(glm::vec2 pos)
{
	if (pos.x == 0 || pos.x >= heightmapHeight - 1 || pos.y == 0 || pos.y >= heightmapHeight - 1) return glm::vec3(0, 1, 0);

	float heightL = getHeightOfPixelData(glm::vec2(pos.x - 1, pos.y));
	float heightR = getHeightOfPixelData(glm::vec2(pos.x + 1, pos.y));
	float heightD = getHeightOfPixelData(glm::vec2(pos.x, pos.y - 1));
	float heightU = getHeightOfPixelData(glm::vec2(pos.x, pos.y + 1));
	
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	normal = glm::normalize(normal);

	return normal;
}

void Terrain::loadHeightmap(const char * heightmapPath)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width * height * 4

	unsigned char * data; // Actual RGBA data buffer

						  // Open the heightmap file
	FILE * file;
	fopen_s(&file, heightmapPath, "rb");
	if (!file) {
		printf("Heightmap could not be opened\n");
		system("pause");
		exit(0);
	}

	// If not 54 bytes read : problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		system("pause");
		exit(0);
	}

	//If header does not start with BM its not a correct bmp file
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		system("pause");
		exit(0);
	}

	// Read file information from the header
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	heightmapWidth = *(int*)&(header[0x12]);
	heightmapHeight = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = heightmapWidth * heightmapHeight * 4; // 4 : one byte for each Red, Green, Blue and Alpha component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize]; // Create a buffer with correct size

										 //Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	//Start is still bottom left pixel (Can export bmp inverted in photoshop)

	//Get data from buffer and put it in the pixel variable for storage. Pixel has RGBA data
	int dataposition = 0;
	for (unsigned int p = 0; p < ((heightmapWidth * heightmapHeight) / 2); p++) {
		pixel pi;
		pi.b = data[dataposition += 2];
		pi.g = data[dataposition += 2];
		pi.r = data[dataposition += 2];
		pi.a = data[dataposition += 2];

		pixels.push_back(pi);
	}

	heightmapHeight /= 2;
}