#if !defined(FRAMEBUFFER_HPP)
#define FRAMEBUFFER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>
#include <borealis/gfx/texture.hpp>

namespace brl
{
    struct GfxShaderValue;
    struct GfxShaderUniform;
    struct GfxTexture;
    struct GfxDrawCall;
    struct GfxAttribBuffer;
    struct GfxMaterial;

    struct GfxFramebufferAttachment : GfxTexture
    {
        GLenum format, internalFormat;
        GLenum type;

        void draw(GfxMaterial* material);

    private:

        static GfxAttribBuffer* fullscreenQuadBuffer;

    };

    struct GfxFramebuffer
    {

        GfxFramebuffer(int width, int height, GfxFramebufferAttachment** attachments = nullptr, int attachmentCount = -1);

        void use();
        void clear();
        ~GfxFramebuffer();

        GfxFramebufferAttachment* getAttachment(int i );
        float getAspectRatio() { return (float)width / (float)height; }

    private:
        int width, height;
        GfxFramebufferAttachment** attachments;
        int attachmentCount;

        unsigned id;
        unsigned rbo;
    };

} // namespace brl


#endif // FRAMEBUFFER_HPP
