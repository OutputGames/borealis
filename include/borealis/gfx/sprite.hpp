#if !defined(SPRITE_HPP)
#define SPRITE_HPP
#include "texture.hpp"

namespace brl
{
    
    struct GfxSprite
    {
        GfxSprite();
        static GfxTexture2d** extractSpritesToTextures(GfxTexture2d* tex, int spriteWidth, int spriteHeight, bool deleteTexture);
        static GfxTexture2dArray* extractSpritesToArray(GfxTexture2d* tex, int spriteWidth, int spriteHeight, bool deleteTextures);

    private:
    };

} // namespace brl


#endif // SPRITE_HPP
