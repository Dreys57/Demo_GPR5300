#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include "engine.h"

namespace gl {

	class HelloTexture : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		unsigned int VBO_;
		unsigned int VAO_[2];
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;
		void IsError(const std::string& file, int line);
	};

	void HelloTexture::IsError(const std::string& file, int line)
	{
		auto error_code = glGetError();
		if (error_code != GL_NO_ERROR)
		{
			std::cerr
				<< error_code
				<< " in file: " << file
				<< " at line: " << line
				<< "\n";
		}
	}

	void HelloTexture::Init()
	{
		std::array<float, 24> vertices = {
			-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0,
			 0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
			-0.5f,  0.5f, 0.0f, 0.0, 0.0, 1.0,
			 0.5f,  0.5f, 0.0f, 1.0, 0.0, 0.0
		};

		std::array<std::uint32_t, 6> indices{
			0, 1, 2,
			1, 2, 3
		};

		// VAO binding should be before VBO.
		glGenVertexArrays(2, VAO_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_[0]);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_[1]);

		// EBO.
		glGenBuffers(1, &EBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(float),
			indices.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);

		// VBO.
		glGenBuffers(1, &VBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),
			vertices.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);

		GLintptr vertex_color_offset = 3 * sizeof(float);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			6 * sizeof(float),
			0);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			6 * sizeof(float),
			(GLvoid*)vertex_color_offset);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(0);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(1);
		IsError(__FILE__, __LINE__);

		std::string path = "..\\";

		std::ifstream ifs_vert(
			path + "data\\shaders\\hello_texture\\texture.vert");
		std::ifstream ifs_frag(
			path + "data\\shaders\\hello_texture\\texture.frag");

		if (!ifs_vert.is_open())
		{
			throw std::runtime_error("Could not open vertex file.");
		}
		if (!ifs_frag.is_open())
		{
			throw std::runtime_error("Could not open fragment file.");
		}

		std::string vertex_source{ std::istreambuf_iterator<char>(ifs_vert), {} };
		std::string fragment_source{ std::istreambuf_iterator<char>(ifs_frag), {} };

		// Vertex shader.
		vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
		IsError(__FILE__, __LINE__);
		const char* ptr1 = vertex_source.c_str();
		glShaderSource(vertex_shader_, 1, &ptr1, 0);
		IsError(__FILE__, __LINE__);
		glCompileShader(vertex_shader_);
		GLint status = 0;
		glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint infoLogLength;
			glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar* infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
			std::cerr << "VS> could not compile: " << infoLog << "\n";
			delete[] infoLog;
			exit(0);
		}

		// Fragment shader.
		fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
		IsError(__FILE__, __LINE__);
		const char* ptr2 = fragment_source.c_str();
		glShaderSource(fragment_shader_, 1, &ptr2, 0);
		IsError(__FILE__, __LINE__);
		glCompileShader(fragment_shader_);
		glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint infoLogLength;
			glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar* infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
			std::cerr << "FS> could not compile: " << infoLog << "\n";
			delete[] infoLog;
			exit(0);
		}

		// Program.
		program_ = glCreateProgram();
		IsError(__FILE__, __LINE__);
		glAttachShader(program_, vertex_shader_);
		IsError(__FILE__, __LINE__);
		glAttachShader(program_, fragment_shader_);
		IsError(__FILE__, __LINE__);
		//glBindAttribLocation(program_, 0, "aPos");
		glLinkProgram(program_);
		IsError(__FILE__, __LINE__);
		assert(program_ != 0);
	}

	void HelloTexture::Update(seconds dt)
	{
		glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
		IsError(__FILE__, __LINE__);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		IsError(__FILE__, __LINE__);
		glUseProgram(program_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_[0]);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_[1]);
		IsError(__FILE__, __LINE__);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);
	}

	void HelloTexture::Destroy()
	{
		glDeleteProgram(program_);
		IsError(__FILE__, __LINE__);
		glDeleteShader(vertex_shader_);
		IsError(__FILE__, __LINE__);
		glDeleteShader(fragment_shader_);
		IsError(__FILE__, __LINE__);
	}

	void HelloTexture::OnEvent(SDL_Event& event)
	{
		if ((event.type == SDL_KEYDOWN) &&
			(event.key.keysym.sym == SDLK_ESCAPE))
		{
			exit(0);
		}
	}

	void HelloTexture::DrawImGui()
	{
	}

} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloTexture program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}
