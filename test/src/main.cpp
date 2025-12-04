#include <cstdio>
#include <iostream>

#include "enemy.h"
#include "player.h"
#include "map.h"
#include "borealis/gfx/ui.hpp"
#include "borealis/debug/debug.hpp"

int main(int argc, const char* argv[])
{
#if _MSC_VER >= 1930
    system("..\\tools\\out\\resource_packer.exe ../test/resources/ ../out/assets.res");
    system("..\\tools\\out\\resource_packer.exe ../default_assets/ ../out/default_assets.res");
#endif

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    brl::IoEngine ioMgr = {};
    brl::EcsEngine entityMgr = {};


    brl::GfxEngine engine;
    engine.initialize(1920/3, 1080/3, "cp1 game");

    float framebufferScale = 0.75f;

    auto camera = new brl::EcsCamera();
    camera->localPosition = {0, 1.0f, 5.f};
    camera->fieldOfView = 90.f;
    camera->targetFramebuffer =
        new brl::GfxFramebuffer(brl::GfxEngine::instance->getMainWidth() * framebufferScale,
                                brl::GfxEngine::instance->getMainHeight() * framebufferScale);

    auto canvas = new brl::GfxCanvas();
    canvas->referenceResolution = {1920, 1080};


    auto heartIcon = new brl::GfxImage(canvas);
    {
        heartIcon->setParent(canvas);

        heartIcon->loadTexture(brl::GfxTexture2d::loadTexture("ui/gameui_HeartIcon.png"));
        heartIcon->localPosition = {50,75,0};
        //heartIcon->SetSize({100, 100});
    }

    auto healthBar = new brl::GfxImage(canvas);
    {
        healthBar->setParent(canvas);
        healthBar->material =
            new brl::GfxMaterial(new brl::GfxShaderProgram(brl::readFileString("shaders/health_bar_player.glsl")));

        healthBar->color = {40, 209, 25};

        healthBar->loadTexture(brl::GfxTexture2d::loadTexture("ui/gameui_HealthBar.png"));
        healthBar->localPosition = {250, 50, 0};
        // heartIcon->SetSize({100, 100});
    }


    auto map = new MapController();

    map->loadMap();

    auto player = new PlayerController(Red);
    player->healthBarImage = healthBar;

    auto enemy = new EnemySpawner(Red);
    enemy->localPosition = {0, 0, -20};


    bool b = false;
    while (engine.isRunning())
    {

        camera->localPosition = mix(camera->localPosition, player->position() + glm::vec3{0, 5, 5},
                                    engine.getDeltaTime() * 10.f);
        glm::quat rotPrev = camera->localRotation;
        camera->lookAt(player->position());
        camera->localRotation = lerp(rotPrev, camera->localRotation,
                                     glm::clamp(engine.getDeltaTime() * 10.f, 0.f, 1.f));

        //heartIcon->SetPosition({0, 0});

        entityMgr.update();

        engine.update();


    }

    entityMgr.shutdown();
    ioMgr.shutdown();

    engine.shutdown();
    return 0;
}
