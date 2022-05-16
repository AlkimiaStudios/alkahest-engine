#pragma once

#include "_base.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    namespace Components
    {
        struct TransformComponent : Component
        {
            glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        };
    }
}
