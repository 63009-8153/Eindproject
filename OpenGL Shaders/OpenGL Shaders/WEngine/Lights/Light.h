#ifndef _LIGHT_
#define _LIGHT_

class Light
{
public:
	Light();
	Light(glm::vec3 pos, glm::vec3 col);
	Light(glm::vec3 pos, glm::vec3 col, glm::vec3 _attenuation);
	~Light();

	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition();

	void setColour(glm::vec3 col);
	glm::vec3 getColour();

	glm::vec3 getAttenuation();

	void setLightCanCastShadow(bool c);
	bool getLightCanCastShadow();
private:
	glm::vec3 position,
			  colour,
			  attenuation;

	bool castsShadow;
};

#endif // !_LIGHT_

//TODO: create a lighting system where you can add a light, enable or disable a light, change its properties and it checks if a light has changed. 
//		Then only give this object and everything that needs the lights can just get the information from this object.