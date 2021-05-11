#ifndef PONG_EFFECT_H
#define PONG_EFFECT_H

#include "EffectParameter.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Effect {
    std::uint32_t shaderProgram;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

   	std::vector<EffectParameter> effectParameters;
};

std::shared_ptr<Effect> buildOrthoEffect() {
    auto effect = std::make_shared<Effect>();

    effect->vertexShaderSource = std::string(
        "#version 330 core\n"

        "uniform mat4 model;"
        "uniform mat4 projection;"

        "layout(location = 0) in vec4 vertexPosition;"
        "layout(location = 2) in vec2 vertexTexCoord;"

        "out vec2 uv;"

        "void main() {"
        "  	vec4 v0 = model * vertexPosition;"
        "   gl_Position = projection * v0;"
        "   uv = vertexTexCoord;"
        "}");

    effect->fragmentShaderSource = std::string(
        "#version 330 core\n"

        "uniform sampler2D tex;"

        "in vec2 uv;"

        "layout(location = 0) out vec4 result;"

        "void main() {"
        "   vec4 temp = texture(tex, vec2(uv.x, 1.0 - uv.y));"
        "   result = vec4(temp.r);"
        "}");

    EffectParameter epProjection {"projection"};
    effect->effectParameters.push_back(epProjection);

    EffectParameter epModel {"model"};
    effect->effectParameters.push_back(epModel);

    EffectParameter epTexture {"tex"};
    effect->effectParameters.push_back(epTexture);

    return effect;
}

#endif // PONG_EFFECT_H
