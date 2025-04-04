#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>
#include "shader.h"

namespace UAV
{
struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Material 
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Mesh 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    unsigned int VAO, VBO, EBO;

    void setupMesh();
    void Draw(Shader& shader);
};

class Model 
{
public:
    Model(const std::string& path);
    void Draw(Shader& shader);
    glm::vec3 GetCenter() const;
    float GetScaleFactor() const;

private:
    std::vector<Mesh> meshes;
    glm::vec3 center;
    float scale_factor;
    glm::vec3 min_bounds;
    glm::vec3 max_bounds;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void calculateBounds();
};
}
