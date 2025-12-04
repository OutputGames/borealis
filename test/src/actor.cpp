#include "actor.h"

#include "map.h"
#include "borealis/gfx/camera.hpp"

std::vector<ActorBehaviour*> ActorBehaviour::cachedActors;


ActorBehaviour::ActorBehaviour()
{
    cachedActors.push_back(this);
}

void ActorBehaviour::update()
{
    EcsEntity::update();
    if (health <= 0 && isAlive)
        onDeath();

    auto blockX = ((roundf(localPosition.x) / MAP_BLOCK_SPACING)) + ((MapController::Instance->chunkCountX / 2) *
        MAP_CHUNK_SIZE);
    auto blockZ = ((roundf(localPosition.z) / MAP_BLOCK_SPACING)) + ((MapController::Instance->chunkCountY / 2) *
        MAP_CHUNK_SIZE);


    int blockHeight = MapController::Instance->GetHeight(blockX, blockZ);
    float blockY = -(MAP_BLOCK_SPACING / 2) + ((MAP_BLOCK_SPACING / 2) * blockHeight);

    localPosition.y = glm::mix(localPosition.y, blockY + (MAP_BLOCK_SPACING / 2),
                               brl::GfxEngine::instance->getDeltaTime() * 10.f);
}

void ActorBehaviour::handleAttack(glm::vec3 dir, float power)
{
}

void ActorBehaviour::onDeath()
{
    isAlive = false;
}

HealthBarBehavior::HealthBarBehavior()
{

    auto texture = brl::GfxTexture2d::loadTexture("textures/healthbar.png");
    texture->clearCachedPixels();

    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/health_bar/healthbar.vert"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/health_bar/healthbar.frag"));
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);


    material = new brl::GfxMaterial(shader);
    material->setTexture("tex", texture);

    renderer = new brl::GfxMeshRenderer();
    renderer->mesh = brl::GfxMesh::GetPrimitive(brl::QUAD);
    renderer->setMaterial(material);
    renderer->localPosition = {0, 0.f, 0};

    renderer->localScale = glm::vec3(0.25f);
    renderer->localScale.x *= texture->getWidth() / texture->getHeight();

    renderer->setParent(this);
}

void HealthBarBehavior::update()
{
    EcsEntity::update();

    material->setFloat("health", health);
    material->setVec3("_color", color);

    renderer->lookAt(brl::GfxCamera::mainCamera->position);
}
