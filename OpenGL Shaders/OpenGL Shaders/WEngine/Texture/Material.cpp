#include "../header.h"

#include "Material.h"

Material::Material()
{
	shineDamper = 0.0f;
	reflectivity = 0.0f;

	ambientLight = 1.0f;

	useFakeLighting = false;

	reflectionAmount = 0.0f;
	refractionAmount = 0.0f;
}
Material::~Material()
{
}

void Material::setMaterial(std::string type)
{
	if (type.c_str() == "") {

	}
	else if (type.c_str() == "") {

	}
}

void Material::setShineAndReflectivity(float shiner, float _reflectivity)
{
	shineDamper = shiner;
	reflectivity = _reflectivity;
}
void Material::setAmbientLight(float ambient)
{
	ambientLight = ambient;
}

void Material::setFakeLighting(bool fakeLighting)
{
	useFakeLighting = fakeLighting;
}

void Material::setReflectionAmount(float reflection)
{
	reflectionAmount = reflection;
}
void Material::setRefractionAmount(float refraction)
{
	refractionAmount = refraction;
}