#pragma once

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "shader.h"
#include "texture.h"

struct Vertex{
    glm::vec3 position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

class Mesh{
public:
    unsigned int VAO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh();
    Mesh(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<Texture> textures);
  //  ~Mesh();
    void Draw(Shader &shader);

private:
    unsigned int VBO,EBO;
    int mat_modelID,u_tex;
    glm::mat4 model;

    void setup_mesh();
};