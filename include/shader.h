#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace gl {

	class Shader
	{
	public:
		unsigned int id;
		// constructor generates the shader on the fly
		Shader(
			const std::string& vertexPath,
			const std::string& fragmentPath,
			const std::string& geometryPath = "")
		{
			// 1. retrieve the vertex/fragment source code from filePath
			std::string vertexCode;
			std::string fragmentCode;
			std::string geometryCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			std::ifstream gShaderFile;
			// ensure ifstream objects can throw exceptions:
			vShaderFile.exceptions(
				std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(
				std::ifstream::failbit | std::ifstream::badbit);
			gShaderFile.exceptions(
				std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				// open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				// read file's buffer contents into streams
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				// close file handlers
				vShaderFile.close();
				fShaderFile.close();
				// convert stream into string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
				// if geometry shader path is present, also load a geometry
				// shader
				if (!geometryPath.empty())
				{
					gShaderFile.open(geometryPath);
					std::stringstream gShaderStream;
					gShaderStream << gShaderFile.rdbuf();
					gShaderFile.close();
					geometryCode = gShaderStream.str();
				}
			}
			catch (std::ifstream::failure& e)
			{
				throw std::runtime_error(e.what());
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();
			// 2. compile shaders
			unsigned int vertex, fragment;
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			IsError(__FILE__, __LINE__);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			IsError(__FILE__, __LINE__);
			glCompileShader(vertex);
			IsError(__FILE__, __LINE__);
			CheckCompileErrors(vertex, "VERTEX");
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			IsError(__FILE__, __LINE__);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			IsError(__FILE__, __LINE__);
			glCompileShader(fragment);
			IsError(__FILE__, __LINE__);
			CheckCompileErrors(fragment, "FRAGMENT");
			// if geometry shader is given, compile geometry shader
			unsigned int geometry;
			if (!geometryPath.empty())
			{
				const char* gShaderCode = geometryCode.c_str();
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				IsError(__FILE__, __LINE__);
				glShaderSource(geometry, 1, &gShaderCode, NULL);
				IsError(__FILE__, __LINE__);
				glCompileShader(geometry);
				IsError(__FILE__, __LINE__);
				CheckCompileErrors(geometry, "GEOMETRY");
			}
			// shader Program
			id = glCreateProgram();
			IsError(__FILE__, __LINE__);
			glAttachShader(id, vertex);
			IsError(__FILE__, __LINE__);
			glAttachShader(id, fragment);
			IsError(__FILE__, __LINE__);
			if (!geometryPath.empty())
			{
				glAttachShader(id, geometry);
				IsError(__FILE__, __LINE__);
			}
			glLinkProgram(id);
			IsError(__FILE__, __LINE__);
			CheckCompileErrors(id, "PROGRAM");
			// delete the shaders as they're linked into our program now and no
			// longer necessary
			glDeleteShader(vertex);
			IsError(__FILE__, __LINE__);
			glDeleteShader(fragment);
			IsError(__FILE__, __LINE__);
			if (!geometryPath.empty())
			{
				glDeleteShader(geometry);
				IsError(__FILE__, __LINE__);
			}
		}
		// activate the shader
		void Use()
		{
			glUseProgram(id);
			IsError(__FILE__, __LINE__);
		}
		// utility uniform functions
		void SetBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
			IsError(__FILE__, __LINE__);
		}
		void SetInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
			IsError(__FILE__, __LINE__);
		}
		void SetFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
			IsError(__FILE__, __LINE__);
		}
		void SetVec2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec2(const std::string& name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
			IsError(__FILE__, __LINE__);
		}
		void SetVec3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
			IsError(__FILE__, __LINE__);
		}
		void SetVec4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec4(
			const std::string& name,
			float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
			IsError(__FILE__, __LINE__);
		}
		void SetMat2(const std::string& name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(
				glGetUniformLocation(id, name.c_str()),
				1,
				GL_FALSE,
				&mat[0][0]);
			IsError(__FILE__, __LINE__);
		}
		void SetMat3(const std::string& name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(
				glGetUniformLocation(id, name.c_str()),
				1,
				GL_FALSE,
				&mat[0][0]);
			IsError(__FILE__, __LINE__);
		}
		void SetMat4(const std::string& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(
				glGetUniformLocation(id, name.c_str()),
				1,
				GL_FALSE,
				&mat[0][0]);
			IsError(__FILE__, __LINE__);
		}

	private:
		// utility function for checking shader compilation/linking errors.
		void CheckCompileErrors(GLuint shader, std::string type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					throw std::runtime_error(
						"Shader compilation error: " + type + " : " + infoLog);
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					throw std::runtime_error(
						"Shader linking error: " + type + " : " + infoLog);
				}
			}
		}
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
