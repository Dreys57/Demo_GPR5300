#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include "stb_image.h"
#include <assimp/material.h>

namespace gl {

	unsigned int LoadCubeMap(std::vector<std::string> faces)
	{
		unsigned int textureID;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nbChannels;

		for(unsigned int i = 0; i < faces.size(); ++i)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nbChannels, 0);

			if(data)
			{
				switch (nbChannels)
				{
				case 1:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
					break;

				case 3:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					break;
					
				case 4:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					break;
				}

				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex load fail at " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
	
	unsigned int LoadTextureFromFile(const char* path, const std::string& directory, aiTextureType textureType)
	{
		std::string filename = directory + "/" + std::string(path);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nbChannels;

		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nbChannels, 0);

		assert(data);
		
		GLenum format;
		GLenum format2;

		switch (nbChannels)
		{
		case 1:
			format = GL_RED;
			format2 = GL_RED;
			break;

		case 2:
			format = GL_RG;
			format2 = GL_RG;
			break;

		case 3:

			if (textureType == aiTextureType_DIFFUSE)
			{
				format = GL_SRGB;
			}
			else
			{
				format = GL_RGB;
			}

			format2 = GL_RGB;
			break;

		case 4:

			if (textureType == aiTextureType_DIFFUSE)
			{
				format = GL_SRGB8_ALPHA8;
			}
			else
			{
				format = GL_RGBA;
			}

			format2 = GL_RGBA;
		

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format2, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		

		return textureID;
	}
	
	class Texture {
	public:
		unsigned int id;

		Texture() = default;
		
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

	struct TextureStruct
	{
		unsigned int id = 0;
		std::string type;
		std::string path;
	};

} // End namespace gl.
