#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include <filesystem>

#include "shader.h"
#include "mesh.h"

struct Material {
glm::vec3 Diffuse;
glm::vec3 Specular;
glm::vec3 Ambient;
};


class Model 
{
    public:
        Model(std::string const &path);

        void Draw(Shader &shader);	
        void Rotate(float angle, glm::vec3 coord);
        void Scale(glm::vec3 size);
        void Move(glm::vec3 coord);


    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;
        glm::mat4 model_mat;
        std::vector<Texture> textures_loaded; 
        Material material;
        float Shininess;
        bool UseMaterial;

        void loadModel(std::string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
        std::vector<Texture> lookForTexture(std::string const path,aiMaterial *mat);
        Material loadMaterial(aiMaterial* mat);

};