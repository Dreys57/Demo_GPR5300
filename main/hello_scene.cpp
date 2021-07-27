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
#include "model.h"
#include "cubemap.h"

namespace gl {

	class HelloScene : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt, SDL_Window* window) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix();
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrix() const;
		unsigned int LoadBasicTexture(char const* path);
		void RenderScene(std::unique_ptr<Shader>& shader);

	protected:
		unsigned int skyboxVAO_;
		unsigned int skyboxVBO_;
		unsigned int skyboxTexture_;
		const unsigned int SHADOW_WIDTH = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;
		unsigned int depthMapFBO;
		unsigned int depthMap;
		unsigned int woodTexture;
		unsigned int planeVBO;
		unsigned int planeVAO;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		std::unique_ptr<Model> tree_ = nullptr;
		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Shader> mainShaders_ = nullptr;
		std::unique_ptr<Shader> depthShaders_ = nullptr;
		std::unique_ptr<Cubemap> skybox_;

		std::unique_ptr<Shader> skyboxShaders_ = nullptr;

		std::vector<std::string> texturesFaces_;

		glm::vec2 windowSize_;
		glm::vec2 windowCenter_;

		glm::vec3 lightPosition_ = glm::vec3(0.0, 20.0, 30.0);
		glm::vec3 lightDir_ = glm::normalize(glm::vec3(-1.0, -1.0, 1.0));

		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);
		glm::mat4 inv_model_ = glm::mat4(1.0f);

		std::string path_ = "";
	};

	void HelloScene::IsError(const std::string& file, int line) const
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

	void HelloScene::Init()
	{
		glEnable(GL_DEPTH_TEST);

		camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 50.0f));

		mainShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/shadow.vert",
			path_ + "data/shaders/shadow.frag");
		depthShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/depthmap.vert",
			path_ +"data/shaders/depthmap.frag");
		
		//plane
		float planeVertices[] = {
			// positions            // normals         // texcoords
			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
			 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);

		//tree
		tree_ = std::make_unique<Model>(path_ + "data/meshes/tree.obj");
		
		woodTexture = LoadBasicTexture((path_ + "data/textures/wood.png").c_str());
		
		//skybox
		skyboxShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/skyboxShader.vert",
			path_ + "data/shaders/skyboxShader.frag");
		skyboxShaders_->Use();
		skyboxShaders_->SetInt("skybox", 0);
		
		
		texturesFaces_ = {
			path_ + "data/textures/skybox/right.jpg",
			path_ + "data/textures/skybox/left.jpg",
			path_ + "data/textures/skybox/top.jpg",
			path_ + "data/textures/skybox/bottom.jpg",
			path_ + "data/textures/skybox/front.jpg",
			path_ + "data/textures/skybox/back.jpg"
		};

		skybox_ = std::make_unique<Cubemap>(texturesFaces_);

		//depthmap
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		GLenum drawBuffer = GL_NONE;
		glDrawBuffers(0, &drawBuffer);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
	}

	void HelloScene::SetModelMatrix(seconds dt)
	{
		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
		inv_model_ = glm::transpose(glm::inverse(model_));
	}

	void HelloScene::SetViewMatrix()
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloScene::SetProjectionMatrix()
	{
		projection_ = glm::perspective(
			glm::radians(45.0f),
			4.0f / 3.0f,
			0.1f,
			100.f);
	}

	void HelloScene::SetUniformMatrix() const
	{
		mainShaders_->Use();
		mainShaders_->SetMat4("model", model_);
		mainShaders_->SetMat4("view", view_);
		mainShaders_->SetMat4("projection", projection_);
		mainShaders_->SetMat4("inv_model", inv_model_);
		mainShaders_->SetVec3("camera_position", camera_->position);
	}

	void HelloScene::Update(seconds dt, SDL_Window* window)
	{
		int x;
		int y;
		SDL_GetWindowSize(window, &x, &y);
		windowSize_ = glm::vec2(x, y);
		
		delta_time_ = dt.count();
		time_ += delta_time_;

		camera_->SetState(glm::vec3(50.0f * cos(time_), 6.0f, 50.0f * sin(time_)), glm::normalize(-glm::vec3(cos(time_), 0.0f, sin(time_))));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float nearPlane = 1.0f;
		float farPlane = 100.0f;

		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearPlane, farPlane);
		lightView = glm::lookAt(glm::vec3(-1.0f, 12.0f, -15.0f) - (50.0f * lightDir_),glm::vec3(-1.0f, 12.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		//render from light's pov
		depthShaders_->Use();
		depthShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		RenderScene(depthShaders_);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//render scene as normal
		mainShaders_->Use();
		SetProjectionMatrix();
		SetViewMatrix();
		mainShaders_->SetMat4("projection", projection_);
		mainShaders_->SetMat4("view", view_);
		mainShaders_->SetVec3("viewPos", camera_->position);
		mainShaders_->SetVec3("lightDir", lightDir_);
		mainShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		mainShaders_->SetInt("shadowMap", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderScene(mainShaders_);

		skybox_->Draw(skyboxShaders_, view_, projection_, camera_);
	}

	void HelloScene::Destroy() {}

	void HelloScene::OnEvent(SDL_Event& event)
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

	void HelloScene::DrawImGui() {}

	unsigned HelloScene::LoadBasicTexture(char const* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	void HelloScene::RenderScene(std::unique_ptr<Shader>& shader)
	{
		//plane
		glm::mat4 model = glm::mat4(1.0f);
		shader->SetMat4("model", model);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//tree
		model_ = glm::mat4(1.0f);
		model_ = glm::translate(model_, glm::vec3(0.0f, -2.0f, 0.0f));
		model_ = glm::rotate(model_, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model_ = glm::scale(model_, glm::vec3(1.5f, 1.5f, 1.5f));

		shader->SetMat4("model", model_);
		tree_->Draw(shader);
	}


} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloScene program;
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