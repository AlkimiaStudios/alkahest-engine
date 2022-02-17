#pragma once

#include "application.h"

extern Alkahest::Application* Alkahest::CreateApplication();

int main (int argc, char** argv)
{
    Alkahest::Application* app = Alkahest::CreateApplication();
    app->run();
    delete app;
    return 0;
}