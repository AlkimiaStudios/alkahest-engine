#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Alkahest
{
    Ref<Camera> Camera::create(glm::vec3 position)
    {
        return CreateRef<OpenGLCamera>(position);
    }

    OpenGLCamera::OpenGLCamera(glm::vec3 position)
    {
        m_pos = position;
    }

    glm::vec3 OpenGLCamera::getPosition()
    {
        return m_pos;
    }

    glm::vec3 OpenGLCamera::getOrientation()
    {
        return m_ori;
    }

    glm::vec3 OpenGLCamera::getUpVector()
    {
        return m_up;
    }

    void OpenGLCamera::setPosition(glm::vec3 position)
    {
        m_pos = position;
    }

    void OpenGLCamera::setOrientation(glm::vec3 orientation)
    {
        m_ori = orientation;
    }

    void OpenGLCamera::setUpVector(glm::vec3 up)
    {
        m_up = up;
    }

    float OpenGLCamera::getSpeed()
    {
        return m_speed;
    }

    float OpenGLCamera::getSensitivity()
    {
        return m_sens;
    }

    void OpenGLCamera::setSpeed(float speed)
    {
        m_speed = speed;
    }

    void OpenGLCamera::setSensitivity(float sens)
    {
        m_sens = sens;
    }

    glm::mat4 OpenGLCamera::getViewMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = glm::lookAt(m_pos, m_pos + m_ori, m_up);

        projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

        return projection * view;
    }
}