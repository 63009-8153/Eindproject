#ifndef _MATHS_
#define _MATHS_

#include "../header.h"

class Maths {
public:
	//Print a 4 x 4 matrix to the console
	static void printMatrix(glm::mat4 matrix) {
		printf("============== Matrix Start =================\n");
		printf("%.3f,\t %.3f,\t %.3f,\t %.3f\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
		printf("%.3f,\t %.3f,\t %.3f,\t %.3f\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
		printf("%.3f,\t %.3f,\t %.3f,\t %.3f\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
		printf("%.3f,\t %.3f,\t %.3f,\t %.3f\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
		printf("============== Matrix End ===================\n\n");
	}

	static float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y;
	}

	//Create a transformation matrix
	static glm::mat4 createTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {
		//Identity matrix
		glm::mat4 matrix = glm::mat4();

		//Translate matrix
		matrix = glm::translate(matrix, translation);

		//Rotate matrix
		matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

		//Scale matrix
		matrix = glm::scale(matrix, scale);

		//printMatrix(matrix);

		return matrix;
	}

	//Create a view matrix
	static glm::mat4 createViewMatrix(glm::vec3 translation, glm::vec3 rotation) {
		//Identity matrix
		glm::mat4 matrix = glm::mat4();

		//Rotate matrix
		matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

		//Translate matrix
		matrix = glm::translate(matrix, -translation);

		//printMatrix(matrix);

		return matrix;
	}
};
#endif // !_MATH_
