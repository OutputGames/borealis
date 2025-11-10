#include "borealis/gfx/sprite.hpp"

brl::GfxSprite::GfxSprite()
{

}

brl::GfxTexture2d** brl::GfxSprite::extractSprites(GfxTexture2d* tex, int spriteWidth, int spriteHeight)
{
    int spriteCountX = tex->width / spriteWidth;
    int spriteCountY = tex->height / spriteHeight;

    auto textures = new GfxTexture2d*[spriteCountX * spriteCountY];

    int offset = 0;
    for (int x = 0; x < spriteCountX; ++x)
    {
        for (int y = 0; y < spriteCountY; ++y)
        {
            auto spritePixels = new Color32[spriteWidth * spriteHeight];
            for (int i = offset; i < offset+(spriteWidth * spriteHeight); i++)
            {
                spritePixels[i - offset] = tex->pixels[i];
            }

            textures[(x * spriteCountX) + y] = new GfxTexture2d(spritePixels, spriteWidth, spriteHeight);

            offset += spriteWidth * spriteHeight;
        }
    }

    return textures;
}
