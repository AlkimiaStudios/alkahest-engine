#pragma once

#include "_base.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    namespace Components
    {
        struct TransformComponent : Component
        {
            glm::vec3 Position;
            glm::vec3 Rotation;
            glm::vec3 Scale;
        };
    }
}
