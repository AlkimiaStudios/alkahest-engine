#include "input.h"
#include "../log/log.h"

namespace Alkahest
{
    Input* Input::m_pInstance = nullptr;

    Input* Input::getInstance()
    {
        if (m_pInstance == nullptr)
            m_pInstance = new Input();
        return m_pInstance;
    }

    Input::Input()
    {
    }

    Input::~Input()
    {
    }

    bool Input::isKeyDownImpl(Key keycode)
    {
        bool down = getKeyStateImpl(keycode) == KeyState::KeyDown;
        
        // If the key is down, we set it back to up to avoid accidentally
        // registering multiple keypresses
        if (down)
            setKeyStateImpl(keycode, KeyState::KeyUp);

        // ...and then we return the initial value;
        return down;
    }

    bool Input::isKeyUpImpl(Key keycode)
    {
        return getKeyStateImpl(keycode) == KeyState::KeyUp;
    }

    bool Input::isKeyHeldImpl(Key keycode)
    {
        return getKeyStateImpl(keycode) == KeyState::KeyHeld;
    }

    KeyState Input::getKeyStateImpl(Key keycode)
    {
        std::unordered_map<Alkahest::Key, Alkahest::KeyState>::iterator state = m_keys.find(keycode);
        if (state == m_keys.end())
            return KeyState::KeyUp;
        return state->second;
    }

    bool Input::isMouseButtonDownImpl(MouseButton button)
    {
        bool down = getMouseButtonStateImpl(button) == ButtonState::ButtonDown;

        // If the key is down, we set it back to up to avoid accidentally
        // registering multiple keypresses
        if (down)
            setMouseButtonStateImpl(button, ButtonState::ButtonUp);

        // ...and then we return the initial value;
        return down;
    }

    bool Input::isMouseButtonUpImpl(MouseButton button)
    {
        return getMouseButtonStateImpl(button) == ButtonState::ButtonUp;
    }

    bool Input::isMouseButtonHeldImpl(MouseButton button)
    {
        return getMouseButtonStateImpl(button) == ButtonState::ButtonHeld;
    }

    ButtonState Input::getMouseButtonStateImpl(MouseButton button)
    {
        auto state = m_buttons.find(button);
        return state == m_buttons.end() ? ButtonState::ButtonUp : state->second;
    }

    glm::vec2 Input::getMousePosImpl()
    {
        return { m_mouseX, m_mouseY };
    }

    glm::vec2 Input::getMouseScrollImpl()
    {
        return { m_scrollX, m_scrollY };
    }

    float Input::getAxisImpl(Axis axis)
    {
        // TODO: axis polling impl
        return 0.0f;
    }

    void Input::setKeyStateImpl(Key keycode, KeyState state)
    {
        logTrace("Setting key [{}] to state [{}]", keycode, state);
        m_keys.insert_or_assign(keycode, state);
        if (m_keys.find(keycode)->second != state)
            logError("Keystate mismatch");
    }

    void Input::setMouseButtonStateImpl(MouseButton button, ButtonState state)
    {
        logTrace("Setting button [{}] to state [{}]", button, state);
        m_buttons.insert_or_assign(button, state);
        if (m_buttons.find(button)->second != state)
            logError("Buttonstate mismatch");
    }

    void Input::setMousePosImpl(double x, double y)
    {
        m_mouseX = x;
        m_mouseY = y;
        logTrace("Setting mouse position to ({}, {})", x, y);
    }

    void Input::setMouseScrollImpl(double x, double y)
    {
        m_scrollX = x;
        m_scrollY = y;
        logTrace("Setting mouse scroll to ({}, {})", x, y);
    }
}