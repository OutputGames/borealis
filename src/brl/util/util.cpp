#include "borealis/util/util.h"

#include "borealis/gfx/engine.hpp"

void brl::UtilCoroutine::startCoroutine(std::function<UtilCoroutine(void)> c)
{
    GfxEngine::instance->active_coroutines.push_back(c());
}
