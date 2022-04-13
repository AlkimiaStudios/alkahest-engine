#pragma once

#include "../../macros.h"
#include "shader.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    class API Camera
    {
    public:
        Camera() {};
        virtual ~Camera() {};

        virtual glm::vec3 getPosition() = 0;
        virtual glm::vec3 getOrientation() = 0;
        virtual glm::vec3 getUpVector() = 0;

        virtual void setPosition(glm::vec3 position) = 0;
        virtual void setOrientation(glm::vec3 orientation) = 0;
        virtual void setUpVector(glm::vec3 up) = 0;

        virtual float getSpeed() = 0;
        virtual float getSensitivity() = 0;

        virtual void setSpeed(float speed) = 0;
        virtual void setSensitivity(float sens) = 0;

        virtual glm::mat4 getViewMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) = 0;
        virtual void updateMatrixInShader(glm::mat4 camMatrix, Ref<Shader> shader, const char* uniform) = 0;

        static Ref<Camera> create(glm::vec3 position);
    };
}