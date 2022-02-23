#include "application.h"

#include "../sys/log/log.h"

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
    void eventSetup(Application *app)
    {
        EventDispatcher *d = EventDispatcher::getInstance();
        
        // Trace log of mouse events
        d->registerCallback(EventType::MouseScroll, [](Event* e){ logTrace(e->toString()); });
        d->registerCallback(EventType::MouseMove, [](Event* e){ logTrace(e->toString()); });
        d->registerCallback(EventType::MouseButtonDown, [](Event* e){ logTrace(e->toString()); });
        d->registerCallback(EventType::MouseButtonUp, [](Event* e){ logTrace(e->toString()); });

        // Trace log of keyboard events
        d->registerCallback(EventType::KeyDown, [](Event* e){ logTrace(e->toString()); });
        d->registerCallback(EventType::KeyUp, [](Event* e){ logTrace(e->toString()); });

        // Trace log of window events
        d->registerCallback(EventType::WindowResize, [](Event* e){ logTrace(e->toString()); });
        d->registerCallback(EventType::WindowClose, [](Event* e){ logTrace(e->toString()); });

        // Handle window close
        d->registerCallback(EventType::WindowClose, [app](Event* e){ app->stop(); });
    }

    Application::Application()
    {
        init();
    }

    Application::~Application() {
        m_dispatcher->stop();
        m_eventThread.join();
    }

    // Not overridden, use to instantiate engine subsystems
    void Application::init()
    {
        logInfo("Alkahest booting up...");

        logTrace("Creating event dispatcher");
        m_dispatcher = std::unique_ptr<EventDispatcher>(EventDispatcher::getInstance());
        eventSetup(this);
        m_eventThread = std::thread([this]{ this->m_dispatcher->run(); });

        logTrace("Creating initial window...");
        m_window = std::unique_ptr<IWindow>(IWindow::create());
        m_window->setEventCallback(Application::onEvent);
    }

    // To be overridden in client, define custom systems, etc
    void Application::run()
    {
        while (!m_shouldStop)
        {
            m_window->onUpdate();
            update();
        }
    }

    void Application::onEvent(Event* e)
    {
        EventQueue* eq = EventQueue::getInstance();
        eq->push(e);
    };
}