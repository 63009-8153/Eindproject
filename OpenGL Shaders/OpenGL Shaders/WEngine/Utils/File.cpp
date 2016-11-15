#include "../header.h"

#include "File.h"

void File::writeFile(const char *filename, const char *messageAlreadyFormatted)
{
	FILE * file;
	fopen_s(&file, filename, "wt");

	fprintf(file, messageAlreadyFormatted);

	fclose(file);
}

void File::createObjFile(const char *filename,
	GLfloat vertices[], int vertexCount,
	GLfloat uvs[], int uvsSize,
	GLfloat normals[], int normalsSize,
	std::vector<face>& faces, int facesCount)
{

	unsigned int i;
	FILE *file;
	fopen_s(&file, filename, "w");

	//Get time to create a time stamp.
	time_t result = time(NULL);
	char str[26];

	ctime_s(str, sizeof str, &result);

	//File head
	fprintf_s(file, "# WEngine Wavefront OBJ Exporter\n");
	fprintf_s(file, "# File Created : %s\n", str);

	//Information
	fprintf_s(file, "# Information about the object:\n");
	fprintf_s(file, "# Vertices: %d\n", (vertexCount / 3));
	fprintf_s(file, "# Textures: %d\n", (uvsSize / 2));
	fprintf_s(file, "# Normals:  %d\n", (normalsSize / 3));
	fprintf_s(file, "# Faces:    %d\n\n", facesCount);

	//Vertices
	fprintf_s(file, "# Vertices\n");
	for (i = 0; i < (vertexCount / 3); i++) {
		fprintf(file, "v %.4f %.4f %.4f\n", vertices[i * 3], vertices[(i * 3) + 1], vertices[(i * 3) + 2]);
	}
	fprintf_s(file, "# %d Vertices\n\n", (vertexCount / 3));

	//Textures
	fprintf_s(file, "# Texture coordinates\n");
	for (i = 0; i < (uvsSize / 2); i++) {
		fprintf(file, "vt %.4f %.4f 0.0000\n", uvs[i * 2], uvs[(i * 2) + 1]);
	}
	fprintf_s(file, "# %d texture coords\n\n", (uvsSize / 2));

	//Normals
	fprintf_s(file, "# Vertex normals\n");
	for (i = 0; i < (normalsSize / 3); i++) {
		fprintf(file, "vn %.4f %.4f %.4f\n", normals[i * 3], normals[(i * 3) + 1], normals[(i * 3) + 2]);
	}
	fprintf_s(file, "# %d normals\n\n", (normalsSize / 3));

	//Faces
	fprintf_s(file, "# Faces\n");
	for (i = 0; i < facesCount; i++) {
		face f = faces.at(i);
		fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			(int)f.vertices.x + 1, (int)f.uvs.x + 1, (int)f.normals.x + 1,
			(int)f.vertices.y + 1, (int)f.uvs.y + 1, (int)f.normals.y + 1,
			(int)f.vertices.z + 1, (int)f.uvs.z + 1, (int)f.normals.z + 1);
	}
	fprintf_s(file, "# %d faces", facesCount);

	//Close file
	fclose(file);
}