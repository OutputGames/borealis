#include "player.h"

PlayerController::PlayerController()
{


    auto texture = new brl::GfxTexture2d("textures/Warrior/Warrior_Idle.png");
    auto walkTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Run.png");
    auto attackTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Attack1.png");
    auto guardTexture = new brl::GfxTexture2d("textures/Warrior/Warrior_Guard.png");
    idleSprites = brl::GfxSprite::extractSpritesToArray(texture, 192, 192);
    walkSprites = brl::GfxSprite::extractSpritesToArray(walkTexture, 192, 192);
    attackSprites = brl::GfxSprite::extractSpritesToArray(attackTexture, 192, 192);
    guardSprites = brl::GfxSprite::extractSpritesToArray(guardTexture, 192, 192);

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

    renderer = new brl::GfxMeshRenderer();
    renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
    renderer->material = material;
    renderer->localPosition = {0, 1.f, 0};
    renderer->localScale = glm::vec3(2.5f);
    renderer->setParent(this);
}

void PlayerController::update()
{
    float horizontal = brl::InputMgr::getAxisRaw("Horizontal");
    float vertical = -brl::InputMgr::getAxisRaw("Vertical");

    glm::vec3 moveDir =
        glm::vec3{horizontal, 0, vertical} *
        brl::GfxEngine::instance->getDeltaTime() * 5.f;

    localPosition += moveDir;

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

        renderer->lookAt(brl::GfxCamera::mainCamera->position);
}
