#include "../header.h"

#include "../Display/DisplayManager.h"

#include "../lodePNG/lodepng.h"
#include <locale>

#include "loader.h"
//Public
//Destuctor, cleanup
Loader::~Loader()
{
	//On destuction, cleanup
	cleanUp();
}
void Loader::cleanUp()
{
	//Cleanup all vaos
	for (unsigned int i = 0; i < vaos.size(); i++) {
		glDeleteVertexArrays(1, &vaos[i]);
	}

	//Cleanup all vbos
	for (unsigned int i = 0; i < vbos.size(); i++) {
		glDeleteBuffers(1, &vbos[i]);
	}

	//Cleanup all textures
	for (unsigned int i = 0; i < textures.size(); i++) {
		glDeleteTextures(1, &textures[i]);
	}
}

//Load position, indices and uvs in a VAO and return its gameobject
gameobject Loader::loadToVAO(GLfloat positions[], int posCount, 
							 GLuint indices[], int indicesCount, 
							 GLfloat uvs[], int uvsSize,
							 GLfloat normals[], int normalsSize)
{
	int vaoID = createVAO();

	bindIndicesBuffer(indices, indicesCount);

	storeDataInAttributeList(0, 3, positions, posCount); //Store positions in VAO position 0
	storeDataInAttributeList(1, 2, uvs, uvsSize); //Store uv coords in VAO position 1
	storeDataInAttributeList(2, 3, normals, normalsSize); //Store uv coords in VAO position 1

	unbindVAO();

	return gameobject(vaoID, indicesCount);
}

gameobject Loader::loadToVAO(GLfloat positions[], int posCount,
							 GLuint indices[], int indicesCount, 
							 GLfloat uvs[], int uvsSize, 
							 GLfloat normals[], int normalsSize, 
							 GLfloat tangents[], int tangentsSize)
{
	int vaoID = createVAO();

	bindIndicesBuffer(indices, indicesCount);

	storeDataInAttributeList(0, 3, positions, posCount); //Store positions in VAO position 0
	storeDataInAttributeList(1, 2, uvs, uvsSize); //Store uv coords in VAO position 1
	storeDataInAttributeList(2, 3, normals, normalsSize); //Store uv coords in VAO position 2
	storeDataInAttributeList(3, 3, tangents, tangentsSize); //Store tangents in VAO position 3

	unbindVAO();

	return gameobject(vaoID, indicesCount);
}

gameobject Loader::loadToVAO(GLfloat positions[], int posCount, int dimensions)
{
	int vaoID = createVAO();

	storeDataInAttributeList(0, dimensions, positions, posCount); //Store positions in VAO position 0

	unbindVAO();

	return gameobject(vaoID, posCount / dimensions);
}

