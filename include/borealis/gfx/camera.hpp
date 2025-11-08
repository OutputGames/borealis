#if !defined(CAMERA_HPP)
#define CAMERA_HPP

#include <borealis/util/util.h>
#include <borealis/gfx/framebuffer.hpp>
#include <borealis/gfx/engine.hpp>

#include "borealis/ecs/entity.hpp"

namespace brl
{
    struct GfxCamera
    {
        static GfxCamera* mainCamera;

        GfxCamera();

        glm::vec3 position;
        glm::quat rotation;

        float fieldOfView = 45;
        float minLimit = 0.01f;
        float maxLimit = 100.0f;

        GfxFramebuffer* targetFramebuffer;

        void draw(const std::vector<GfxDrawCall>& calls);

    private:
        friend GfxEngine;
        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjMatrix();
        float getAspectRatio();

        GfxFramebuffer* cachedFramebuffer = nullptr;
    };

    struct EcsCamera : EcsEntity
    {

        float fieldOfView = 75.f;

        float minLimit = 0.01f;
        float maxLimit = 100.f;

    private:
        GfxCamera* gfxCamera = new GfxCamera;

        void earlyUpdate() override;
    };
} // namespace brl


#endif // CAMERA_HPP
