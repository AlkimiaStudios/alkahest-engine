#pragma once

#include "../src/init/entrypoint.h"

#include "../src/sys/log/log.h"

#include "../src/sys/events/event.h"
#include "../src/sys/events/eventqueue.h"

#include "../src/sys/input/input.h"

#include "../src/sys/window/window.h"

#include "../src/net/message.h"
#include "../src/net/connection.h"
#include "../src/net/client.h"
#include "../src/net/server.h"

#ifdef SOME_DX11_MACRO
#else // OpenGL Fallback
#include "../src/sys/input/openglkeys.h"
#endif