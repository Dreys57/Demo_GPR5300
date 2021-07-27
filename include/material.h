#pragma once

#include <glm/glm.hpp>
#include "texture.h"

namespace gl {

	class Material {
		
	public:
		
		Texture color;
		Texture specular;
		Texture normalMap;
		glm::vec3 specular_vec;
		int specular_pow;
	};

}//End namepsace gl