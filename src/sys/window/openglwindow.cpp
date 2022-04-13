#include "openglwindow.h"
#include "../log/log.h"
#include "../events/windowevent.h"
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../input/input.h"

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

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        m_cam->getViewMatrix(45.0f, static_cast<float>(width / height), 0.1f, 100.0f);

        m_floor->draw(m_shaderProgram, m_cam);
        
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

        m_shaderProgram = Shader::create("shaders/default.vert", "shaders/default.frag");
        m_lightShader = Shader::create("shaders/light.vert", "shaders/light.frag");

        std::vector<Vertex> vertices = {
        //  Coordinates           /    Colors                /  TexCoords     /  Normals             //
            {{-1.0f, 0.0f,  1.0f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   { 0.0f,  1.0f,  0.0f}}, // bottom
            {{-1.0f, 0.0f, -1.0f},     {0.83f, 0.70f, 0.44f},	{0.0f, 1.0f},   { 0.0f,  1.0f,  0.0f}},
            {{ 1.0f, 0.0f, -1.0f},     {0.83f, 0.70f, 0.44f},	{1.0f, 1.0f},   { 0.0f,  1.0f,  0.0f}},
            {{ 1.0f, 0.0f,  1.0f},     {0.83f, 0.70f, 0.44f},	{1.0f, 0.0f},   { 0.0f,  1.0f,  0.0f}},
        };

        // Indices for vertices order
        std::vector<GLuint> indices =
        {
            0, 1, 2,
            0, 2, 3
        };

        glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);

        glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 pyramidModel = glm::mat4(1.0f);
        pyramidModel = glm::translate(pyramidModel, pyramidPos);

        m_shaderProgram->activate();
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram->getID(), "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
        glUniform4f(glGetUniformLocation(m_shaderProgram->getID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(m_shaderProgram->getID(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        m_lightShader->activate();
        glUniformMatrix4fv(glGetUniformLocation(m_lightShader->getID(), "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        glUniform4f(glGetUniformLocation(m_lightShader->getID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

        m_tex = Texture::create("assets/textures/planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE);
        m_spec = Texture::create("assets/textures/planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE);

        m_floor = Mesh::create(vertices, indices, {m_tex, m_spec});

        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLWindow::setInputMode(InputMode mode)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }

    void OpenGLWindow::shutdown()
    {
        // cleanup
        m_tex->destroy();
        m_spec->destroy();
        m_shaderProgram->destroy();
        m_lightShader->destroy();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}