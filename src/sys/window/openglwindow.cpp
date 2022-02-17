#include "openglwindow.h"
#include "../log/log.h"
#include "../events/windowevent.h"
#include "../events/keyevent.h"
#include "../events/mouseevent.h"

namespace Alkahest
{
    static bool s_glfwInit = false;

    static void handleGLFWError(int e, const char *msg)
    {
        logError("GLFW Error (Code " + std::to_string(e) + "): " + msg);
    }

    IWindow* IWindow::create(const WindowProps& props)
    {
        return new OpenGLWindow(props);
    }
    
    OpenGLWindow::OpenGLWindow(const WindowProps& props)
    {
        init(props);
    }

    OpenGLWindow::~OpenGLWindow()
    {
        shutdown();
    }

    void OpenGLWindow::onUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void OpenGLWindow::setVSync(bool vsync)
    {
        if (vsync)
        {
            glfwSwapInterval(1);
        }
        else
        {
            glfwSwapInterval(0);
        }

        m_data.vsync = vsync;
    }

    bool OpenGLWindow::isVSync() const
    {
        return m_data.vsync;
    }

    void OpenGLWindow::init(const WindowProps& props)
    {
        if (!s_glfwInit)
        {
            if (!glfwInit())
            {
                logError("GLFW failed to initialize!");
                const char* message;
                glfwGetError(&message);
                logError(message);
                exit(1);
            }

            if (!gl3wInit())
            {
                logError("GL3W failed to initialize!");
                exit(1);
            }

            glfwSetErrorCallback(handleGLFWError);
            s_glfwInit = true;
        }

        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;

        logTrace("Creating window " + m_data.title + ": (" + std::to_string(m_data.width) + "x" + std::to_string(m_data.height) + ")");

        glfwWindowHintString(GLFW_X11_CLASS_NAME, "alkahest");
        glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "alkahest");

        m_window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, &m_data);
        setVSync(true);

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.width = width;
            data.height = height;

            WindowResizeEvent* e = new WindowResizeEvent(width, height);
            data.eventCallback(e);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent* e = new WindowCloseEvent();
            data.eventCallback(e);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            switch(action)
            {
                case GLFW_PRESS:
                {
                    KeyDownEvent* e = new KeyDownEvent(key, false);
                    data.eventCallback(e);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyDownEvent* e = new KeyDownEvent(key, true);
                    data.eventCallback(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyUpEvent* e = new KeyUpEvent(key);
                    data.eventCallback(e);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            switch(action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonDownEvent* e = new MouseButtonDownEvent(button, false);
                    data.eventCallback(e);
                    break;
                }
                case GLFW_REPEAT:
                {
                    MouseButtonDownEvent* e = new MouseButtonDownEvent(button, true);
                    data.eventCallback(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonUpEvent* e = new MouseButtonUpEvent(button);
                    data.eventCallback(e);
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            MouseMoveEvent* e = new MouseMoveEvent(x, y);
            data.eventCallback(e);
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y){
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            MouseScrollEvent* e = new MouseScrollEvent(x, y);
            data.eventCallback(e);
        });
    }

    void OpenGLWindow::shutdown()
    {
        glfwDestroyWindow(m_window);
    }
}