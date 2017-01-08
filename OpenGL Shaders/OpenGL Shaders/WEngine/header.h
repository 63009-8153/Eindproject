//Include default stuff
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>

#include <map>

#include <fstream>
#include <sstream>


//Include OpenGL stuff
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>


//Include GLM Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>


//Include other stuff
#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include <float.h>
#include <process.h>

//Own includes
#include "Utils/Maths.h"

//Define PI
#define _PI       3.14159265358979323846   // pi
#define _PI_2     1.57079632679489661923   // pi/2
#define _PI_4     0.785398163397448309616  // pi/4


//Defines for program
#define PROGRAM_NAME "OpenGL Shaders test"
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 761

#define FPSLIMIT 64

//Typedefs
#ifndef TYPE_FACE
#define TYPE_FACE

typedef struct {
	glm::vec3 vertices,
		normals,
		uvs;
} face;

#endif // !TYPE_FACE

#ifndef TYPE_PIXEL
#define TYPE_PIXEL

typedef struct {
	float r, g, b, a;
} pixel;

#endif // !TYPE_PIXEL
