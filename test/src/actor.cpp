#include "actor.h"

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

void ActorBehaviour::onDeath() {
    isAlive = false;
}
