#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <limits>

namespace UAV
{
void Mesh::setupMesh() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) 
{
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model(const std::string& path) 
{
    loadModel(path);
    calculateBounds();
}

void Model::Draw(Shader& shader) 
{
    for (auto& mesh : meshes)
        mesh.Draw(shader);
}

glm::vec3 Model::GetCenter() const { return center; }
float Model::GetScaleFactor() const { return scale_factor; }

void Model::loadModel(const std::string& path) 
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Model loaded successfully: " << path << std::endl;
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) 
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals()) 
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) 
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else 
            vertex.TexCoords = glm::vec2(0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh result;
    result.vertices = vertices;
    result.indices = indices;

    if (mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D ambient(0.f), diffuse(0.f), specular(0.f);
        float shininess = 32.0f;

        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->Get(AI_MATKEY_SHININESS, shininess);

        if (ambient.r == 0 && ambient.g == 0 && ambient.b == 0) ambient = diffuse;
        if (specular.r == 0 && specular.g == 0 && specular.b == 0) specular = aiColor3D(0.2f);
        if (shininess <= 0.0f) shininess = 32.0f;

        result.material.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        result.material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        result.material.specular = glm::vec3(specular.r, specular.g, specular.b);
        result.material.shininess = shininess;
    }
    else 
    {
        result.material.ambient = glm::vec3(0.2f);
        result.material.diffuse = glm::vec3(0.8f);
        result.material.specular = glm::vec3(0.5f);
        result.material.shininess = 32.0f;
    }

    result.setupMesh();
    return result;
}

void Model::calculateBounds() 
{
    if (meshes.empty()) 
    {
        min_bounds = max_bounds = center = glm::vec3(0.0f);
        scale_factor = 1.0f;
        return;
    }

    min_bounds = glm::vec3(std::numeric_limits<float>::max());
    max_bounds = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& mesh : meshes) 
    {
        for (const auto& vertex : mesh.vertices) 
        {
            min_bounds = glm::min(min_bounds, vertex.Position);
            max_bounds = glm::max(max_bounds, vertex.Position);
        }
    }

    center = (min_bounds + max_bounds) * 0.5f;
    glm::vec3 size = max_bounds - min_bounds;
    scale_factor = 2.0f / std::max(std::max(size.x, size.y), size.z);
}
}
