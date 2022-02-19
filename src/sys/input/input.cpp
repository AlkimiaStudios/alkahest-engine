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

    float Input::getAxisImpl(Axis axis)
    {
        // TODO: axis polling impl
        return 0.0f;
    }

    void Input::setKeyStateImpl(Key keycode, KeyState state)
    {
        std::stringstream ss;
        ss << "Setting key [" << keycode << "] to state [" << state << "]";
        logTrace(ss.str());
        m_keys.insert_or_assign(keycode, state);
        if (m_keys.find(keycode)->second != state)
            logError("Keystate mismatch");
    }

    void Input::setMouseButtonStateImpl(MouseButton button, ButtonState state)
    {
        m_buttons.insert_or_assign(button, state);
    }
}