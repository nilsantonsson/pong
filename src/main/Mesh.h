#ifndef PONG_MESH_H
#define PONG_MESH_H

#include "Effect.h"
#include "Texture.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>

static const std::uint8_t VertexComponentCount = 3;
static const std::uint8_t ColorComponentCount = 3;
static const std::uint8_t TexCoordComponentCount = 2;
static const std::uint32_t VertexStride = VertexComponentCount + ColorComponentCount + 
    TexCoordComponentCount;

struct Mesh {
    Mesh(std::shared_ptr<Effect> effect) : effect(effect) {}

    std::uint32_t vertexCount;
    std::uint32_t verticesTotalSize;
    float *vertices;

    int indexCount;
    int indicesTotalSize;
    std::uint32_t *indices;

    std::uint32_t vertexArrayObject;
    std::uint32_t vertexBufferObject;
    std::uint32_t elementBufferObject;

    std::shared_ptr<Effect> effect;
    std::shared_ptr<Texture> texture;

    glm::mat4 transform = glm::mat4(1.0);
};

std::shared_ptr<Mesh> buildQuadMesh(float width, float height, std::shared_ptr<Effect> effect) {

    float halfWidth = width * 0.5;
    float halfHeight = height * 0.5;

    auto mesh = std::make_shared<Mesh>(effect);

    // Vertices consists of following columns:
    // Positions: 3 elements
    // Colors: 3 elements (no alpha)
    // Texture coords: 2 elements
    float vertices[] = {
        halfWidth, halfHeight, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        halfWidth, -halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -halfWidth, -halfHeight, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -halfWidth,  halfHeight, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    mesh->vertexCount = 4;
    int verticesSize = mesh->vertexCount *
        (VertexComponentCount + ColorComponentCount + TexCoordComponentCount);
    mesh->verticesTotalSize = sizeof(float) * verticesSize;

    mesh->vertices = new float[verticesSize];
    std::copy(vertices, vertices + verticesSize, mesh->vertices);

    // Indices
    std::uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    mesh->indexCount = 6;
    mesh->indicesTotalSize = sizeof(std::uint32_t) * mesh->indexCount;
    mesh->indices = new std::uint32_t[mesh->indexCount];
    std::copy(indices, indices + mesh->indexCount, mesh->indices);

    return mesh;
}

#endif // PONG_MESH_H
