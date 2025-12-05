#include "enemy.h"

#include "map.h"
#include "player.h"
#include "borealis/debug/debug.hpp"
#include "borealis/gfx/camera.hpp"
#include "borealis/gfx/engine.hpp"
#include "borealis/gfx/sprite.hpp"
#include "borealis/util/random.hpp"

std::vector<EnemyController*> EnemyController::cachedEnemies;

brl::GfxTexture2dArray* EnemyController::idleSprites;
brl::GfxTexture2dArray* EnemyController::walkSprites;
brl::GfxTexture2dArray* EnemyController::attackSprites;
brl::GfxTexture2dArray* EnemyController::guardSprites;

EnemyController::EnemyController(EnemyTeam team)
{
    Team = team;
    cachedEnemies.push_back(this);

    if (!idleSprites)
    {
        std::string unitFolder = "textures/Units/NonColor Units/";

        auto texture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Idle.png");
        auto walkTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Run.png");
        auto attackTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Attack1.png");
        auto guardTexture = brl::GfxTexture2d::loadTexture(unitFolder + "Warrior/Warrior_Guard.png");
        idleSprites = brl::GfxSprite::extractSpritesToArray(texture, 192, 192, true);
        walkSprites = brl::GfxSprite::extractSpritesToArray(walkTexture, 192, 192, true);
        attackSprites = brl::GfxSprite::extractSpritesToArray(attackTexture, 192, 192, true);
        guardSprites = brl::GfxSprite::extractSpritesToArray(guardTexture, 192, 192, true);
    }

    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/enemy/enemy.vert"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/enemy/enemy.frag"));
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);


    material = new brl::GfxMaterial(shader);
    {
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
    }
    material->setTexture("idleSprite", idleSprites);
    material->setTexture("walkSprite", walkSprites);
    material->setTexture("attackSprite", attackSprites);
    material->setTexture("guardSprite", guardSprites);

    renderer = new brl::GfxMeshRenderer();
    renderer->mesh = brl::GfxMesh::GetPrimitive(brl::QUAD);
    renderer->setMaterial(material);
    renderer->localPosition = {0, 1.f, 0};
    renderer->localScale = glm::vec3(2.5f);
    renderer->setParent(this);

    healthBar = new HealthBarBehavior;
    healthBar->localPosition = {0, 2.5f, 0};
    switch (Team)
    {

        case Red:
            healthBar->color = glm::vec3(212, 28, 64);
            break;
        case Blue:
            healthBar->color = glm::vec3(66, 93, 245);
            break;
        case Yellow:
            healthBar->color = glm::vec3(245, 188, 66);
            break;
        case Black:
            healthBar->color = glm::vec3(28, 36, 48);
            break;
    }

    healthBar->setParent(this);


}

void EnemyController::update()
{
    ActorBehaviour::update();


    float deltaTime = brl::GfxEngine::instance->getDeltaTime();

    healthBar->health = glm::mix(healthBar->health, health, deltaTime * 15.f);

    if (!isAlive)
        return;


    auto entityPosition = glm::vec3(0);
    float entityDistance = 0;
    ActorBehaviour* targetEntity = nullptr;

    for (auto cachedEntity : EnemyController::cachedEnemies)
    {
        glm::vec3 pos = cachedEntity->position();

        float distance = glm::distance(pos, position());

        if (distance < 7.5f && cachedEntity->Team != Team)
        {
            targetEntity = cachedEntity;
            entityPosition = pos;
            entityDistance = distance;
        }

    }

    if (!targetEntity) {
        for (auto cachedEntity : PlayerEntity::cachedEntities)
        {
            glm::vec3 pos = cachedEntity->position();

            float distance = glm::distance(pos, position());

            if (distance < 7.5f && cachedEntity->Team != Team)
            {
                targetEntity = cachedEntity;
                entityPosition = pos;
                entityDistance = distance;
            }

        }
    }



    bool attack = false;
    isGuarding = false;

    attackTimer += deltaTime;
    attackDelay = 5.0f;

    if (targetEntity)
    {
        wanderPosition = position();
        if (entityDistance > 2.5f)
        {
            glm::vec3 diff = entityPosition - position();
            diff = normalize(diff);

            velocity.x = diff.x;
            velocity.y = diff.z;
        }
        else if (entityDistance <= 2.5f)
        {
            velocity = glm::vec2(0);
            if (attackTimer >= attackDelay)
            {
                startAttackDistance = entityDistance;
                glm::vec3 diff = entityPosition - position();
                diff = normalize(diff);
                float power = 12.5f;
                targetEntity->handleAttack(diff, power);
                attack = true;
                attackTimer = 0.f;
            }
            else if (attackTimer > attackDelay * (3.f / 4))
            {
                isGuarding = true;
            }
        }
    } else {
        float spawnerDistance = glm::distance(position(), spawner->position());

        if (spawnerDistance > 5.f)
        {
            glm::vec3 diff = spawner->position() - position();
            diff = normalize(diff);

            velocity.x = diff.x;
            velocity.y = diff.z;
            wanderPosition = position();
        } 
    }

    float horizontal = velocity.x;
    float vertical = velocity.y;

    glm::vec3 moveDir = (glm::vec3{horizontal, 0, vertical} * 5.f) * deltaTime;

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

void EnemyController::onDestroy()
{
    ActorBehaviour::onDestroy();


}

void EnemyController::onDeath()
{
    ActorBehaviour::onDeath();

    brl::GfxEngine::instance->active_coroutines.push_back(DeathCoroutine());

}

brl::UtilCoroutine EnemyController::AttackCoroutine(glm::vec3 dir, float power)
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

    health -= power / 100.0f;
}

