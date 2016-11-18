#ifndef _WE_MATERIAL_
#define _WE_MATERIAL_

class Material
{
	public:
		Material();
		~Material();

		void setMaterial(std::string type);

		void setShineAndReflectivity(float shiner, float _reflectivity);
		void setAmbientLight(float ambient);

		void setFakeLighting(bool fakeLighting);

		void setReflectionAmount(float reflection);
		void setRefractionAmount(float refraction);

	private:

		float shineDamper,
			reflectivity;

		float ambientLight;

		bool useFakeLighting;
		
		float reflectionAmount,
			refractionAmount;
		
};

#endif