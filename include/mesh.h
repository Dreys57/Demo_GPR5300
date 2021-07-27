#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h"
#include "shader.h"


namespace gl {

    class Vertex
    {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;
        glm::vec3 tangeant;
    };

    class Mesh
    {
    public:        
        bool hasNormalTexture = false;
    	
        Mesh(const std::vector<Vertex>& vertices,
            const std::vector<unsigned int>& indices,
            std::vector<TextureStruct>& textures) :
            vertices_(vertices),
    		indices_(indices),
    		textures_(textures)
        {
        	
            // VAO binding should be before VAO.
            glGenVertexArrays(1, &VAO_);
            IsError(__FILE__, __LINE__);
            glBindVertexArray(VAO_);
            IsError(__FILE__, __LINE__);

            // EBO.
            glGenBuffers(1, &EBO_);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices_.size() * sizeof(unsigned int),
                indices_.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);
           

            // VBO.
            glGenBuffers(1, &VBO_);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ARRAY_BUFFER,
                vertices_.size() * sizeof(Vertex),
                vertices_.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);

            glEnableVertexAttribArray(0);       	
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (GLvoid*) nullptr);
            IsError(__FILE__, __LINE__);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (GLvoid*)offsetof(Vertex, normal));
            IsError(__FILE__, __LINE__);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (GLvoid*)offsetof(Vertex, texture));
        	
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (GLvoid*)offsetof(Vertex, tangeant));

            glBindVertexArray(0);
        }
        void BindTextures(std::unique_ptr<Shader>& shader) const
        {
            unsigned int diffuseNb = 1;
            unsigned int specularNb = 1;
            unsigned int normalNb = 1;
            unsigned int emissionNb = 1;

        	for(unsigned int i = 0; i < textures_.size(); ++i)
        	{
                glActiveTexture(GL_TEXTURE0 + i);
                std::string nb;
                std::string name = textures_[i].type;

        		if(name == "texture_diffuse")
        		{
                    nb = std::to_string(diffuseNb++);
        		}
                else if(name == "texture_specular")
                {
                    nb = std::to_string(specularNb++);
                }
                else if(name == "texture_normal")
                {
                    nb = std::to_string(normalNb++);
                }
                else if(name == "texture_emission")
                {
                    nb = std::to_string(emissionNb++);
                }

                shader->SetInt((name + nb).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        	}

            glActiveTexture(GL_TEXTURE0);
        }
        void Draw(std::unique_ptr<Shader>& shader)
        {
            BindTextures(shader);

            glBindVertexArray(VAO_);
            glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

    private:
        unsigned int VAO_ = 0;
        unsigned int VBO_ = 0;
        unsigned int EBO_ = 0;
    	
        void IsError(const std::string& file, int line) const
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

        std::vector<Vertex> vertices_;
        std::vector<TextureStruct> textures_;
        std::vector<unsigned int> indices_;
    };

} // End namespace gl.
