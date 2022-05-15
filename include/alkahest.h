#pragma once

#include "../src/init/entrypoint.h"

#include "../src/sys/log/log.h"

#include "../src/ecs/common.h"
#include "../src/ecs/entity.h"
#include "../src/ecs/components/include.h"
#include "../src/ecs/systems/include.h"

#include "../src/sys/events/event.h"
#include "../src/sys/events/eventqueue.h"

#include "../src/sys/input/input.h"

#include "../src/sys/window/window.h"

#include "../src/net/message.h"
#include "../src/net/connection.h"
#include "../src/net/client.h"
#include "../src/net/server.h"

#include "../src/renderer/common.h"
#include "../src/renderer/model.h"
#include "../src/renderer/texture.h"
#include "../src/renderer/text.h"

#include "../src/util/importers/modelimporter.h"

#ifdef SOME_DX11_MACRO
#else // OpenGL Fallback
#include "../src/sys/input/openglkeys.h"
#endif
