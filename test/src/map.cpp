#include "map.h"

MapObject::MapObject(brl::GfxMeshRenderer *renderer)
{
    this->renderer = renderer;
    renderer->setParent(this);
}

void MapObject::update()
{
    renderer->localPosition = glm::vec3{0,0,0};
    renderer->lookAt(brl::GfxCamera::mainCamera->position);

}
