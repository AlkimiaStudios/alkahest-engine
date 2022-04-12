#pragma once

#include "../../macros.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    // Forward declarations
    enum InputMode : uint32_t;
    enum KeyState : uint8_t;
    enum ButtonState : uint8_t;
    enum Axis : uint8_t;
    enum MouseButton : uint8_t;
    enum Key : int16_t;

    class API Input
    {
    protected:
        static Input* getInstance(void *window = nullptr);
    public:
        static void initialize(void *window);

        Input() {};
        virtual ~Input() {};

        // Keyboard
        static bool isKeyDown(Key keycode) { return getInstance()->isKeyDownImpl(keycode); };
        static bool isKeyUp(Key keycode) { return getInstance()->isKeyUpImpl(keycode); };
        static bool isKeyHeld(Key keycode) { return getInstance()->isKeyHeldImpl(keycode); };
        static KeyState getKeyState(Key keycode) { return getInstance()->getKeyState(keycode); };

        // Mouse
        static bool isMouseButtonDown(MouseButton button) { return getInstance()->isMouseButtonDownImpl(button); };
        static bool isMouseButtonUp(MouseButton button) { return getInstance()->isMouseButtonUpImpl(button); };
        static bool isMouseButtonHeld(MouseButton button) { return getInstance()->isMouseButtonHeldImpl(button); };
        static ButtonState getMouseButtonState(MouseButton button) { return getInstance()->getMouseButtonStateImpl(button); };

        static glm::vec2 getMousePos() { return getInstance()->getMousePosImpl(); };
        static double getMouseX() { return getInstance()->getMousePosImpl().x; };
        static double getMouseY() { return getInstance()->getMousePosImpl().y; };

        static glm::vec2 getMouseScroll() { return getInstance()->getMouseScrollImpl(); };
        static double getMouseScrollX() { return getInstance()->getMouseScrollImpl().x; };
        static double getMouseScrollY() { return getInstance()->getMouseScrollImpl().y; };

        // Joystick Axis
        static float getAxis(Axis axis) { return getInstance()->getAxisImpl(axis); };

        // Controller support
        // TODO: add controller support
    protected:
        virtual bool isKeyDownImpl(Key keycode) = 0;
        virtual bool isKeyUpImpl(Key keycode) = 0;
        virtual bool isKeyHeldImpl(Key keycode) = 0;
        virtual KeyState getKeyStateImpl(Key keycode) = 0;

        virtual bool isMouseButtonDownImpl(MouseButton button) = 0;
        virtual bool isMouseButtonUpImpl(MouseButton button) = 0;
        virtual bool isMouseButtonHeldImpl(MouseButton button) = 0;
        virtual ButtonState getMouseButtonStateImpl(MouseButton button) = 0;

        virtual glm::vec2 getMousePosImpl() = 0;
        virtual glm::vec2 getMouseScrollImpl() = 0;

        virtual float getAxisImpl(Axis axis) = 0;

        // TODO: controller support impl
    public:
        static void setKeyState(Key keycode, KeyState state) { getInstance()->setKeyStateImpl(keycode, state); };
        static void setMouseButtonState(MouseButton button, ButtonState state) { getInstance()->setMouseButtonStateImpl(button, state); };
        static void setMousePos(double x, double y, bool propagate = false) { getInstance()->setMousePosImpl(x, y, propagate); };
        static void setMouseScroll(double x, double y) { getInstance()->setMouseScrollImpl(x, y); };
        // TODO: set joystick/controller button state
    protected:
        virtual void setKeyStateImpl(Key keycode, KeyState state) = 0;
        virtual void setMouseButtonStateImpl(MouseButton button, ButtonState state) = 0;
        virtual void setMousePosImpl(double x, double y, bool propagate) = 0;
        virtual void setMouseScrollImpl(double x, double y) = 0;
        // TODO: set joystick/controller button state impl
    protected:
        static bool m_initialized;
        static Input *m_pInstance;
        static void *m_window;
    };
}