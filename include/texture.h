#pragma once

#include <string>
#include <fstream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gl {

	class Texture {
	public:
		unsigned int id;

		Texture(){}
		
		Texture(const std::string& file_name)
		{
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(
				file_name.c_str(),
				&width,
				&height,
				&nrChannels,
				0);
			assert(data);
			glGenTextures(1, &id);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, id);
			IsError(__FILE__, __LINE__);
			switch (nrChannels)
			{
			case 3:
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGB,
					width,
					height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					data);
				IsError(__FILE__, __LINE__);
				break;
			case 4:
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGBA,
					width,
					height,
					0,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					data);
				IsError(__FILE__, __LINE__);
				break;
			case 1:
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					width,
					height,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					data);
				IsError(__FILE__, __LINE__);
				break;
			default:
				throw std::runtime_error(
					"Unknown number of colors: " + std::to_string(nrChannels));
			}
			stbi_image_free(data);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			IsError(__FILE__, __LINE__);
			glGenerateMipmap(GL_TEXTURE_2D);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, 0);
			IsError(__FILE__, __LINE__);
		}
		void Bind(unsigned int i = 0) const
		{
			glActiveTexture(GL_TEXTURE0 + i);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, id);
			IsError(__FILE__, __LINE__);
		}
		void UnBind() const
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	protected:
		void IsError(const char* file, int line) const {
			auto error_code = glGetError();
			if (error_code != GL_NO_ERROR)
			{
				throw std::runtime_error(
					std::to_string(error_code) +
					" in file: " + file +
					" at line: " + std::to_string(line));
			}
		}
	};

} // End namespace gl.
