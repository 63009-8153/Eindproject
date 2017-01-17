#ifndef __GUN__
#define __GUN__

#include "Header.h"

#include "Model.h"

extern Model GUN_WALTER;

class Gun
{
public:
	Gun();
	~Gun();

	Model gun_model;

	void Init(glm::vec3 position, glm::vec3 rotation);
	void Update(glm::vec3 position, glm::vec3 rotation);

private:

};
#endif // !__GUN__