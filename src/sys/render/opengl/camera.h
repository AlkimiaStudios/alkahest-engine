#pragma once

#include "../../../macros.h"
#include "../camera.h"

namespace Alkahest
{
    class NOT_EXPORTED OpenGLCamera : public Camera
    {
    public:
        OpenGLCamera(glm::vec3 position);
        virtual ~OpenGLCamera() {};

        glm::vec3 getPosition() override;
        glm::vec3 getOrientation() override;
        glm::vec3 getUpVector() override;

        void setPosition(glm::vec3 position) override;
        void setOrientation(glm::vec3 orientation) override;
        void setUpVector(glm::vec3 up) override;

        float getSpeed() override;
        float getSensitivity() override;

        void setSpeed(float speed) override;
        void setSensitivity(float sens) override;

        glm::mat4 getViewMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) override;
    private:
        glm::vec3 m_pos;
        glm::vec3 m_ori = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

        float m_speed = 0.1f;
        float m_sens = 1.0f;
    };
}