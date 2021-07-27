#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "material.h"
#include "mesh.h"
#include "texture.h"

namespace gl {

	class Model {
	public:
        std::string directory;
		std::string textureDirectory = "data/textures";
		
		Model(const std::string& filename)
		{
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

			if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
                std::cout << "ERROR ASSIMP" << importer.GetErrorString() << "\n";
                return;
			}

            directory = filename.substr(0, filename.find_last_of('/'));

			ProcessNode(scene->mRootNode, scene);
		}

		void Draw(std::unique_ptr<Shader>& shader)
		{
			for(unsigned int i = 0; i < meshes.size(); ++i)
			{
				meshes[i].Draw(shader);
			}
		}
		std::vector<Mesh> meshes;
		std::vector<Material> materials;

	private:

		void ProcessNode(aiNode* node, const aiScene* scene)
		{
			for(unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(ProcessMesh(mesh, scene));
			}

			for(unsigned int i = 0; i < node->mNumChildren; ++i)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<TextureStruct> textures;

			for(unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				Vertex vertex;

				glm::vec3 positions;
				glm::vec3 normals;
				glm::vec2 texCoord;
				glm::vec3 tangeants;

				positions.x = mesh->mVertices[i].x;
				positions.y = mesh->mVertices[i].y;
				positions.z = mesh->mVertices[i].z;
				vertex.position = positions;

				normals.x = mesh->mNormals[i].x;
				normals.y = mesh->mNormals[i].y;
				normals.z = mesh->mNormals[i].z;
				vertex.normal = normals;

				texCoord.x = mesh->mTextureCoords[0][i].x;
				texCoord.y = mesh->mTextureCoords[0][i].y;
				vertex.texture = texCoord;

				tangeants.x = mesh->mTangents[i].x;
				tangeants.y = mesh->mTangents[i].y;
				tangeants.z = mesh->mTangents[i].z;
				vertex.tangeant = tangeants;

				vertices.push_back(vertex);
			}

			for(unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];

				for(unsigned int j = 0; j < face.mNumIndices; ++j)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			if(mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				std::vector<TextureStruct> diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

				std::vector<TextureStruct> specularTextures = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

				std::vector<TextureStruct> normalmap = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
				textures.insert(textures.end(), normalmap.begin(), normalmap.end());
			}

			return Mesh(vertices, indices, textures);
		}

		std::vector<TextureStruct> LoadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName)
		{
			std::vector<TextureStruct> textures;
			const std::size_t textureCount = material->GetTextureCount(textureType);

			for(unsigned int i = 0; i < textureCount; ++i)
			{
				aiString string;

				material->GetTexture(textureType, i, &string);

				bool skip = false;
				const std::size_t sizeOfTextureLoaded = texturesLoaded_.size();

				for(unsigned int j = 0; j < texturesLoaded_.size(); ++j)
				{
					if(std::strcmp(texturesLoaded_[j].path.data(), string.C_Str()) == 0)
					{
						textures.push_back(texturesLoaded_[j]);
						skip = true;
						break;
					}
					assert(j < sizeOfTextureLoaded);
				}

				if(!skip)
				{
					TextureStruct texture;

					texture.id = LoadTextureFromFile(string.C_Str(), textureDirectory, textureType);
					texture.type = typeName;
					texture.path = string.C_Str();
					
					textures.push_back(texture);
					texturesLoaded_.push_back(texture);
				}
			}

			return textures;
		}

		std::vector<TextureStruct> texturesLoaded_;
	};

}//End namepsace gl

