#pragma once

#include "../../macros.h"
#include "window.h"
#include "../events/event.h"
#include "../input/input.h"
#include "../render/shader.h"
#include "../render/buffer.h"
#include "../render/camera.h"
#include "../render/mesh.h"
#include "../../renderer/model.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define ALKAHEST_CURSOR_MODE_NORMAL GLFW_CURSOR_NORMAL
#define ALKAHEST_CURSOR_MODE_HIDDEN GLFW_CURSOR_HIDDEN

#ifndef ALKAHEST_CURSOR_MODE
#define ALKAHEST_CURSOR_MODE ALKAHEST_CURSOR_MODE_NORMAL
#endif

#ifndef ALKAHEST_RAW_MOTION
#define ALKAHEST_RAW_MOTION GLFW_TRUE
#endif

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
    class NOT_EXPORTED OpenGLWindow : public IWindow
    {
    public:
        OpenGLWindow(const WindowProps& props);
        virtual ~OpenGLWindow();

        void onUpdate() override;

        inline virtual int getWidth() const override { return m_width; };
        inline virtual int getHeight() const override { return m_height; };

        virtual void setVSync(bool vsync) override;
        virtual bool isVSync() const override;

        virtual inline void setEventCallback(std::function<void(Event*)> func) { m_data.eventCallback = func; };
        virtual inline void setMainCamera(Ref<Camera> c) { m_cam = c; };
        virtual inline void initializeInput() { Input::initialize(m_window); };
        virtual void setInputMode(InputMode mode) override;
        void registerRenderable(Ref<IRenderable> r) override;
    private:
        static OpenGLWindow *instance;

        void init(const WindowProps& props);
        void shutdown();

        int m_width, m_height;

        struct WindowData {
            std::string title;
            unsigned int width;
            unsigned int height;
            bool vsync;
            std::function<void(Event*)> eventCallback;
        };
        WindowData m_data;
        GLFWwindow *m_window;

        Ref<Shader> m_modelShader;
        Ref<Shader> m_lightShader;
        Ref<Shader> m_textShader;

        Ref<Camera> m_cam;

        std::vector<Ref<IRenderable>> m_renderables;
    };
}