#pragma once

#include "../macros.h"
#include "../sys/window/window.h"
#include "../sys/events/event.h"
#include "../sys/events/eventqueue.h"

// Much of the early systems developed for AlkahestEngine were developed following
// along with the Game Engine series from The Cherno (Yan Chernikov) as he built
// the Hazel game engine. A future refactor of these systems is planned to
// eventually re-work this code to adapt it more closely to the direction the
// AlkahestEngine is heading. In the meantime, however, thank you to Yan for
// providing one of the most educational channels on YT when it comes to C++
// and OpenGL development.

namespace Alkahest
{
    class API Application
    {
    public:
        Application();
        virtual ~Application();
        void init();
        virtual void run();
        virtual void update() {};
        void stop() { m_shouldStop = true; };
    protected:
        std::unique_ptr<IWindow> m_window;
        std::unique_ptr<EventDispatcher> m_dispatcher;
        bool m_shouldStop = false;

        using super = Application;
    private:
        std::thread m_eventThread;
        static void onEvent(Event* e);
    };

    Application* CreateApplication();
}