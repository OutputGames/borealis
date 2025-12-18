#if !defined(UI_HPP)
#define UI_HPP


#include "buffer.hpp"
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

                static GfxCanvas* mainCanvas;

    private:
        friend struct GfxImage;
        friend struct GfxUIElement;
        friend struct GfxTextRenderer;
        friend GfxEngine;


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

    struct GfxFont
    {
        struct GfxFontCharacter : GfxTexture
        {
            GfxFontCharacter() = default;
            GfxFontCharacter(unsigned int id, glm::vec2 s, glm::vec2 b, unsigned int adv);
            glm::vec2 size; // Size of glyph
            glm::vec2 bearing; // Offset from baseline to left/top of glyph
            unsigned int advanceOffset; // Offset to advance to next glyph

            GfxAttribBuffer* charVAO = nullptr;
            GfxBuffer* charVBO = nullptr;
        };

        std::map<char, GfxFontCharacter*> characters;
        int fontSize = 48;

        GfxFont(std::string path);

    };

    struct GfxTextRenderer : GfxUIElement
    {
        GfxTextRenderer(GfxCanvas* c);

        GfxFont* font = nullptr;

        std::string text;

        void start() override;
        void debugPrintCharacterInfo();
        void lateUpdate() override;

    private:


            // Material pool for text rendering with different colors
        struct TextMaterialKey
        {
            glm::vec3 color;
            GfxFont::GfxFontCharacter* textureID;

            bool operator<(const TextMaterialKey& other) const;
        };
        std::map<TextMaterialKey, GfxMaterial*> materialCache;
        // Helper to get or create text material with specific color and texture
        GfxMaterial* getMaterial(glm::vec3 color, GfxFont::GfxFontCharacter* textureID);
    // Helper to create quad geometry for a single character
        void createCharacterQuad(const GfxFont::GfxFontCharacter* ch, glm::vec2 position, float scale, float vertices[6][4]);
    };

} // namespace brl


#endif // UI_HPP
