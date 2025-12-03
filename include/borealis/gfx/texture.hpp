#if !defined(TEXTURE_HPP)
#define TEXTURE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxTexture
    {
        int getWidth() { return width; }
        int getHeight() { return height; }

        ~GfxTexture();


    private:
        friend struct GfxMaterial;
        friend struct GfxShader;
        friend struct GfxSprite;
        friend struct GfxTexture2d;
        friend struct GfxTexture2dArray;
        friend struct GfxFramebuffer;

        unsigned id = UINT_MAX;
        int width, height;
    };

    #pragma pack(push, 1)
    struct Color32 {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    #pragma pack(pop)

    struct GfxTexture2d : GfxTexture {

        static GfxTexture2d* loadTexture(std::string path);

        GfxTexture2d(Color32* pixels, int width, int height);

        static GfxTexture2d* getWhiteTexture();

        ~GfxTexture2d();

        void clearCachedPixels() {
            if (pixels)
            {
                delete[] pixels;
                pixels = nullptr;
            }
        }

    private:

        
        GfxTexture2d(std::string path);

        friend struct GfxSprite;
        Color32* pixels;
    };


    struct GfxTexture2dArray : GfxTexture
    {

        GfxTexture2dArray(Color32* pixels, int width, int height, int layerCount);

    private:
        friend struct GfxSprite;
        Color32* pixels;
    };

} // namespace brl


#endif // TEXTURE_HPP
