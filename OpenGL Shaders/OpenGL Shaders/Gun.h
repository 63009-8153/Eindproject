#ifndef __GUN__
#define __GUN__

#include "Header.h"

#include "Model.h"
#include "Player.h"

extern Model GUN_WALTER;

class Gun : public Model
{
public:
	Gun();
	~Gun();

	void Init(glm::vec3 position, glm::vec3 rotation);
	void Update(glm::vec3 position, glm::vec3 rotation);

private:

};
#endif // !__GUN__