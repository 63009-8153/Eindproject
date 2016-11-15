#ifndef _FILE_TOOLS_
#define _FILE_TOOLS_

class File {
public:
	static void writeFile(const char *filename, const char *messageAlreadyFormatted);
	static void createObjFile(const char *filename, GLfloat vertices[], int vertexCount, GLfloat uvs[], int uvsSize, GLfloat normals[], int normalsSize, std::vector<face>& faces, int facesCount);
};

#endif // !_FILE_TOOLS_
