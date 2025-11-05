#include <cstdio>
#include <iostream>
#include <borealis/gfx/gfx.hpp>

int main(int argc, char const *argv[])
{
    brl::GfxEngine engine;
    engine.initialize();

    while(engine.isRunning())
    {
        engine.update();
    }

    engine.shutdown();
    return 0;
}
