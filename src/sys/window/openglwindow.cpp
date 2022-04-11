#include "openglwindow.h"
#include "../log/log.h"
#include "../events/windowevent.h"
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../input/input.h"

#include <glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

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
        // Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tell OpenGL which program to use
        m_shaderProgram->activate();

        // Basic 60fps ticks
        double ct = glfwGetTime();
        if (ct - m_prevTime >= 1/60)
        {
            m_rotation += 0.5f;
            m_prevTime = ct;
        }

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        glm::mat4 viewMatrix = m_cam->getViewMatrix(45.0f, static_cast<float>(width / height), 0.1f, 100.0f);
        GLint vmLoc = glGetUniformLocation(m_shaderProgram->getID(), "viewMatrix");
        glUniformMatrix4fv(vmLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Bind the VAO
        m_vao->bind();

        // Bind texture so it is displayed
        m_tex->bind();

        // Draw elements: primitives, number of elements, type of indices, starting index
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

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
        //if (glfwRawMouseMotionSupported())
            //glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
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

        m_shaderProgram = Shader::create("shaders/default.vert", "shaders/default.frag");

        std::vector<Vertex> vertices = {
        //     COORDINATES     /        COLORS      /   TexCoord  //
            {{-0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f}},
            {{-0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f}},
            {{0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f}},
            {{0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f}},
            {{0.0f, 0.8f,  0.0f},     {0.92f, 0.86f, 0.76f},	{2.5f, 5.0f}}
        };

        // Indices for vertices order
        std::vector<GLuint> indices =
        {
            0, 1, 2,
            0, 2, 3,
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4
        };

        m_vao = VertexArray::create();
        m_vao->bind();

        // Generate the VBO and EBO
        m_vbo = VertexBuffer::create(vertices);
        m_ebo = ElementBuffer::create(indices);

        m_vao->linkAttribute(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), static_cast<void*>(0));
        m_vao->linkAttribute(m_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 2, 2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

        // Re-bind the VAO and VBO to 0 so we don't accidentally modify them
        m_vao->unbind();
        m_vbo->unbind();

        // Re-bind the EBO *after* the VAO because the VAO is using the EBO
        m_ebo->unbind();

        m_scale = glGetUniformLocation(m_shaderProgram->getID(), "scale");

        m_tex = Texture::create("assets/textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        m_tex->setUniformTexture(m_shaderProgram, "tex0", 0);

        glEnable(GL_DEPTH_TEST);

        m_rotation = 0.0f;
        m_prevTime = glfwGetTime();
    }

    void OpenGLWindow::setInputMode(InputMode mode)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }

    void OpenGLWindow::shutdown()
    {
        // cleanup
        m_vao->destroy();
        m_vbo->destroy();
        m_ebo->destroy();
        m_tex->destroy();
        m_shaderProgram->destroy();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}