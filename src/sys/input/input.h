#pragma once

#include "../../macros.h"
#include "keys.h"

namespace Alkahest
{
    class API Input
    {
    private:
        static Input *getInstance();
    public:
        friend class OpenGLWindow;

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

        // Mouse/Joystick Axis
        static float getAxis(Axis axis) { return getInstance()->getAxisImpl(axis); };

        // Controller support
        // TODO: add controller support
    private:
        bool isKeyDownImpl(Key keycode);
        bool isKeyUpImpl(Key keycode);
        bool isKeyHeldImpl(Key keycode);
        KeyState getKeyStateImpl(Key keycode);

        bool isMouseButtonDownImpl(MouseButton button);
        bool isMouseButtonUpImpl(MouseButton button);
        bool isMouseButtonHeldImpl(MouseButton button);
        ButtonState getMouseButtonStateImpl(MouseButton button);

        float getAxisImpl(Axis axis);

        // TODO: controller support impl
    private:
        static void setKeyState(Key keycode, KeyState state) { getInstance()->setKeyStateImpl(keycode, state); };
        static void setMouseButtonState(MouseButton button, ButtonState state) { getInstance()->setMouseButtonStateImpl(button, state); };
        // TODO: set joystick/controller button state
    private:
        void setKeyStateImpl(Key keycode, KeyState state);
        void setMouseButtonStateImpl(MouseButton button, ButtonState state);
        // TODO: set joystick/controller button state impl
    private:
        Input();
        ~Input();
    private:
        static Input *m_pInstance;
        std::unordered_map<Key, KeyState> m_keys;
        std::unordered_map<MouseButton, ButtonState> m_buttons;
    };
}