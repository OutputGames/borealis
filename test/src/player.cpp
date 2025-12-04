#include "player.h"

#include "enemy.h"
#include "borealis/gfx/ui.hpp"

std::vector<PlayerEntity*> PlayerEntity::cachedEntities;

PlayerEntity::PlayerEntity()
{
    cachedEntities.push_back(this);
}

void PlayerEntity::handleAttack(glm::vec3 dir, float power)
{
}

PlayerController::PlayerController(EnemyTeam team)
{
    Team = team;
    std::string unitFolder = "textures/Units/NonColor Units/";

    auto texture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Idle.png");
    auto walkTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Run.png");
    auto attackTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Attack1.png");
    auto guardTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Guard.png");
    idleSprites = brl::GfxSprite::extractSpritesToArray(texture, 192, 192, true);
    walkSprites = brl::GfxSprite::extractSpritesToArray(walkTexture, 192, 192, true);
    attackSprites = brl::GfxSprite::extractSpritesToArray(attackTexture, 192, 192, true);
    guardSprites = brl::GfxSprite::extractSpritesToArray(guardTexture, 192, 192, true);


    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/enemy/enemy.vert"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/enemy/enemy.frag"));
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

    material = new brl::GfxMaterial(shader);
    // material->setVec3("color", brl::vector3{1,0,0});
    material->setTexture("idleSprite", idleSprites);
    material->setTexture("walkSprite", walkSprites);
    material->setTexture("attackSprite", attackSprites);
    material->setTexture("guardSprite", guardSprites);

    glm::vec3 v;
    switch (Team)
    {

        case Red:
            v = glm::vec3(212, 28, 64);
            break;
        case Blue:
            v = glm::vec3(66, 93, 245);
            break;
        case Yellow:
            v = glm::vec3(245, 188, 66);
            break;
        case Black:
            v = glm::vec3(28, 36, 48);
            break;
    }
    material->setVec3("_color", v);
    material->setVec3("_armorColor", glm::vec3(239, 225, 171));
    material->setVec3("_weaponColor", glm::vec3(212, 237, 194));

    renderer = new brl::GfxMeshRenderer();
    renderer->mesh = brl::GfxMesh::GetPrimitive(brl::QUAD);
    renderer->setMaterial(material);
    renderer->localPosition = {0, 1.f, 0};
    renderer->localScale = glm::vec3(2.5f);
    renderer->setParent(this);
}

void PlayerController::update()
{
    PlayerEntity::update();

    float horizontal = brl::InputMgr::getAxisRaw("Horizontal");
    float vertical = -brl::InputMgr::getAxisRaw("Vertical");

    float deltatime = brl::GfxEngine::instance->getDeltaTime();

    glm::vec3 moveDir =
        (glm::vec3{horizontal, 0, vertical} * 5.f) * deltatime;

    localPosition += moveDir;

    material->setVec2("moveDir", {horizontal, vertical});

    if (brl::InputMgr::getMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
    {
        material->setFloat("attackTime", glfwGetTime());

        for (auto cachedEntity : ActorBehaviour::cachedActors)
        {
            if (cachedEntity == this)
                continue;

            glm::vec3 pos = cachedEntity->position();

            float distance = glm::distance(pos, position());

            if (distance < 2.5f)
            {
                cachedEntity->handleAttack(normalize(pos - position()),12.5f);
            }
        }
    }

    isGuarding = brl::InputMgr::getMouseButton(GLFW_MOUSE_BUTTON_RIGHT);

    if (isGuarding)
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

    healthBarImage->material->setFloat("health", glm::mix(healthBarImage->material->getUniform("health").floatValue,health,brl::GfxEngine::instance->getDeltaTime()*15.0f));
}

void PlayerController::handleAttack(glm::vec3 dir, float power)
{
    PlayerEntity::handleAttack(dir, power);
    brl::GfxEngine::instance->active_coroutines.push_back(AttackCoroutine(dir, power));
}

brl::UtilCoroutine PlayerController::AttackCoroutine(glm::vec3 dir, float power)
{
    if (isGuarding)
        power *= 0.5f;
    float start = glfwGetTime();

    float diff = 0;

    while (diff * 10 <= 2.0f)
    {
        diff = glfwGetTime() - start;
        co_yield brl::GfxEngine::instance->getDeltaTime();
    }

    material->setFloat("damageTime", glfwGetTime());

    while (diff * 10.0f > 2.0f && diff * 10.0f < 4.0f)
    {
        localPosition += (dir * power) * brl::GfxEngine::instance->getDeltaTime();
        co_yield brl::GfxEngine::instance->getDeltaTime();

        diff = glfwGetTime() - start;
    }

    
    health -= power / 250.0f;
}
