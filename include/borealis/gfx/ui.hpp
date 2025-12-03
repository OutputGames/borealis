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

        enum class ScaleMode
        {
            ConstantPixelSize,
            ScaleWithScreenSize,
            ConstantPhysicalSize
        };

        ScaleMode mode = ScaleMode::ScaleWithScreenSize;

        glm::vec2 referenceResolution;
        float referencePixelsPerUnit = 100.0f;
        float referenceDPI = 96.0f;

        float matchWidthOrHeight = 0.5f;

        GfxCanvas();
        void earlyUpdate() override;
        void lateUpdate() override;

    private:
        friend struct GfxImage;
        friend struct GfxUIElement;
        friend GfxEngine;

        static GfxCanvas* mainCanvas;

        void insertDrawCall(const GfxUIDrawCall call);

        GfxCamera* cachedCamera = nullptr;

        void draw();

        float GetScaleFactor(glm::vec2 screenResolution, float screenDPI = 96.0f)
        {
            switch (mode)
            {
                case ScaleMode::ConstantPixelSize:
                    return 1.0f;

                case ScaleMode::ScaleWithScreenSize:
                    {
                        float logWidth = std::log(screenResolution.x / referenceResolution.x);
                        float logHeight = std::log(screenResolution.y / referenceResolution.y);
                        float logScale = glm::mix(logWidth, logHeight, matchWidthOrHeight);
                        return std::exp(logScale);
                    }

                case ScaleMode::ConstantPhysicalSize:
                    return screenDPI / referenceDPI;
            }
            return 1.0f;
        }

        std::vector<GfxUIDrawCall> draw_calls;

        glm::vec2 ScaleUICoordinates(glm::vec2 uiPos, glm::vec2 screenResolution, float screenDPI = 96.0f)
        {
            float scale = GetScaleFactor(screenResolution, screenDPI);
            return uiPos * scale;
        }

        glm::vec2 GetScaledCanvasSize(glm::vec2 screenResolution, float screenDPI = 96.0f)
        {
            float scale = GetScaleFactor(screenResolution, screenDPI);
            return screenResolution / scale; // Returns virtual resolution
        }
    };

    struct GfxUIRect
    {
        glm::vec2 position; // bottom-left corner
        glm::vec2 size;
    };


    struct GfxUIElement : EcsEntity
    {
        GfxUIElement(GfxCanvas* c);

        glm::mat4 calculateTransform() override;

    protected:
        GfxCanvas* canvas;


    };
    
    struct GfxImage : GfxUIElement
    {
        GfxImage(GfxCanvas* c);

        glm::vec3 color = glm::vec3(1.0);
        GfxTexture2d* texture = GfxTexture2d::getWhiteTexture();
        GfxMaterial* material = new GfxMaterial(new GfxShaderProgram(brl::readFileString("D:/shaders/defaultui.glsl")));

        void loadTexture(GfxTexture2d* tex);

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