gameobject Loader::loadObjFile(const char * path, bool writeParsed, bool forceParse) {

	//Clear the faces vector
	faces.clear();

	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	bool parsedFile = false;

	//Open gile
	FILE * file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Impossible to open file: %s!\n", path);
		system("pause");
		exit(0);
	}

	//Infinate loop to itterate through lines
	while (true) {
		char lineHeader[128]; //Lineheader buffer

		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) { //vertices
			glm::vec3 vertex;

			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) { //uvs
			glm::vec2 uv;

			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) { //normals
			glm::vec3 normal;

			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) { //faces
			unsigned int vertexIndex[3],
						 uvIndex[3],
						 normalIndex[3];

			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File %s can't be read by our simple parser : ( Try exporting with other options\n", path);
				return gameobject();
			}

			face currentFace;

			currentFace.vertices.x = (float)((int)vertexIndex[0] - 1);
			currentFace.vertices.y = (float)((int)vertexIndex[1] - 1);
			currentFace.vertices.z = (float)((int)vertexIndex[2] - 1);

			currentFace.normals.x = (float)((int)normalIndex[0] - 1);
			currentFace.normals.y = (float)((int)normalIndex[1] - 1);
			currentFace.normals.z = (float)((int)normalIndex[2] - 1);

			currentFace.uvs.x = (float)((int)uvIndex[0] - 1);
			currentFace.uvs.y = (float)((int)uvIndex[1] - 1);
			currentFace.uvs.z = (float)((int)uvIndex[2] - 1);

			faces.push_back(currentFace);
		}
	}

	//Close file
	fclose(file);

	printf("Model loaded!\n");

	//If there are more texture coords than vertices, we need to parse the obj file further so the texture aligns properly.
	if (temp_uvs.size() > temp_vertices.size() || forceParse) {
		printf("Starting parsing file: %s!\n", path);

		for (unsigned int i = (faces.size() - 1); i > 0; i--) {
			for (unsigned int j = 0; j < i; j++) {
				if (i == j) continue;

				for (unsigned int l = 0; l < 3; l++) {
					for (unsigned int k = 0; k < 3; k++) {
						if (faces[i].vertices[l] == faces[j].vertices[k] && faces[i].uvs[l] != faces[j].uvs[k]) {
							//Get position of vertex in question
							int vertexComparedVal = (int)faces[i].vertices[l];
							glm::vec3 otherVertexValue = temp_vertices[vertexComparedVal];

							//Create new vertex and get its id
							int newVertexPosition = temp_vertices.size();
							temp_vertices.push_back(otherVertexValue);

							//Put the new vertex value in the right place in its face
							switch (l) {
								case 0:
									faces[i].vertices.x = (float)newVertexPosition;
									break;
								case 1:
									faces[i].vertices.y = (float)newVertexPosition;
									break;
								case 2:
									faces[i].vertices.z = (float)newVertexPosition;
									break;
							}

							//TODO: proper normals and texture coords 
						}
					}
				}
			}
		}

		printf("Finished parsing file!\n");

		parsedFile = true;
		//TODO: Make this parser as a standalone program to parse obj files before use.
	}

	if (parsedFile && writeParsed) {
		printf("Starting writing parsed file!\n");

		File::createObjFile(path,
			&temp_vertices[0].x, (temp_vertices.size() * 3),
			&temp_uvs[0].x, (temp_uvs.size() * 2),
			&temp_normals[0].x, (temp_normals.size() * 3),
			faces, faces.size());

		printf("Finished writing parsed file!\n");
	}

	std::vector < GLuint > indices;
	std::vector< glm::vec3 > normalsSortedVertex(faces.size() * 3);
	std::vector< glm::vec2 > uvsSortedVertex(faces.size() * 3);
	
	std::vector<glm::vec3> tangents(temp_vertices.size());
	std::vector<bool> tangentSet(temp_vertices.size());
	for (unsigned int i = 0; i < temp_vertices.size(); i++) tangentSet[i] = false;

	//Vertex inices to indices vector
	for (unsigned int i = 0; i < faces.size(); i++) {
		face f = faces[i];
		std::vector<glm::vec3> returnTangents;

		computeTangentBasis(
			temp_vertices[(int)f.vertices.x],
			temp_vertices[(int)f.vertices.y],
			temp_vertices[(int)f.vertices.z],
			temp_uvs[(int)f.uvs.x],
			temp_uvs[(int)f.uvs.y],
			temp_uvs[(int)f.uvs.z],
			returnTangents);

		//normalize( max( cross(normalize(surfaceNormal), vec3(0.0, 0.0, 1.0)) , cross(normalize(surfaceNormal), vec3(0.0, 1.0, 0.0)) ) );
		returnTangents.clear();
		returnTangents.push_back(glm::normalize(glm::max(glm::cross(glm::normalize(temp_normals[(int)f.normals.x]), glm::vec3(0.0f, 0.0f, 1.0f)), glm::cross(glm::normalize(temp_normals[(int)f.normals.x]), glm::vec3(0.0f, 1.0f, 0.0f)))));
		returnTangents.push_back(glm::normalize(glm::max(glm::cross(glm::normalize(temp_normals[(int)f.normals.y]), glm::vec3(0.0f, 0.0f, 1.0f)), glm::cross(glm::normalize(temp_normals[(int)f.normals.y]), glm::vec3(0.0f, 1.0f, 0.0f)))));
		returnTangents.push_back(glm::normalize(glm::max(glm::cross(glm::normalize(temp_normals[(int)f.normals.z]), glm::vec3(0.0f, 0.0f, 1.0f)), glm::cross(glm::normalize(temp_normals[(int)f.normals.z]), glm::vec3(0.0f, 1.0f, 0.0f)))));

		processVertexTangent((int)f.vertices.x, returnTangents[0], tangents, tangentSet);
		processVertexTangent((int)f.vertices.y, returnTangents[1], tangents, tangentSet);
		processVertexTangent((int)f.vertices.z, returnTangents[2], tangents, tangentSet);

		for (unsigned int j = 0; j < 3; j++) {

			unsigned int currentVertexPointer = (int)f.vertices[j];
			unsigned int uvPointer			  = (int)f.uvs[j];
			unsigned int normalPointer		  = (int)f.normals[j];

			indices.push_back(currentVertexPointer);

			//Make sure there are as many uvs and normals as there are vertices.
			uvsSortedVertex.at(currentVertexPointer)     = temp_uvs[uvPointer];
			normalsSortedVertex.at(currentVertexPointer) = temp_normals[normalPointer];
		}
	}

	/*fopen_s(&file, "res/tangents.txt", "w");
	for (unsigned int i = 0; i < tangents.size(); i++)
	{
		if(i > 25 && i < 35) printf("%d : %f \t%f \t%f\n", i, tangents[i].x, tangents[i].y, tangents[i].z);
		fprintf_s(file, "%d \t%f \t%f \t%f\n", i, tangents[i].x, tangents[i].y, tangents[i].z);
	}
	fclose(file);*/

	//Make vectors into float arrays
	float *textureArray = &uvsSortedVertex[0].x;
	float *normalsArray = &normalsSortedVertex[0].x;

	float *verticesArray = &temp_vertices[0].x;
	GLuint *indicesArray = &indices[0];

	float *tangentsArray = &tangents[0].x;

	//Return a gameobject with loaded data.
	return loadToVAO(verticesArray, (temp_vertices.size() * 3), 
					 indicesArray,  (indices.size()), 
					 textureArray,  (uvsSortedVertex.size() * 2),
					 normalsArray,  (normalsSortedVertex.size() * 3),
					 tangentsArray, (tangents.size() * 3));
}

