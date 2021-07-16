#ifndef PONG_HELPER_H
#define PONG_HELPER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 createTranslation(glm::vec2 translation) {
    return glm::translate(glm::mat4(1.0), glm::vec3(translation, 0.0));
}

#endif // PONG_HELPER_H
