#pragma once

#include "../../macros.h"
#include "input.h"
#include "openglkeys.h"

#include <GLFW/glfw3.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLInput : public Input
    {
    public:
        OpenGLInput(GLFWwindow *window) { m_window = window; };
        virtual ~OpenGLInput() {};
    public:
        virtual bool isKeyDownImpl(Key keycode) override;
        virtual bool isKeyUpImpl(Key keycode) override;
        virtual bool isKeyHeldImpl(Key keycode) override;
        virtual KeyState getKeyStateImpl(Key keycode) override;

        virtual bool isMouseButtonDownImpl(MouseButton button) override;
        virtual bool isMouseButtonUpImpl(MouseButton button) override;
        virtual bool isMouseButtonHeldImpl(MouseButton button) override;
        virtual ButtonState getMouseButtonStateImpl(MouseButton button) override;

        virtual glm::vec2 getMousePosImpl() override;
        virtual glm::vec2 getMouseScrollImpl() override;

        virtual float getAxisImpl(Axis axis) override;

        // TODO: controller support impl
    public:
        virtual void setKeyStateImpl(Key keycode, KeyState state) override;
        virtual void setMouseButtonStateImpl(MouseButton button, ButtonState state) override;
        virtual void setMousePosImpl(double x, double y, bool propagate) override;
        virtual void setMouseScrollImpl(double x, double y) override;
        // TODO: set joystick/controller button state impl
    private:
        std::unordered_map<Key, KeyState> m_keys;
        std::unordered_map<MouseButton, ButtonState> m_buttons;
        double m_mouseX, m_mouseY, m_scrollX, m_scrollY;
    };
}