//Load a BMP texture and return its texture id
GLuint Loader::loadTexture(const char * imagepath, bool createMipmap) 
{
	GLuint textureID;

	//TODO: more permanent and failproof solution
	//if (imagepath[strlen(imagepath) - 1] == 'p') { //bmp 23 bit
	//	textureID = loadBMP(imagepath);
	//}else 
	if (imagepath[strlen(imagepath) - 1] == 'p') { //bmp 32 bit
		textureID = loadBMPRGBA(imagepath);
	}else if (imagepath[strlen(imagepath) - 1] == 'g') { //png
		textureID = loadPNG(imagepath);
	}
	else return -1;

	if (createMipmap) {
		//Create mipmapping for the texture
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		if (DisplayManager::supports_AnisotropicFiltering) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0); //Set mimapping to 0 to use anisotropic filtering

			float aniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			float amount = std::max(4.0f, aniso);
			printf("Using %fx anisotropic filtering\n", amount);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); 
		}
		else {
			printf("Anisotropic filtering not supported\n");

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f); //More negative makes mipmap less
		}
	}
	
	//Save the texture id for cleanup
	textures.push_back(textureID);

	return textureID;
}
GLuint Loader::loadTextureInData(const char * imagepath, bool bmpAlign, bool createMipmap, unsigned char *&_data, int & width, int & height, int & size, GLenum & type)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width * height * 4
	unsigned int imagewidth, imageheight;

	unsigned char * data; // Actual RGBA data buffer

	// Open the image file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("image could not be opened\n");
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
	imagewidth = *(int*)&(header[0x12]);
	imageheight = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = imagewidth * imageheight * 4; // 4 : one byte for each Red, Green, Blue and Alpha component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize]; // Create a buffer with correct size
	_data = new unsigned char[imageSize]; // Create a buffer with correct size

	//Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	size = imageSize;
	width = imagewidth;
	height = imageheight;
	type = GL_BGRA;

	//Start is still bottom left pixel (Can export bmp inverted in photoshop)

	_data = data;

	if (bmpAlign) {
		unsigned char * formattedData = new unsigned char[imageSize];
		unsigned int formattedDataPosition = 0;
		for (int row = (height - 1); row >= 0; row--) {
			for (int colum = 0; colum < (width * 4); colum++) {
				formattedData[formattedDataPosition++] = _data[(row * (width * 4)) + colum];
			}
		}
		_data = formattedData;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagewidth, imageheight, 0, GL_BGRA, GL_UNSIGNED_BYTE, _data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (createMipmap) {
		//Create mipmapping for the texture
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		if (DisplayManager::supports_AnisotropicFiltering) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0); //Set mimapping to 0 to use anisotropic filtering

			float aniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			float amount = std::max(4.0f, aniso);
			printf("Using %fx anisotropic filtering\n", amount);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		}
		else {
			printf("Anisotropic filtering not supported\n");

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f); //More negative makes mipmap less
		}
	}

	return textureID;
}
void Loader::loadTextureInData(const char * imagepath, bool bmpAlign, unsigned char *&_data, int & width, int & height, int & size, GLenum & type)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width * height * 4
	unsigned int imagewidth, imageheight;


	// Open the image file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("image could not be opened\n");
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
	imagewidth = *(int*)&(header[0x12]);
	imageheight = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = imagewidth * imageheight * 4; // 4 : one byte for each Red, Green, Blue and Alpha component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	_data = new unsigned char[imageSize]; // Create a buffer with correct size

	//Read the actual data from the file into the buffer
	fread(_data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	size = imageSize;
	width = imagewidth;
	height = imageheight;
	type = GL_BGRA;

	if (bmpAlign) {
		unsigned char * formattedData = new unsigned char[imageSize];
		unsigned int formattedDataPosition = 0;
		for (int row = (height - 1); row >= 0; row--) {
			for (int colum = 0; colum < (width * 4); colum++) {
				formattedData[formattedDataPosition++] = _data[(row * (width * 4)) + colum];
			}
		}
		_data = formattedData;
	}
}

