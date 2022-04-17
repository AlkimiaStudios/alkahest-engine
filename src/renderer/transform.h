#pragma once

#include "../macros.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Alkahest
{
    class API Transform
    {
    public:
        Transform() {};
        ~Transform() {};

        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::mat4 getMatrix()
        {
            // Translation is easy
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);

            // Building a quaternion from the assumption that a value of 1 = 360 degrees of rotation
            glm::vec3 euler = glm::vec3(glm::radians(Rotation.x * 360.0f), glm::radians(Rotation.y * 360.0f), glm::radians(Rotation.z * 360.0f));
            glm::quat qRotation = glm::quat(euler);
            glm::mat4 rotation = glm::mat4_cast(qRotation);

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);

            return translation * rotation * scale;
        }
    };
}