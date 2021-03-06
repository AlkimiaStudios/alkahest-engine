#include "openglwindow.h"
#include "../log/log.h"
#include "../events/windowevent.h"
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../input/input.h"
#include "../../renderer/text.h"

#include "../../util/importers/modelimporter.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    IWindow* IWindow::instance = nullptr;
    static bool s_glfwInit = false;

    static void handleGLFWError(int e, const char *msg)
    {
        logError("GLFW Error (Code {}): {}", e, msg);
    }

    IWindow* IWindow::create(const WindowProps& props)
    {
        if (instance == nullptr)
            instance = new OpenGLWindow(props);
        return instance;
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
        // Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        m_cam->getViewMatrix(45.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100.0f);
        
        for (Ref<IRenderable> r : m_renderables)
        {
            r->draw();
        }
        
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

        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        glViewport(0, 0, m_width, m_height);

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

        m_modelShader = Shader::create("shaders/default.vert", "shaders/default.frag");
        m_lightShader = Shader::create("shaders/light.vert", "shaders/light.frag");
        m_textShader = Shader::create("shaders/text.vert", "shaders/text.frag");

        glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);

        m_modelShader->activate();
        glUniform4f(glGetUniformLocation(m_modelShader->getID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(m_modelShader->getID(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void OpenGLWindow::setInputMode(InputMode mode)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }

    void OpenGLWindow::registerRenderable(Ref<IRenderable> r)
    {
        m_renderables.push_back(r);

        switch (r->getType())
        {
        case IRenderable::RenderTypeModel:
            r->setShader(m_modelShader);
            break;
        case IRenderable::RenderTypeLight:
            r->setShader(m_lightShader);
            break;
        case IRenderable::RenderTypeText:
            r->setShader(m_textShader);
            break;
        default:
            r->setShader(m_modelShader);
            break;
        }

        r->setCamera(m_cam);
    }

    void OpenGLWindow::shutdown()
    {
        // cleanup
        m_modelShader->destroy();
        m_lightShader->destroy();
        m_textShader->destroy();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}