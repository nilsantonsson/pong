#ifndef PONG_GUI_H
#define PONG_GUI_H

#include "Helper.h"
#include "Image.h"
#include "Texture.h"
#include "Mesh.h"
#include "Renderer.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <cstdint>
#include <fstream>
#include <vector>

class Gui {
public:
    Gui(
        std::shared_ptr<Renderer> renderer,
        std::shared_ptr<Effect> orthoEffect) : 

        renderer(renderer),
        orthoEffect(orthoEffect) {

        std::ifstream ifs("../data/arial.ttf", std::ios::in | std::ios::binary);
        if (ifs.is_open()) {
            std::vector<std::uint8_t> ttfBuffer {
                std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>()
            };

            stbtt_fontinfo font;
            stbtt_InitFont(
                &font,
                ttfBuffer.data(),
                stbtt_GetFontOffsetForIndex(ttfBuffer.data(), 0));

            for (std::uint8_t i = 0; i < 10; i++) {
                float scaleY = stbtt_ScaleForPixelHeight(&font, 120);
                int imageWidth;
                int imageHeight;
                std::uint8_t *imageData = stbtt_GetCodepointBitmap(
                    &font,
                    10,
                    scaleY,
                    indexToChar(i),
                    &imageWidth,
                    &imageHeight,
                    0,
                    0);

                auto image = std::make_shared<Image>(imageWidth, imageHeight, imageData);
                auto texture = std::make_shared<Texture>(image);
                mapCharToTexture[indexToChar(i)] = texture;
                renderer->addTexture(texture);
            }

            pointsLeftMesh = createTextMesh(glm::vec2(-100.0, -310.0));
            pointsRightMesh = createTextMesh(glm::vec2(100.0, -310.0));

            enabled = true;

            ifs.close();

        } else {
            std::cerr << "Font file not found\n";
        }
    }

    void update(std::uint8_t pointsLeft, std::uint8_t pointsRight) {
        if (enabled) {
            pointsLeftMesh->texture = mapCharToTexture[indexToChar(pointsLeft)];
            pointsRightMesh->texture = mapCharToTexture[indexToChar(pointsRight)];
        }
    }

private:
    std::shared_ptr<Mesh> createTextMesh(glm::vec2 position) {
        auto textMesh = buildQuadMesh(30.0, 40.0, orthoEffect);
        textMesh->texture = mapCharToTexture[indexToChar(0)];
        textMesh->transform = createTranslation(position);
        renderer->addMesh(textMesh);
        return textMesh;
    }

    char indexToChar(int index) {
        return index + 48;
    }

    bool enabled = false;

    std::shared_ptr<Effect> orthoEffect;
    std::shared_ptr<Mesh> pointsLeftMesh;
    std::shared_ptr<Mesh> pointsRightMesh;
    std::map<char, std::shared_ptr<Texture>> mapCharToTexture;

    std::shared_ptr<Renderer> renderer;
};

#endif // PONG_GUI_H
