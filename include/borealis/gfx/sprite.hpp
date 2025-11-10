#if !defined(SPRITE_HPP)
#define SPRITE_HPP
#include "texture.hpp"

namespace brl
{
    
    struct GfxSprite
    {
        GfxSprite();
        static GfxTexture2d** extractSprites(GfxTexture2d* tex, int spriteWidth, int spriteHeight);

    private:
    };

} // namespace brl


#endif // SPRITE_HPP
