#include "borealis/gfx/sprite.hpp"

brl::GfxSprite::GfxSprite()
{

}

brl::GfxTexture2d** brl::GfxSprite::extractSpritesToTextures(GfxTexture2d* tex, int spriteWidth, int spriteHeight, bool deleteTexture)
{
    int spriteCountX = tex->width / spriteWidth;
    int spriteCountY = tex->height / spriteHeight;

    auto textures = new GfxTexture2d*[spriteCountX * spriteCountY];

    for (int y = 0; y < spriteCountY; ++y)
    {
        for (int x = 0; x < spriteCountX; ++x)
        {
            auto spritePixels = new Color32[spriteWidth * spriteHeight];

            // Copy row by row for this sprite
            for (int row = 0; row < spriteHeight; ++row)
            {
                // Start position in the full texture
                int srcY = y * spriteHeight + row;
                int srcX = x * spriteWidth;

                int srcOffset = srcY * tex->width + srcX;
                int dstOffset = row * spriteWidth;

                // Copy the row�s pixels
                memcpy(&spritePixels[dstOffset], &tex->pixels[srcOffset], spriteWidth * sizeof(Color32));
            }

            // Store sprite texture
            textures[y * spriteCountX + x] = new GfxTexture2d(spritePixels, spriteWidth, spriteHeight);
        }
    }

    if (deleteTexture)
        delete tex;

    return textures;
}

brl::GfxTexture2dArray* brl::GfxSprite::extractSpritesToArray(GfxTexture2d* tex, int spriteWidth, int spriteHeight, bool deleteTextures)
{
    int spriteCountX = tex->width / spriteWidth;
    int spriteCountY = tex->height / spriteHeight;
    int totalSprites = spriteCountX * spriteCountY;
    auto spritePixels = new Color32[spriteWidth * spriteHeight * totalSprites];

    int spriteIndex = 0;
    for (int y = 0; y < spriteCountY; ++y)
    {
        for (int x = 0; x < spriteCountX; ++x)
        {
            // Calculate the base offset for this sprite in the destination array
            int spriteOffset = spriteIndex * spriteWidth * spriteHeight;

            // Copy row by row for this sprite
            for (int row = 0; row < spriteHeight; ++row)
            {
                // Start position in the full texture
                int srcY = y * spriteHeight + row;
                int srcX = x * spriteWidth;
                int srcOffset = srcY * tex->width + srcX;

                // Destination position in the sprite array
                int dstOffset = spriteOffset + row * spriteWidth;

                // Copy the row's pixels
                memcpy(&spritePixels[dstOffset], &tex->pixels[srcOffset], spriteWidth * sizeof(Color32));
            }
            spriteIndex++;
        }
    }
    auto array = new GfxTexture2dArray(spritePixels, spriteWidth, spriteHeight, totalSprites);

    if (deleteTextures)
        delete tex;

    return array;
}
