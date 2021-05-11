#ifndef PONG_RENDERER_H
#define PONG_RENDERER_H

#include "Effect.h"
#include "Texture.h"
#include "Mesh.h"

#include "glad.h"

#if defined(__WIN32__)
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

class Renderer {
public:
    Renderer(std::uint32_t canvasWidth, std::uint32_t canvasHeight) {
        this->canvasWidth = canvasWidth;
        this->canvasHeight = canvasHeight;
    }

    void addEffect(std::shared_ptr<Effect> effect) {
        effects.push_back(effect);
    }

    void addTexture(std::shared_ptr<Texture> texture) {
        textures.push_back(texture);
    }

    void addMesh(std::shared_ptr<Mesh> mesh) {
        meshes.push_back(mesh);
    }

    void prepare() {
        for (auto effect : effects) {
            prepareEffect(effect);
        }

        for (auto texture : textures) {
            prepareTexture(texture);
        }

        for (auto mesh : meshes) {
            prepareMesh(mesh);
        }
    }

    void render() {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto mesh : meshes) {

            glUseProgram(mesh->effect->shaderProgram);

            float sw = canvasWidth*0.5;
            float sh = canvasHeight*0.5;
            glm::mat4 orthoProjection = glm::ortho(-sw, sw, -sh, sh, -100.0f, 100.0f);
            useMatrix(mesh->effect->effectParameters[0].id, orthoProjection);

            useMatrix(mesh->effect->effectParameters[1].id, mesh->transform);

            if (mesh->texture != nullptr) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh->texture->textureId);
                glUniform1i(mesh->effect->effectParameters[2].id, 0);
            }

            renderMesh(mesh);
        }
    }

private:
    void renderMesh(std::shared_ptr<Mesh> mesh) {

        glBindVertexArray(mesh->vertexArrayObject);
        glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void prepareEffect(std::shared_ptr<Effect> effect) {

        std::uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        compileShader(vertexShader, effect->vertexShaderSource);

        std::uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        compileShader(fragmentShader, effect->fragmentShaderSource);

        std::uint32_t shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        int success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            printError(shaderProgram, "Shader program failed");
        }

        effect->shaderProgram = shaderProgram;

        for (std::uint8_t i = 0; i < effect->effectParameters.size(); i++) {
            GLint parameterId0 = glGetUniformLocation(
                effect->shaderProgram,
                effect->effectParameters[i].name.c_str());
            effect->effectParameters[i].id = parameterId0;
        }
    }

    void prepareTexture(std::shared_ptr<Texture> texture) {

        glActiveTexture(GL_TEXTURE0);

        GLuint textureId;
        glGenTextures(1, &textureId);
    
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            texture->image->width,
            texture->image->height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            texture->image->data);

        texture->textureId = textureId;
    }

    void prepareMesh(std::shared_ptr<Mesh> mesh) {

        glGenVertexArrays(1, &mesh->vertexArrayObject);
        glBindVertexArray(mesh->vertexArrayObject);

        glGenBuffers(1, &mesh->vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, mesh->verticesTotalSize, mesh->vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &mesh->elementBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesTotalSize, mesh->indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, VertexComponentCount, GL_FLOAT, GL_FALSE, VertexStride * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color attribute
        glVertexAttribPointer(1, ColorComponentCount, GL_FLOAT, GL_FALSE, VertexStride * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);

        // tex coord attribute
        glVertexAttribPointer(2, TexCoordComponentCount, GL_FLOAT, GL_FALSE, VertexStride * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void compileShader(std::uint32_t shader, std::string shaderSource) {

        const char *source = shaderSource.c_str();
        glShaderSource(shader, 1, &source, nullptr);

        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            printError(shader, "Shader compilation failed");
        }
    }

    void printError(std::uint32_t shader, std::string message) {
        const int InfoLogSize = 512;
        char infoLog[InfoLogSize];
        glGetShaderInfoLog(shader, InfoLogSize, nullptr, infoLog);
        std::cout << message << ": " << std::string(infoLog);
    }

    void useMatrix(std::int32_t effectParameterId, glm::mat4 matrix) {
        glUniformMatrix4fv(effectParameterId, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    std::uint32_t canvasWidth;
    std::uint32_t canvasHeight;

    std::vector<std::shared_ptr<Effect>> effects;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Mesh>> meshes;
};

#endif // PONG_RENDERER_H
