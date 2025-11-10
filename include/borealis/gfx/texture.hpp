#if !defined(TEXTURE_HPP)
#define TEXTURE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxTexture
    {
    private:
        friend struct GfxMaterial;
        friend struct GfxShader;
        friend struct GfxTexture2d;
        friend struct GfxFramebuffer;
        unsigned id = UINT_MAX;
    };

    struct Color32
    {
        unsigned char r,g,b,a;
    };

    struct GfxTexture2d : GfxTexture {

        GfxTexture2d(std::string path);
        GfxTexture2d(Color32* pixels, int width, int height);

        int getWidth() {return width;}
        int getHeight() {return height;}

    private:
        friend struct GfxSprite;
        Color32* pixels;

        int width, height;
    };

} // namespace brl


#endif // TEXTURE_HPP
