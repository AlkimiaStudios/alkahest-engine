#include "openglinput.h"
#include "../log/log.h"

namespace Alkahest
{
    bool Input::m_initialized = false;
    Input* Input::m_pInstance = nullptr;
    void* Input::m_window = nullptr;

    Input* Input::getInstance(void *window)
    {
        // If we have not been initiated yet and we weren't given a window,
        // we return a dummy that will be destroyed
        if (!m_initialized)
        {
            if (window != nullptr)
            {
                if (m_pInstance != nullptr)
                    delete m_pInstance;
                
                m_pInstance = new OpenGLInput(reinterpret_cast<GLFWwindow*>(window));

                m_initialized = true;
            }
            else
            {
                if (m_pInstance == nullptr)
                    m_pInstance = new OpenGLInput(static_cast<GLFWwindow*>(0));
            }
        }
        else
        {
            if (m_pInstance == nullptr)
            {
                logError("Input has been initialized but instance is nullptr!");
                m_pInstance = new OpenGLInput(static_cast<GLFWwindow*>(0));
            }
        }


        return m_pInstance;
    }

    void Input::initialize(void *window)
    {
        getInstance(window);
    }

    bool OpenGLInput::isKeyDownImpl(Key keycode)
    {
        // TODO: figure out how to not register multiple presses
        // without setting the state back to KeyUp...
        /*
        bool down = getKeyStateImpl(keycode) == KeyState::KeyDown;
        
        // If the key is down, we set it back to up to avoid accidentally
        // registering multiple keypresses
        if (down)
            setKeyStateImpl(keycode, KeyState::KeyUp);

        // ...and then we return the initial value;
        return down;
        */

       return getKeyStateImpl(keycode) == KeyState::KeyDown;
    }

    bool OpenGLInput::isKeyUpImpl(Key keycode)
    {
        return getKeyStateImpl(keycode) == KeyState::KeyUp;
    }

    bool OpenGLInput::isKeyHeldImpl(Key keycode)
    {
        return getKeyStateImpl(keycode) == KeyState::KeyHeld || getKeyStateImpl(keycode) == KeyState::KeyDown;
    }

    KeyState OpenGLInput::getKeyStateImpl(Key keycode)
    {
        std::unordered_map<Alkahest::Key, Alkahest::KeyState>::iterator state = m_keys.find(keycode);
        if (state == m_keys.end())
            return KeyState::KeyUp;
        return state->second;
    }

    bool OpenGLInput::isMouseButtonDownImpl(MouseButton button)
    {
        // TODO: figure out how to not register multiple presses
        // without setting the state back to KeyUp...
        /*
        bool down = getMouseButtonStateImpl(button) == ButtonState::ButtonDown;

        // If the key is down, we set it back to up to avoid accidentally
        // registering multiple keypresses
        if (down)
            setMouseButtonStateImpl(button, ButtonState::ButtonUp);

        // ...and then we return the initial value;
        return down;
        */

       return getMouseButtonStateImpl(button) == ButtonState::ButtonDown;
    }

    bool OpenGLInput::isMouseButtonUpImpl(MouseButton button)
    {
        return getMouseButtonStateImpl(button) == ButtonState::ButtonUp;
    }

    bool OpenGLInput::isMouseButtonHeldImpl(MouseButton button)
    {
        return getMouseButtonStateImpl(button) == ButtonState::ButtonHeld || getMouseButtonStateImpl(button) == ButtonState::ButtonDown;
    }

    ButtonState OpenGLInput::getMouseButtonStateImpl(MouseButton button)
    {
        auto state = m_buttons.find(button);
        return state == m_buttons.end() ? ButtonState::ButtonUp : state->second;
    }

    glm::vec2 OpenGLInput::getMousePosImpl()
    {
        return { m_mouseX, m_mouseY };
    }

    glm::vec2 OpenGLInput::getMouseScrollImpl()
    {
        return { m_scrollX, m_scrollY };
    }

    float OpenGLInput::getAxisImpl(Axis axis)
    {
        // TODO: axis polling impl
        return 0.0f;
    }

    void OpenGLInput::setKeyStateImpl(Key keycode, KeyState state)
    {
        logTrace("Setting key [{}] to state [{}]", keycode, state);
        m_keys.insert_or_assign(keycode, state);
        if (m_keys.find(keycode)->second != state)
            logError("Keystate mismatch");
    }

    void OpenGLInput::setMouseButtonStateImpl(MouseButton button, ButtonState state)
    {
        logTrace("Setting button [{}] to state [{}]", button, state);
        m_buttons.insert_or_assign(button, state);
        if (m_buttons.find(button)->second != state)
            logError("Buttonstate mismatch");
    }

    void OpenGLInput::setMousePosImpl(double x, double y, bool propagate)
    {
        m_mouseX = x;
        m_mouseY = y;
        logTrace("Setting mouse position to ({}, {})", x, y);

        if (propagate)
            glfwSetCursorPos(reinterpret_cast<GLFWwindow*>(m_window), x, y);
    }

    void OpenGLInput::setMouseScrollImpl(double x, double y)
    {
        m_scrollX = x;
        m_scrollY = y;
        logTrace("Setting mouse scroll to ({}, {})", x, y);
    }
}