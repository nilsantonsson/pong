#include "Helper.h"
#include "Effect.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Window.h"
#include "Randomizer.h"
#include "Gui.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>

struct Body {
    Body(glm::vec2 position, float width, float height) :
        position(position), width(width), height(height) {}

    glm::vec2 position;
    float width;
    float height;
};

struct Obstacle {
    Obstacle(std::shared_ptr<Body> body, glm::vec2 normal) : body(body), normal(normal) {}

    std::shared_ptr<Body> body;
    glm::vec2 normal;
};

struct Paddle : public Obstacle {
    Paddle(std::shared_ptr<Body> body, glm::vec2 normal, std::shared_ptr<Mesh> mesh) :
        Obstacle(body, normal), mesh(mesh) {}

    std::shared_ptr<Mesh> mesh;
};

struct Ball {
    Ball(std::shared_ptr<Body> body, std::shared_ptr<Mesh> mesh) : body(body), mesh(mesh) {}

    glm::vec2 direction;
    std::shared_ptr<Body> body;
    std::shared_ptr<Mesh> mesh;
};

enum class PaddleAiState {
    Idle,
    GoingToIdle,
    CatchingBall
};

const std::uint32_t WINDOW_WIDTH = 1280;
const std::uint32_t WINDOW_HEIGHT = 720;
const float SIGHT_DISTANCE = 350.0;
const float SURFACE_DISTANCE = 4.0;
const float PADDLE_SPEED = 300.0;
const float BALL_SPEED = 400.0;
const float LIMIT_X = 600.0;
const float LIMIT_Y = 300.0;
const float DISTANCE_TO_IDLE_Y = 30.0;
const float BALL_PADDLE_DIFF_Y = 30.0;
const float PADDLE_HEIGHT = 50.0;

std::shared_ptr<Window> window;
std::shared_ptr<Renderer> renderer;
std::shared_ptr<Effect> orthoEffect;
std::uint8_t whitePixel = 255;
std::shared_ptr<Texture> whiteTexture;

std::shared_ptr<Obstacle> topWall;
std::shared_ptr<Obstacle> bottomWall;
std::shared_ptr<Paddle> paddleLeft;
std::shared_ptr<Paddle> paddleRight;
std::vector<std::shared_ptr<Obstacle>> obstacles;
std::shared_ptr<Ball> ball;

bool movingUp = false;
bool movingDown = false;
std::uint8_t pointsLeft = 0;
std::uint8_t pointsRight = 0;

Randomizer randomizer;
PaddleAiState paddleAiState = PaddleAiState::Idle;
auto currentTime = std::chrono::high_resolution_clock::now();

std::shared_ptr<Gui> gui;

void keyCallback(GLFWwindow* glfwWindow, int key, int scanCode, int action, int mods) {

    if (action == GLFW_PRESS) {

        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            movingUp = true;
        }

        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            movingDown = true;
        }

        if (key == GLFW_KEY_ESCAPE) {
            window->setShouldClose();
        }
    }

    if (action == GLFW_RELEASE) {

        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            movingUp = false;
        }

        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            movingDown = false;
        }
    }
}

void createWalls() {
    topWall = std::make_shared<Obstacle>(
        std::make_shared<Body>(glm::vec2(0.0, 340.0), 1280.0f, 20.0f),
        glm::vec2(0.0, -1.0));
    obstacles.push_back(topWall);

    bottomWall = std::make_shared<Obstacle>(
        std::make_shared<Body>(glm::vec2(0.0, -340.0), 1280.0f, 20.0f),
        glm::vec2(0.0, 1.0));
    obstacles.push_back(bottomWall);
}

void createWhiteTexture() {
    auto image = std::make_shared<Image>(1, 1, &whitePixel);
    whiteTexture = std::make_shared<Texture>(image);
    renderer->addTexture(whiteTexture);
}

void createBall() {
    auto ballMesh = buildQuadMesh(10.0, 10.0, orthoEffect);
    ballMesh->texture = whiteTexture;
    ballMesh->transform = createTranslation(glm::vec2(0.0, 0.0));
    renderer->addMesh(ballMesh);

    ball = std::make_shared<Ball>(
        std::make_shared<Body>(glm::vec2(0.0, 0.0), 10.0f, 10.0f),
        ballMesh);
    ball->direction = randomizer.randomDirection();
}

void createPaddles() {
    auto paddleLeftMesh = buildQuadMesh(20.0, 50.0, orthoEffect);
    paddleLeftMesh->texture = whiteTexture;
    paddleLeftMesh->transform = createTranslation(glm::vec2(-500.0, 0.0));
    renderer->addMesh(paddleLeftMesh);

    paddleLeft = std::make_shared<Paddle>(
        std::make_shared<Body>(glm::vec2(-500.0, 0.0), 20.0f, PADDLE_HEIGHT),
        glm::vec2(1.0, 0.0),
        paddleLeftMesh);
    obstacles.push_back(paddleLeft);

    auto paddleRightMesh = buildQuadMesh(20.0, 50.0, orthoEffect);
    paddleRightMesh->texture = whiteTexture;
    paddleRightMesh->transform = createTranslation(glm::vec2(500.0, 0.0));
    renderer->addMesh(paddleRightMesh);

    paddleRight = std::make_shared<Paddle>(
        std::make_shared<Body>(glm::vec2(500.0, 0.0), 20.0f, 50.0f),
        glm::vec2(-1.0, 0.0),
        paddleRightMesh);
    obstacles.push_back(paddleRight);
}

