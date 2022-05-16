#pragma once

#include "application.h"

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

extern Alkahest::Application* Alkahest::CreateApplication();

int main (int argc, char** argv)
{
    Alkahest::Application* app = Alkahest::CreateApplication();
    app->run();
    delete app;
    return 0;
}
