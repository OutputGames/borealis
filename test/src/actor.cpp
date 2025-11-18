#include "actor.h"

#include "borealis/gfx/camera.hpp"

ActorBehaviour::ActorBehaviour()
{

}

void ActorBehaviour::update()
{
    if (health <= 0 && isAlive)
        onDeath();
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

    renderer->lookAt(brl::GfxCamera::mainCamera->position);
}