void setupGame() {

    renderer = std::make_shared<Renderer>(WINDOW_WIDTH, WINDOW_HEIGHT);

    orthoEffect = buildOrthoEffect();
    renderer->addEffect(orthoEffect);

    createWhiteTexture();
    createWalls();
    createBall();
    createPaddles();

    gui = std::make_shared<Gui>(renderer, orthoEffect);

    renderer->prepare();
}

bool overlaps(std::shared_ptr<Body> bodyA, std::shared_ptr<Body> bodyB) {
    glm::vec2 t0 = bodyA->position;
    glm::vec2 aabbMin0 = t0 - glm::vec2(bodyA->width*0.5, bodyA->height*0.5);
    glm::vec2 aabbMax0 = t0 + glm::vec2(bodyA->width*0.5, bodyA->height*0.5);

    glm::vec2 t1 = bodyB->position;
    glm::vec2 aabbMin1 = t1 - glm::vec2(bodyB->width*0.5, bodyB->height*0.5);
    glm::vec2 aabbMax1 = t1 + glm::vec2(bodyB->width*0.5, bodyB->height*0.5);

    return (aabbMin0.x <= aabbMax1.x && aabbMax0.x >= aabbMin1.x) &&
        (aabbMin0.y <= aabbMax1.y && aabbMax0.y >= aabbMin1.y);
}

void updateBall(double frameTime) {

    for (auto obstacle : obstacles) {
        if (overlaps(ball->body, obstacle->body)) {
            ball->body->position += (obstacle->normal * SURFACE_DISTANCE);
            auto normal = obstacle->normal;
            if (obstacle == paddleLeft || obstacle == paddleRight) {
                float diffY = ball->body->position.y - obstacle->body->position.y;
                float pctY = diffY / (PADDLE_HEIGHT * 0.5);
                normal = glm::normalize(glm::vec2(obstacle->normal.x, pctY * 0.1));
            }
            ball->direction = glm::reflect(ball->direction, normal);
        }
    }

    ball->body->position += (ball->direction * BALL_SPEED * static_cast<float>(frameTime));
    bool missedLeft = ball->body->position.x < -LIMIT_X;
    bool missedRight = ball->body->position.x > LIMIT_X;
    
    if (missedLeft || missedRight) {
        missedLeft ? pointsRight++ : pointsLeft++;

        if (pointsLeft > 9 || pointsRight > 9) {
            pointsLeft = 0;
            pointsRight = 0;
        }

        ball->body->position = glm::vec2();
        ball->direction = randomizer.randomDirection();
    }

    ball->mesh->transform = createTranslation(ball->body->position);
}

void updatePaddle(std::shared_ptr<Paddle> paddle, glm::vec2 velocity) {
    glm::vec2 newPosition = paddle->body->position + velocity;
    glm::vec2 cappedPosition = glm::vec2(
        newPosition.x,
        std::min(LIMIT_Y, std::max(-LIMIT_Y, newPosition.y)));
    paddle->body->position = cappedPosition;
    paddle->mesh->transform = createTranslation(cappedPosition);
}

void updateLeftPaddle(double frameTime) {
    float velocityY = 0.0;
    if (movingUp) {
        velocityY += PADDLE_SPEED * frameTime;
    }
    if (movingDown) {
        velocityY -= PADDLE_SPEED * frameTime;
    }

    updatePaddle(paddleLeft, glm::vec2(0.0, velocityY));
}

bool ballIsInSight() {
    return std::abs(ball->body->position.x - paddleRight->body->position.x) < SIGHT_DISTANCE;
}

void updateRightPaddle(double frameTime) {

    float velocityY = 0.0;

    switch (paddleAiState) {

        case PaddleAiState::Idle :
            if (ballIsInSight() && ball->direction.x > 0.0) {
                paddleAiState = PaddleAiState::CatchingBall;
            }
            break;

        case PaddleAiState::CatchingBall :
            if (ballIsInSight() && ball->direction.x > 0.0) {
                if (ball->body->position.y - paddleRight->body->position.y > BALL_PADDLE_DIFF_Y) {
                    velocityY = PADDLE_SPEED * frameTime;
                } else if (ball->body->position.y - paddleRight->body->position.y < -BALL_PADDLE_DIFF_Y) {
                    velocityY = -PADDLE_SPEED * frameTime;
                }

            } else {
                paddleAiState = PaddleAiState::GoingToIdle;
            }
            break;

        case PaddleAiState::GoingToIdle :
            if (paddleRight->body->position.y > DISTANCE_TO_IDLE_Y) {
                velocityY = -PADDLE_SPEED * frameTime;

            } else if (paddleRight->body->position.y < -DISTANCE_TO_IDLE_Y) {
                velocityY = PADDLE_SPEED * frameTime;

            } else {
                paddleAiState = PaddleAiState::Idle;
            }
            break;

        default:
            break;
    }

    updatePaddle(paddleRight, glm::vec2(0.0, velocityY));
}

void updateGame(double frameTime) {

    updateBall(frameTime);
    updateLeftPaddle(frameTime);
    updateRightPaddle(frameTime);

    gui->update(pointsLeft, pointsRight);

    renderer->render();
}

int main() {

    window = std::make_shared<Window>(WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetKeyCallback(window->getGlfwWindow(), keyCallback);

    setupGame();

    while (!window->shouldClose()) {

        auto newTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameTime = newTime - currentTime;
        currentTime = newTime;
        updateGame(frameTime.count());

        window->update();
    }

    return 0;
}
