#include <cstdio>
#include <iostream>

#include "player.h"
#include "map.h"

int main(int argc, const char* argv[])
{
    system("..\\tools\\out\\resource_packer.exe ../test/resources/ ../out/");

    brl::IoEngine ioMgr;
    brl::EcsEngine entityMgr;


    brl::GfxEngine engine;
    engine.initialize();

    auto camera = new brl::EcsCamera();
    camera->localPosition = {0, 5.0f, 5.f};
    camera->fieldOfView = 90.f;
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

    {
        auto shaderBins = new brl::GfxShader*[2];

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map_object/vtx.glsl"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map_object/frg.glsl"));
        auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map_object/vtx.glsl"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map_object/anim_frg.glsl"));
        auto anim_shader = new brl::GfxShaderProgram(shaderBins, 2, true);

        /*
        {
            auto material = new brl::GfxMaterial(shader);
            material->setTexture("tex", new brl::GfxTexture2d("textures/YellowBuildings/Tower.png"));

            auto renderer = new brl::GfxMeshRenderer();
            renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
            renderer->material = material;
            renderer->setEulerAngles({-90, 0, 0});
            renderer->localScale = glm::vec3(2.5f);
            auto uni = material->getUniform("tex");

            renderer->localScale.y *= uni.txValue->getHeight() / uni.txValue->getWidth();


            auto mapObject = new MapObject(renderer);
            mapObject->localPosition = {5,(renderer->localScale.y/2)+1,5};
        }
            */

        {
            auto material = new brl::GfxMaterial(anim_shader);
            material->setTexture("tex", brl::GfxSprite::extractSpritesToArray( new brl::GfxTexture2d("textures/Trees/Tree1.png"), 192, 256));

            auto renderer = new brl::GfxMeshRenderer();
            renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
            renderer->material = material;
            renderer->setEulerAngles({-90, 0, 0});
            renderer->localScale = glm::vec3(2.5f);

            auto uni = material->getUniform("tex");

            renderer->localScale.y *= uni.txValue->getHeight() / uni.txValue->getWidth();

            auto mapObject = new MapObject(renderer);
            mapObject->localPosition = {-5,(renderer->localScale.y/2)+1,5};
        }

        {
            auto material = new brl::GfxMaterial(anim_shader);
            material->setTexture("tex", brl::GfxSprite::extractSpritesToArray( new brl::GfxTexture2d("textures/Trees/Tree2.png"), 192, 256));

            auto renderer = new brl::GfxMeshRenderer();
            renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
            renderer->material = material;
            renderer->setEulerAngles({-90, 0, 0});
            renderer->localScale = glm::vec3(2.5f);

            auto uni = material->getUniform("tex");

            renderer->localScale.y *= uni.txValue->getHeight() / uni.txValue->getWidth();

            auto mapObject = new MapObject(renderer);
            mapObject->localPosition = {5,(renderer->localScale.y/2)+1,5};
        }
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
