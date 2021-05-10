#ifndef PONG_IMAGE_H
#define PONG_IMAGE_H

#include <cstdint>

struct Image {

	Image(std::uint32_t width, std::uint32_t height, std::uint8_t* data) :
		width(width),
		height(height),
		data(data) {}

    std::uint32_t width;
    std::uint32_t height;
    std::uint8_t* data;
};

#endif // PONG_IMAGE_H