brl::UtilCoroutine EnemyController::DeathCoroutine()
{
    glm::vec3 startScale = renderer->localScale;
    glm::vec3 startPos = renderer->localPosition;

    float timeToReach = 0.5f;
    float t = 0;
    while (t < timeToReach)
    {
        renderer->localScale = mix(startScale, glm::vec3(0.1f), t / timeToReach);
        renderer->localPosition = mix(startPos, glm::vec3(0), t / timeToReach);

        t += brl::GfxEngine::instance->getDeltaTime();

        co_yield brl::GfxEngine::instance->getDeltaTime();
    }

    destroy();

}

EnemySpawner::EnemySpawner(EnemyTeam t)
{
    auto tower = brl::GfxModel::loadModel("models/tower/tower.glb");

    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map_object/vtx.glsl"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map_object/frg.glsl"));
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

    team = t;

    auto color = glm::vec3{0};
    switch (team)
    {

        case Red:
            color = glm::vec3(212, 28, 64);
            break;
        case Blue:
            color = glm::vec3(66, 93, 245);
            break;
        case Yellow:
            color = glm::vec3(245, 188, 66);
            break;
        case Black:
            color = glm::vec3(28, 36, 48);
            break;
    }

    for (auto material : tower->materials)
    {
        material->reloadShader(shader);
        material->setVec3("_color", color);
    }

    const auto& towerEntity = tower->createEntity();

    renderer = towerEntity->getEntityInChildren<brl::GfxMeshRenderer>();
    for (int i = 0; i < renderer->materials.size(); ++i)
    {
        auto material = new brl::GfxMaterial(*renderer->materials[i]);
        material->reloadShader(shader);
        material->setVec3("_color", color);

        renderer->materials[i] = material;
    }
    materials = renderer->materials;

    towerEntity->setEulerAngles({0, 180, 0});
    towerEntity->localScale = glm::vec3(1.0f);

    towerEntity->setParent(this);

    healthBar = new HealthBarBehavior;
    healthBar->localPosition = {0, 5.f, 0};
    switch (team)
    {

        case Red:
            healthBar->color = glm::vec3(212, 28, 64);
            break;
        case Blue:
            healthBar->color = glm::vec3(66, 93, 245);
            break;
        case Yellow:
            healthBar->color = glm::vec3(245, 188, 66);
            break;
        case Black:
            healthBar->color = glm::vec3(28, 36, 48);
            break;
    }

    healthBar->setParent(this);

}

void EnemySpawner::start()
{
    ActorBehaviour::start();
    health = 1;

    auto pos = position();
    for (int i = 0; i < count; ++i)
    {
        float angle = (360.f / count) * i;
        angle = glm::radians(angle);

        glm::vec3 startPosition = {};
        startPosition.x = glm::sin(angle) * radius;
        startPosition.z = glm::cos(angle) * radius;
        startPosition += pos;

        auto enemy = new EnemyController(team);
        enemy->localPosition = startPosition;
        enemy->Team = team;
        enemy->spawner = this;

        enemies.push_back(enemy);
    }
}


void EnemySpawner::update()
{
    ActorBehaviour::update();
    float deltaTime = brl::GfxEngine::instance->getDeltaTime();

    healthBar->health = glm::mix(healthBar->health, health, deltaTime * 15.f);

    if (!isAlive)
        return;

    
    brl_debug::drawMesh(brl::GfxMesh::GetPrimitive(brl::CIRCLE)->GetSubMesh(0)->buffer,
            glm::translate(position())*glm::scale(scale()*radius));

    brl_debug::drawLine(position(), glm::vec3(0,5,0));

    if (enemies.size() > 0) {

    }
}

void EnemySpawner::onDeath() {
    ActorBehaviour::onDeath();

    brl::GfxEngine::instance->active_coroutines.push_back(DeathCoroutine());

}

void EnemySpawner::handleAttack(glm::vec3 dir, float power) {
    ActorBehaviour::handleAttack(dir, power);
    brl::GfxEngine::instance->active_coroutines.push_back(AttackCoroutine(dir, power));

}

brl::UtilCoroutine EnemySpawner::AttackCoroutine(glm::vec3 dir, float power)
{
    float start = glfwGetTime();

    float diff = 0;

    while (diff * 10 <= 2.0f)
    {
        diff = glfwGetTime() - start;
        co_yield brl::GfxEngine::instance->getDeltaTime();
    }

    for (const auto & material : materials)
        material->setFloat("damageTime", glfwGetTime());

    while (diff * 10.0f > 2.0f && diff * 10.0f < 4.0f)
    {
        localPosition += (dir * power) * brl::GfxEngine::instance->getDeltaTime();
        co_yield brl::GfxEngine::instance->getDeltaTime();

        diff = glfwGetTime() - start;
    }

    health -= power /= 50.0f;
}

brl::UtilCoroutine EnemySpawner::DeathCoroutine() {
    glm::vec3 startScale = renderer->localScale;
    glm::vec3 startPos = renderer->localPosition;

    float timeToReach = 0.5f;
    float t = 0;
    while (t < timeToReach)
    {
        renderer->localScale = mix(startScale, glm::vec3(0.1f), t / timeToReach);
        renderer->localPosition = mix(startPos, glm::vec3(0), t / timeToReach);

        t += brl::GfxEngine::instance->getDeltaTime();

        co_yield brl::GfxEngine::instance->getDeltaTime();
    }

    destroy();
}
