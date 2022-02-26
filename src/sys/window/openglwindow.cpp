#include "openglwindow.h"
#include "../log/log.h"
#include "../events/windowevent.h"
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../input/input.h"
#include <cmath>

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
    static bool s_glfwInit = false;

    static void handleGLFWError(int e, const char *msg)
    {
        logError("GLFW Error (Code {}): {}", e, msg);
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
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_VERTEX_ARRAY, m_vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
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

        logTrace("Creating window {}: ({},{})", m_data.title, m_data.width, m_data.height);

        glfwWindowHintString(GLFW_X11_CLASS_NAME, "alkahest");
        glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "alkahest");

        m_window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, &m_data);
        glfwSetInputMode(m_window, GLFW_CURSOR, ALKAHEST_CURSOR_MODE);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        setVSync(true);

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        glViewport(0, 0, width, height);

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
            glViewport(0, 0, width, height);
        });

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
            Input::setKeyState(static_cast<Key>(key), static_cast<KeyState>(action));
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
            Input::setMouseButtonState(static_cast<MouseButton>(button), static_cast<ButtonState>(action));
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
            Input::setMousePos(x, y);
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y){
            Input::setMouseScroll(x, y);
        });

        static const GLfloat g_vertex_buffer_data[] = {
            -0.3f, -0.5f * static_cast<float>(std::sqrt(3)) / 3, 0.0f,
            0.3f, -0.5f * static_cast<float>(std::sqrt(3)) / 3, 0.0f,
            0.0f,  0.5f * static_cast<float>(std::sqrt(3)) * 2 / 3, 0.0f,
        };

        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    }

    void OpenGLWindow::shutdown()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}