GLuint Loader::loadCubeMap(unsigned char * data[], glm::vec2 size[], GLenum type)
{
	GLuint textID;
	glGenTextures(1, &textID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textID);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, (GLsizei)size[i].x, (GLsizei)size[i].y, 0, type, GL_UNSIGNED_BYTE, data[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	textures.push_back(textID);

	return textID;
}

//Create empty VBO with a size of floatCount(max floats the buffer will ever have) * sizeof(GLfloat) to get the amount of bytes
GLuint Loader::createEmptyVbo(int floatCount)
{
	GLuint vbo;
	//Generate VBO
	glGenBuffers(1, &vbo);

	//Save id for cleanup
	vbos.push_back(vbo);
	
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	//Allocate VBO Size
	glBufferData(GL_ARRAY_BUFFER, (5 * 21) * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	
	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return vbo;
}
//Add a attribute to the VAO with the data of the large VBO to be able to use it in the shader code.
//vao: VAOid of the model.
//vbo: VBOid of the large vbo.
//attribute: the attribute number to use.
//dataSize: Size of a single spot in the attibute.
//instanceDataLength: 
//offset: offset in floats of the data
void Loader::addInstancedAttribute(GLuint vao, GLuint vbo, GLuint attribute, GLuint dataSize, GLuint instancedDataLength, GLuint offset)
{
	//Bind VAO
	glBindVertexArray(vao);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Set data size and pointer where the data is, stride and offset
	glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE, (instancedDataLength * sizeof(GLfloat)), (GLvoid*)(offset * sizeof(GLfloat)));

	glEnableVertexAttribArray(attribute);

	//Attribute is a instance
	glVertexAttribDivisor(attribute, 1);

	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Unbind VAO
	glBindVertexArray(0);
}
void Loader::updateVbo(GLuint vbo, GLfloat *data, int dataSize)
{
	int pointer = 0;
	GLfloat *vboData = new GLfloat[2 * 21];

	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 10.0f;
	vboData[pointer++] = 0.001f;
	vboData[pointer++] = 0.2f;

	vboData[pointer++] = 0.0f;
	vboData[pointer++] = 0.0f;

	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 10.0f;
	vboData[pointer++] = 0.001f;
	vboData[pointer++] = 0.2f;

	vboData[pointer++] = 0.0f;
	vboData[pointer++] = 0.0f;
	
	//printf("Error1: %d\n", error);
	//Bind VBO for use
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBufferData(GL_ARRAY_BUFFER, (5 * 21) * sizeof(GLfloat), vboData, GL_DYNAMIC_DRAW);
	//Set VBO Data
	//glBufferData(GL_ARRAY_BUFFER, (42 * sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);

	//Update buffer data
	//glBufferSubData(Target, offset(bytes), size to be copied(bytes), data)
	//glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * 21) * sizeof(GLfloat)), vboData);

	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


GLuint Loader::loadBMP(const char * imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width * height * 3

	unsigned char * data; // Actual RGB data

	// Open the file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("Image could not be opened\n");
		system("pause");
		exit(0);
	}

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize]; // Create a buffer

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

GLuint Loader::loadBMPRGBA(const char * imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width * height * 4

	unsigned char * data; // Actual RGBA data

	// Open the file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("Image %s could not be opened!\n", imagepath);
		system("pause");
		exit(0);
	}

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = width * height * 4; // 4 : one byte for each Red, Green, Blue and Alpha component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize]; // Create a buffer

										 // Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

GLuint Loader::loadPNG(const char * imagepath) {
	// Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;

	unsigned error = lodepng::decode(image, width, height, imagepath);

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		system("pause");
		exit(0);
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// glGenerateMipmap(GL_TEXTURE_2D); //Performance increse glew32.dll

	return textureID;
}

//Private
//Create a new empty VAO
GLuint Loader::createVAO() 
{
	GLuint _vaoID;

	//Create vao and bind it
	glGenVertexArrays(1, &_vaoID);
	glBindVertexArray(_vaoID);

	//Store vao id for cleanup
	vaos.push_back(_vaoID);

	//Return vaoid
	return _vaoID;
}

//Store data in a VAO's attribute list at attribueNumber
void Loader::storeDataInAttributeList(int attributeNumber, int coordSize, GLfloat data[], int size) 
{
	GLuint _vboID;

	// Create VBO and bind it
	glGenBuffers(1, &_vboID);

	glBindBuffer(GL_ARRAY_BUFFER, _vboID);

	//Store vboid for cleanup
	vbos.push_back(_vboID);

	// Put data into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, data, GL_STATIC_DRAW);

	// Put the data in the VAO
	//Index, size, type, normalized, distance between, offset
	glVertexAttribPointer(attributeNumber, coordSize, GL_FLOAT, GL_FALSE, 0, 0);

	//Unbind the vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//Bind the indices buffer
void Loader::bindIndicesBuffer(GLuint indices[], int amount)
{
	GLuint _iboID;

	// Create IBO (index buffer object) and bind it
	glGenBuffers(1, &_iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);

	//Store IBO id for cleanup like a vbo
	vbos.push_back(_iboID);

	// Put data into the IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * amount, indices, GL_STATIC_DRAW);

}

//Unbind the VAO
void Loader::unbindVAO() 
{
	//Unbind currently bound vao
	glBindVertexArray(0);
}

void Loader::computeTangentBasis(
		// inputs
		glm::vec3 pos1,
		glm::vec3 pos2,
		glm::vec3 pos3,

		glm::vec2 uv1,
		glm::vec2 uv2,
		glm::vec2 uv3,
		// outputs
		std::vector<glm::vec3> & tangents
	) 
{
	/*glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	glm::vec3 tangent;

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

	glm::vec3 & n = normals[i];
	glm::vec3 & t = tangents[i];
	glm::vec3 & b = bitangents[i];

	// Gram-Schmidt orthogonalize
	t = glm::normalize(t - n * glm::dot(n, t));

	// Calculate handedness
	if (glm::dot(glm::cross(n, t), b) < 0.0f) {
		t = t * -1.0f;
	}


	tangents.push_back(tangent);*/
}

void Loader::processVertexTangent(int index, glm::vec3 tangent, std::vector<glm::vec3> & tangents, std::vector<bool> & tangentsSet)
{
	if (_isnan(tangent.x))
	{
		tangent = glm::vec3(0.000001f, 0.0f, 0.0f);
	}

	if (tangentsSet[index]) {
		glm::vec3 existingTangent = tangents[index];
		tangents[index] = (existingTangent + tangent) / 2;
	}
	else tangents[index] = tangent;
	tangentsSet[index] = true;
}