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

    struct GfxTexture2d : GfxTexture {

        GfxTexture2d(std::string path);

        int getWidth() {return width;}
        int getHeight() {return height;}

    private:

        int width, height;
    };

} // namespace brl


#endif // TEXTURE_HPP
