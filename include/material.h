#pragma once

#include <glm/glm.hpp>
#include "texture.h"

namespace gl {

	class Material {
		
	public:
		
		Texture color;
		Texture specular;
		glm::vec3 specular_vec;
		float specular_pow;
	};

}//End namepsace gl