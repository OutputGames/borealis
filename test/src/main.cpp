#include <cstdio>
#include <iostream>
#include <borealis/gfx/gfx.hpp>

#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

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


    auto texture = new brl::GfxTexture2d("textures/Warrior/Warrior_Idle.png");
    auto walkTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Run.png");
    auto attackTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Attack1.png");
    auto guardTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Guard.png");
    auto idleSprites = brl::GfxSprite::extractSpritesToArray(texture, 192, 192);
    auto walkSprites = brl::GfxSprite::extractSpritesToArray(walkTexture, 192, 192);
    auto attackSprites = brl::GfxSprite::extractSpritesToArray(attackTexture, 192, 192);
    auto guardSprites = brl::GfxSprite::extractSpritesToArray(guardTexture, 192, 192);

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

    auto renderer = new brl::GfxMeshRenderer();
    brl::GfxMaterial* material = nullptr;

    {
        auto shaderBins = new brl::GfxShader*[2];

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/test/vtx.glsl"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/test/frg.glsl"));
        auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);


        material = new brl::GfxMaterial(shader);
        // material->setVec3("color", brl::vector3{1,0,0});
        material->setTexture("idleSprite", idleSprites);
        material->setTexture("walkSprite", walkSprites);
        material->setTexture("attackSprite", attackSprites);
        material->setTexture("guardSprite", guardSprites);

        renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
        renderer->material = material;
        renderer->localPosition = {0, 1.f, 0};
        renderer->localScale = glm::vec3(2.5f);
    }


    while (engine.isRunning())
    {
        float horizontal = brl::InputMgr::getAxisRaw("Horizontal");
        float vertical = -brl::InputMgr::getAxisRaw("Vertical");

        glm::vec3 moveDir =
            glm::vec3{horizontal, 0, vertical} *
            engine.getDeltaTime() * 5.f;

        renderer->localPosition += moveDir;

        material->setVec2("moveDir", {horizontal, vertical});

        if (brl::InputMgr::getMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
        {
            material->setFloat("attackTime", glfwGetTime());
        }

        if (brl::InputMgr::getMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
        {
            material->setInt("guarding", 1);
        }
        else
        {
            material->setInt("guarding", 0);
        }

        if (glm::abs(length(moveDir)) <= 0.0001f)
        {
        }
        else
        {


            if (horizontal > 0)
            {
                material->setInt("flip", 0);
            }
            else
            {
                material->setInt("flip", 1);
            }
        }


        camera->localPosition = renderer->position() + glm::vec3{0, 5, 5};
        renderer->lookAt(camera->localPosition);
        camera->lookAt(renderer->position());

        entityMgr.update();
        engine.update();
    }

    engine.shutdown();
    return 0;
}
