#if !defined(CAMERA_HPP)
#define CAMERA_HPP

#include <borealis/util/util.h>
#include <borealis/gfx/framebuffer.hpp>
#include <borealis/gfx/engine.hpp>

#include "borealis/ecs/entity.hpp"

namespace brl
{
    struct GfxCanvas;

    enum GfxCameraType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct GfxCamera
    {
        static GfxCamera* mainCamera;

        GfxCamera();

        glm::vec3 position;
        glm::quat rotation;

        float fieldOfView = 45;
        float orthographicSize = 5.0f;

        float minLimit = 0.01f;
        float maxLimit = 100.0f;

        GfxCameraType type = PERSPECTIVE;

        GfxFramebuffer* targetFramebuffer;

        void draw(std::vector<GfxDrawCall>& calls, const GfxInstancedDrawCallList instancedCalls);

    private:
        friend GfxEngine;
        friend GfxCanvas;
        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjMatrix();
        glm::mat4 GetOrthoProjMatrix();
        glm::mat4 GetPerspectiveProjMatrix();
        float getAspectRatio();

        GfxFramebuffer* cachedFramebuffer = nullptr;
    };

    struct EcsCamera : EcsEntity
    {

        float fieldOfView = 75.f;
        float orthographicSize = 5.f;

        float minLimit = 0.001f;
        float maxLimit = 100.f;


        GfxCameraType type = PERSPECTIVE;
        GfxFramebuffer* targetFramebuffer;

    private:
        friend GfxCanvas;
        GfxCamera* gfxCamera = new GfxCamera;

        void earlyUpdate() override;
    };
} // namespace brl


#endif // CAMERA_HPP
