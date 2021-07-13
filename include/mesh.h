#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>

namespace gl {

    class Vertex
    {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;
    };

    class Mesh
    {
    public:        
        const unsigned int material_index;
        const unsigned int nb_vertices;

        Mesh(const std::vector<Vertex>& vertices,
            const std::vector<std::uint32_t>& indices,
            const unsigned int material_id) :   
    		nb_vertices(static_cast<unsigned int>(indices.size())),
            material_index(material_id)
        {
            // VAO binding should be before VAO.
            glGenVertexArrays(1, &VAO);
            IsError(__FILE__, __LINE__);
            glBindVertexArray(VAO);
            IsError(__FILE__, __LINE__);

            // EBO.
            glGenBuffers(1, &EBO);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(float),
                indices.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);

            // VBO.
            glGenBuffers(1, &VBO);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                vertices.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);

            GLintptr vertex_color_offset = 3 * sizeof(float);
            GLintptr vertex_tex_offset = 6 * sizeof(float);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                0);
            IsError(__FILE__, __LINE__);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                (GLvoid*)vertex_color_offset);
            IsError(__FILE__, __LINE__);
            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                (GLvoid*)vertex_tex_offset);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(0);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(1);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(2);
            IsError(__FILE__, __LINE__);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            IsError(__FILE__, __LINE__);
            glBindVertexArray(0);
            IsError(__FILE__, __LINE__);
        }
        void Bind() const
        {
            glBindVertexArray(VAO);
            IsError(__FILE__, __LINE__);
        }
        void UnBind() const
        {
            glBindVertexArray(0);
            IsError(__FILE__, __LINE__);
        }

    private:
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;
    	
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
    };

} // End namespace gl.
