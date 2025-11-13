#include "enemy.h"

#include "player.h"
#include "borealis/gfx/camera.hpp"
#include "borealis/gfx/engine.hpp"
#include "borealis/gfx/sprite.hpp"

std::vector<EnemyController*> EnemyController::cachedEnemies;

EnemyController::EnemyController()
{
    cachedEnemies.push_back(this);

    auto texture = new brl::GfxTexture2d("textures/Units/Black Units/Warrior/Warrior_Idle.png");
    auto walkTexture = new brl::GfxTexture2d("textures/Units/Black Units/Warrior/Warrior_Run.png");
    auto attackTexture = new brl::GfxTexture2d("textures/Units/Black Units/Warrior/Warrior_Attack1.png");
    auto guardTexture = new brl::GfxTexture2d("textures/Units/Black Units/Warrior/Warrior_Guard.png");
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

void EnemyController::update()
{
    EcsEntity::update();

    float deltaTime = brl::GfxEngine::instance->getDeltaTime();

    auto entityPosition = glm::vec3(0);
    float entityDistance = 0;
    PlayerEntity* playerEntity = nullptr;

    for (auto cachedEntity : PlayerEntity::cachedEntities)
    {
        glm::vec3 pos = cachedEntity->position();

        float distance = glm::distance(pos, position());

        if (distance < 7.5f)
        {
            playerEntity = cachedEntity;
            entityPosition = pos;
            entityDistance = distance;
        }

    }

    velocity = glm::vec2(0);
    bool attack = false;
    isGuarding = false;

    attackTimer += deltaTime;
    attackDelay = 5.0f;

    if (playerEntity && length(velocity) <= 0.1f)
    {

        if (entityDistance > 2.5f)
        {
            glm::vec3 diff = entityPosition - position();
            diff = normalize(diff);

            velocity.x = diff.x;
            velocity.y = diff.z;
        }
        else if (entityDistance <= 2.5f)
        {
            if (attackTimer >= attackDelay)
            {
                startAttackDistance = entityDistance;
                glm::vec3 diff = entityPosition - position();
                diff = normalize(diff);
                float power = playerEntity->isGuarding ? 5.0f : 12.5f;
                playerEntity->handleAttack(diff, power);
                attack = true;
                attackTimer = 0.f;
            }
            else if (attackTimer > attackDelay * (3.f / 4))
            {
                isGuarding = true;
            }
        }
    }

    float horizontal = velocity.x;
    float vertical = velocity.y;

    glm::vec3 moveDir = (glm::vec3{horizontal, 0, vertical} * 2.5f) * deltaTime;

    localPosition += moveDir;

    material->setVec2("moveDir", {horizontal, vertical});


    if (attack)
    {
        material->setFloat("attackTime", glfwGetTime());
    }


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
}

void EnemyController::handleAttack(glm::vec3 dir, float power)
{
    brl::GfxEngine::instance->active_coroutines.push_back(AttackCoroutine(dir, power));
    //brl::UtilCoroutine::startCoroutine([this, dir] { return AttackCoroutine(dir, 7.5f); });
}

brl::UtilCoroutine EnemyController::AttackCoroutine(glm::vec3 dir, float power)
{
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

}
