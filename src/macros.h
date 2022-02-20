#pragma once

#include "pch.h"

// Much of the early systems developed for AlkahestEngine were developed following
// along with the Game Engine series from The Cherno (Yan Chernikov) as he built
// the Hazel game engine. A future refactor of these systems is planned to
// eventually re-work this code to adapt it more closely to the direction the
// AlkahestEngine is heading. In the meantime, however, thank you to Yan for
// providing one of the most educational channels on YT when it comes to C++
// and OpenGL development.

#if defined WIN32
    #ifdef WIN_EXPORT
        #define API __declspec(dllexport)
    #else
        #define API __declspec(dllimport)
    #endif
    #define NOT_EXPORTED
#elif defined GNU_EXPORT
    #define API __attribute__((visibility("default")))
    #define NOT_EXPORTED __attribute__((visibility("hidden")))
#else
    #define API
    #define NOT_EXPORTED
#endif

#ifndef BIT
    #define BIT(x) (1 << x)
#endif