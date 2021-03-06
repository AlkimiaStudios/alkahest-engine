#pragma once

#include "../../macros.h"

// Much of the early systems developed for AlkahestEngine were developed following
// along with the Game Engine series from The Cherno (Yan Chernikov) as he built
// the Hazel game engine. A future refactor of these systems is planned to
// eventually re-work this code to adapt it more closely to the direction the
// AlkahestEngine is heading. In the meantime, however, thank you to Yan for
// providing one of the most educational channels on YT when it comes to C++
// and OpenGL development.
//
// In compliance with the Hazel license, a copy of said license is available at
// {project_root}/.licenses/Hazel.LICENSE
//
// The code within this file has been modified from the original source, and
// this notice is provided in compliance with the above license terms.

namespace Alkahest
{
    enum EventCategory {
        None        = 0,
        Window      = BIT(0),
        Input       = BIT(1),
        Keyboard    = BIT(2),
        Mouse       = BIT(3),
    };

    enum class EventType {
        None = 0,
        WindowResize, WindowClose,
        KeyDown, KeyUp,
        MouseButtonDown, MouseButtonUp, MouseMove, MouseScroll 
    };

    // Macros to easily set up new Event types
    #define SETUP_EVENT_TYPE(type) static EventType getStaticType() { return EventType::type; };\
                                    virtual EventType getEventType() const override { return getStaticType(); }\
                                    virtual const char *getName() const override { return #type; }
    #define SETUP_EVENT_FLAGS(flags) virtual int getCategoryFlags() const override { return flags; }

    class API Event
    {
    public:
        Event() {};
        virtual ~Event() {};
        virtual EventType getEventType() const = 0;
        virtual int getCategoryFlags() const = 0;
        virtual const char *getName() const = 0;
        virtual std::string toString() const = 0;
    protected:
        bool m_Handled = false;
    };

    class NOT_EXPORTED NullEvent : public Event
    {
    public:
        SETUP_EVENT_TYPE(None);
        SETUP_EVENT_FLAGS(None);
        std::string toString() const override { return "NullEvent"; };
    };
}