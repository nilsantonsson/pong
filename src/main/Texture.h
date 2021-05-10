#ifndef PONG_TEXTURE_H
#define PONG_TEXTURE_H

#include "Image.h"

#include <cstdint>

struct Texture {
    Texture(std::shared_ptr<Image> image) : image(image) {}

    std::shared_ptr<Image> image;
    std::uint32_t textureId;
};

#endif // PONG_TEXTURE_H
