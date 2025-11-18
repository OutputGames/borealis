#include "map.h"

MapObject::MapObject(brl::GfxMeshRenderer* renderer)
{
    this->renderer = renderer;
    renderer->setParent(this);
}

void MapObject::update()
{
    renderer->localPosition = glm::vec3{0, 0, 0};
    renderer->lookAt(brl::GfxCamera::mainCamera->position);

}

void MapController::loadMap()
{

    auto mapData =
        new int[]{
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        };

    int mapWidth = 10;
    int mapHeight = 10;
    float spacing = 2.6f;

    for (int x = -mapWidth / 2; x < mapWidth / 2; ++x)
    {
        for (int y = -mapHeight / 2; y < mapHeight / 2; ++y)
        {
            int index = ((y + mapHeight / 2) * mapWidth) + (x + mapWidth / 2);
            int value = mapData[index];

            for (int i = 0; i <= value; ++i)
            {
                brl::GfxModel* block = nullptr;

                if (i != value)
                {
                    block = brl::GfxModel::loadModel("models/block.glb");
                }
                else
                {
                    block = brl::GfxModel::loadModel("models/block.glb");
                }

                float blockX = x * 2.2f;
                float blockY = y * spacing;

                if (abs(x) % 2 == 1)
                {
                    blockY += 1.3f;
                }

                auto blockEntity = block->createEntity();
                blockEntity->localPosition = {blockX, -2 + (3 * i), blockY};
                blockEntity->setEulerAngles({0, 180, 0});
                blockEntity->localScale = glm::vec3(1.5);
            }
        }
    }

}

void MapController::update()
{
    EcsEntity::update();
}
