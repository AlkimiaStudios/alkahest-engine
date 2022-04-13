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

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        glm::mat4 camMatrix = m_cam->getViewMatrix(45.0f, static_cast<float>(width / height), 0.1f, 100.0f);
        m_cam->updateMatrixInShader(camMatrix, m_shaderProgram, "camMatrix");
        glm::vec3 camPos = m_cam->getPosition();
        glUniform3f(glGetUniformLocation(m_shaderProgram->getID(), "camPos"), camPos.x, camPos.y, camPos.z);

        // Bind texture so it is displayed
        m_tex->bind();

        // Bind the VAO
        m_vao->bind();

        // Draw elements: primitives, number of elements, type of indices, starting index
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        m_lightShader->activate();
        m_cam->updateMatrixInShader(camMatrix, m_lightShader, "camMatrix");
        m_lightVAO->bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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
        m_lightShader = Shader::create("shaders/light.vert", "shaders/light.frag");

        std::vector<Vertex> vertices = {
        //  Coordinates           /    Colors                /  TexCoords     /  Normals             //
            {{-0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   { 0.0f, -1.0f,  0.0f}}, // bottom
            {{-0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 5.0f},   { 0.0f, -1.0f,  0.0f}},
            {{ 0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 5.0f},   { 0.0f, -1.0f,  0.0f}},
            {{ 0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f},   { 0.0f, -1.0f,  0.0f}},

            {{-0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   {-0.8f,  0.5f,  0.0f}}, // left
            {{-0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f},   {-0.8f,  0.5f,  0.0f}},
            {{ 0.0f, 0.8f,  0.0f},     {0.92f, 0.86f, 0.76f},	{2.5f, 5.0f},   {-0.8f,  0.5f,  0.0f}},

            {{-0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f},   { 0.0f,  0.5f, -0.8f}}, // back
            {{ 0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   { 0.0f,  0.5f, -0.8f}},
            {{ 0.0f, 0.8f,  0.0f},     {0.92f, 0.86f, 0.76f},	{2.5f, 5.0f},   { 0.0f,  0.5f, -0.8f}},

            {{ 0.5f, 0.0f, -0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   { 0.8f,  0.5f,  0.0f}}, // right
            {{ 0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f},   { 0.8f,  0.5f,  0.0f}},
            {{ 0.0f, 0.8f,  0.0f},     {0.92f, 0.86f, 0.76f},	{2.5f, 5.0f},   { 0.8f,  0.5f,  0.0f}},

            {{ 0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{5.0f, 0.0f},   { 0.0f,  0.5f,  0.8f}}, // front
            {{-0.5f, 0.0f,  0.5f},     {0.83f, 0.70f, 0.44f},	{0.0f, 0.0f},   { 0.0f,  0.5f,  0.8f}},
            {{ 0.0f, 0.8f,  0.0f},     {0.92f, 0.86f, 0.76f},	{2.5f, 5.0f},   { 0.0f,  0.5f,  0.8f}}
        };

        // Indices for vertices order
        std::vector<GLuint> indices =
        {
            0, 1, 2,
            0, 2, 3,
            4, 6, 5,
            7, 9, 8,
            10, 12, 11,
            13, 15, 14
        };

        std::vector<Vertex> lightVertices = {
            {{-0.1f, -0.1f,  0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{-0.1f, -0.1f, -0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{ 0.1f, -0.1f, -0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{ 0.1f, -0.1f,  0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{-0.1f,  0.1f,  0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{-0.1f,  0.1f, -0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{ 0.1f,  0.1f, -0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{ 0.1f,  0.1f,  0.1f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
        };

        std::vector<GLuint> lightIndices =
        {
            0, 1, 2,
            0, 2, 3,
            0, 4, 7,
            0, 7, 3,
            3, 7, 6,
            3, 6, 2,
            2, 6, 5,
            2, 5, 1,
            1, 5, 4,
            1, 4, 0,
            4, 5, 6,
            4, 6, 7
        };

        m_vao = VertexArray::create();
        m_vao->bind();

        // Generate the VBO and EBO
        m_vbo = VertexBuffer::create(vertices);
        m_ebo = ElementBuffer::create(indices);

        m_vao->linkAttribute(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), static_cast<void*>(0));
        m_vao->linkAttribute(m_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 2, 2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 3, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(8 * sizeof(GL_FLOAT)));

        // Re-bind the VAO and VBO to 0 so we don't accidentally modify them
        m_vao->unbind();
        m_vbo->unbind();

        // Re-bind the EBO *after* the VAO because the VAO is using the EBO
        m_ebo->unbind();

        m_lightVAO = VertexArray::create();
        m_lightVAO->bind();

        m_lightVBO = VertexBuffer::create(lightVertices);
        m_lightEBO = ElementBuffer::create(lightIndices);

        m_lightVAO->linkAttribute(m_lightVBO, 0, 3, GL_FLOAT, sizeof(Vertex), static_cast<void*>(0));
        m_lightVAO->linkAttribute(m_lightVBO, 1, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
        m_lightVAO->linkAttribute(m_lightVBO, 2, 2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
        m_lightVAO->linkAttribute(m_lightVBO, 3, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(8 * sizeof(GL_FLOAT)));

        m_lightVAO->unbind();
        m_lightVBO->unbind();
        m_lightEBO->unbind();

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

        m_tex = Texture::create("assets/textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        m_tex->setUniformTexture(m_shaderProgram, "tex0", 0);

        glEnable(GL_DEPTH_TEST);
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

        m_lightVAO->destroy();
        m_lightVBO->destroy();
        m_lightEBO->destroy();
        m_lightShader->destroy();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}