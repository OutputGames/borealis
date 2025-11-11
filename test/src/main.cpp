#include <cstdio>
#include <iostream>
#include "player.h"

int main(int argc, const char* argv[])
{
    system("..\\tools\\out\\resource_packer.exe ../test/resources/ ../out/");

    brl::IoEngine ioMgr;
    brl::EcsEngine entityMgr;


    brl::GfxEngine engine;
    engine.initialize();

    auto camera = new brl::EcsCamera();
    camera->localPosition = {0, 4.0f, 5.f};
    camera->fieldOfView = 75.f;
    //camera->type = brl::ORTHOGRAPHIC;

    auto tilemapTexture = new brl::GfxTexture2d("textures/Tilemap_color1.png");
    auto tilemapSprites = brl::GfxSprite::extractSpritesToArray(tilemapTexture, 64, 64);


    {
        auto shaderBins = new brl::GfxShader*[2];

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/test/vtx.glsl"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/test/floorFrg.glsl"));
        auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);


        auto floorTexture = new brl::GfxTexture2d("textures/test.png");

        auto floorMaterial = new brl::GfxMaterial(shader);
        // material->setVec3("color", brl::vector3{1,0,0});
        floorMaterial->setTexture("tex", tilemapSprites);

        auto floorRenderer = new brl::GfxMeshRenderer();
        floorRenderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
        floorRenderer->material = floorMaterial;
        floorRenderer->setEulerAngles({-90, 0, 0});
        floorRenderer->localScale = glm::vec3(15.0f);
        floorRenderer->localPosition = {0, 0, -5.f};
    }

    PlayerController* player = new PlayerController();


    while (engine.isRunning())
    {


        player->update();

        camera->localPosition = player->position() + glm::vec3{0, 5, 5};

        camera->lookAt(player->position());

        entityMgr.update();
        engine.update();
    }

    engine.shutdown();
    return 0;
}
