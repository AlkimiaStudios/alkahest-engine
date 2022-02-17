#pragma once

#include "pch.h"

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