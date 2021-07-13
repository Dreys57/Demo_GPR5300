#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

namespace gl {

	class HelloOutline : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrix() const;

	protected:
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		std::unique_ptr<Mesh> mesh_ = nullptr;
		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Texture> texture_diffuse_ = nullptr;
		std::unique_ptr<Texture> texture_specular_ = nullptr;
		std::unique_ptr<Shader> cube_shader_ = nullptr;
		std::unique_ptr<Shader> outline_shader_ = nullptr;

		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);
		glm::mat4 inv_model_ = glm::mat4(1.0f);
	};

	void HelloOutline::IsError(const std::string& file, int line) const
	{
		auto error_code = glGetError();
		if (error_code != GL_NO_ERROR)
		{
			throw std::runtime_error(
				std::to_string(error_code) +
				" in file: " + file +
				" at line: " + std::to_string(line));
		}
	}

	void HelloOutline::Init()
	{
		glEnable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		glEnable(GL_STENCIL_TEST);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		IsError(__FILE__, __LINE__);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		IsError(__FILE__, __LINE__);

		camera_ = std::make_unique<Camera>(glm::vec3(.0f, .0f, 3.0f));

		std::string path = "../";

		mesh_ = std::make_unique<Mesh>(path + "data/meshes/Apple.obj");
		texture_diffuse_ = std::make_unique<Texture>(
			path + "data/textures/Color.jpg");
		texture_specular_ = std::make_unique<Texture>(
			path + "data/textures/Roughness.jpg");

		cube_shader_ = std::make_unique<Shader>(
			path + "data/shaders/hello_mesh/mesh.vert",
			path + "data/shaders/hello_mesh/mesh.frag");

		outline_shader_ = std::make_unique<Shader>(
			path + "data/shaders/hello_outline/outline.vert",
			path + "data/shaders/hello_outline/outline.frag");

		// Bind uniform to program.
		cube_shader_->Use();
		texture_diffuse_->Bind(0);
		cube_shader_->SetInt("Diffuse", 0);
		// texture_diffuse_->UnBind();
		texture_specular_->Bind(1);
		cube_shader_->SetInt("Specular", 1);
		// texture_specular_->UnBind();

		glClearColor(0.6f, 0.2f, 0.8f, 1.0f);
		IsError(__FILE__, __LINE__);
	}

	void HelloOutline::SetModelMatrix(seconds dt)
	{
		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
		inv_model_ = glm::transpose(glm::inverse(model_));
	}

	void HelloOutline::SetViewMatrix(seconds dt)
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloOutline::SetProjectionMatrix()
	{
		projection_ = glm::perspective(
			glm::radians(45.0f),
			4.0f / 3.0f,
			0.1f,
			100.f);
	}

	void HelloOutline::SetUniformMatrix() const
	{
		cube_shader_->Use();
		cube_shader_->SetMat4("model", model_);
		cube_shader_->SetMat4("view", view_);
		cube_shader_->SetMat4("projection", projection_);
		cube_shader_->SetMat4("inv_model", inv_model_);
		cube_shader_->SetVec3("camera_position", camera_->position);
	}

	void HelloOutline::Update(seconds dt)
	{
		delta_time_ = dt.count();
		time_ += delta_time_;
		SetViewMatrix(dt);
		SetModelMatrix(dt);
		SetProjectionMatrix();
		SetUniformMatrix();
		glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);
		IsError(__FILE__, __LINE__);

		// First pass.
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilMask(0xff);
		IsError(__FILE__, __LINE__);
		mesh_->Bind();
		cube_shader_->Use();
		glDrawElements(GL_TRIANGLES, mesh_->nb_vertices, GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);

		// Second pass with the outline.
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilMask(0x00);
		IsError(__FILE__, __LINE__);
		glDisable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		float scale = 1.2f;
		outline_shader_->Use();
		glm::mat4 model = model_;
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		outline_shader_->SetMat4("model", model);
		outline_shader_->SetMat4("view", view_);
		outline_shader_->SetMat4("projection", projection_);
		mesh_->Bind();
		glDrawElements(GL_TRIANGLES, mesh_->nb_vertices, GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);

		// Cleanup
		glStencilMask(0xFF);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		IsError(__FILE__, __LINE__);
		glEnable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
	}

	void HelloOutline::Destroy() {}

	void HelloOutline::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_->ProcessKeyboard(
					CameraMovementEnum::FORWARD,
					delta_time_);
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_->ProcessKeyboard(
					CameraMovementEnum::BACKWARD,
					delta_time_);
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_->ProcessKeyboard(
					CameraMovementEnum::LEFT,
					delta_time_);
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_->ProcessKeyboard(
					CameraMovementEnum::RIGHT,
					delta_time_);
			}
		}
	}

	void HelloOutline::DrawImGui() {}

} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloOutline program;
	gl::Engine engine(program);
	try
	{
		engine.Run();
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return EXIT_SUCCESS;
}