#if !defined(UI_HPP)
#define UI_HPP


#include "borealis/ecs/entity.hpp"
#include "borealis/gfx/camera.hpp"
#include "borealis/gfx/shader.hpp"

namespace brl
{
    struct GfxUIDrawCall;

    struct GfxCanvas : EcsEntity
    {

        EcsCamera* targetedCamera = nullptr;
        glm::vec2 scaledSize;

        GfxCanvas();
        void earlyUpdate() override;
        void lateUpdate() override;

    private:
        friend struct GfxImage;
        friend GfxEngine;

        static GfxCanvas* mainCanvas;

        void insertDrawCall(const GfxUIDrawCall call);

        GfxCamera* cachedCamera = nullptr;

        void draw();

        std::vector<GfxUIDrawCall> draw_calls;
    };

    struct GfxUIElement : EcsEntity
    {
        GfxUIElement();
        void start() override;


    protected:
        GfxCanvas* canvas;
    };
    
    struct GfxImage : GfxUIElement
    {
        GfxTexture2d* texture;
        GfxMaterial* material = new GfxMaterial(new GfxShaderProgram(brl::readFileString("D:/shaders/defaultui.glsl")));

                void lateUpdate() override;
    };

    struct GfxUIDrawCall
    {
        glm::mat4 transform;
        GfxAttribBuffer* gfxBuffer;
        GfxMaterial* material;
    };

} // namespace brl


#endif // UI_HPP
