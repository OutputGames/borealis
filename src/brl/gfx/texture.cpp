#include "borealis/gfx/gfx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

auto cachedTextures = std::map<std::string, brl::GfxTexture2d*>();

brl::GfxTexture2d* whiteTexture = nullptr;


brl::GfxTexture::~GfxTexture()
{
    glDeleteTextures(1, &id);
}

brl::GfxTexture* brl::GfxTexture::clone() const
{
    const auto& tex = new GfxTexture;

    tex->id = id;
    tex->width = width;
    tex->height = height;

    return tex;
}

brl::GfxTexture2d::GfxTexture2d(std::string path)
{
    cachedTextures.insert_or_assign(path, this);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int nrChannels;

    IoFile file = readFileBinary(path);


    unsigned char* data = stbi_load_from_memory(file.data, file.dataSize, &width, &height, &nrChannels, 4);

    file.free(true);

    if (data)
    {
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;

        switch (nrChannels)
        {
            case 1:
                format = GL_R;
                internalFormat = GL_R8;
                break;
            case 2:
                format = GL_RG;
                internalFormat = GL_RG8;
                break;
            case 3:
                format = GL_RGBA;
                internalFormat = GL_RGBA;
                break;
            case 4:
                format = GL_RGBA;
                internalFormat = GL_RGBA8;
        }


        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        pixels = new Color32[width * height * 4];
        for (int i = 0; i < width * height * 4; i += 4)
        {
            Color32 pix;
            unsigned char r = data[i]; // Red component
            unsigned char g = data[i + 1]; // Green component
            unsigned char b = data[i + 2]; // Blue component
            unsigned char a = data[i + 3]; // Alpha component
            pixels[i / 4] = Color32{r, g, b, a};
        }
    }
    else
    {
        std::cout << "Failed to load texture at" << path << std::endl;
        exit(-1);
    }
    stbi_image_free(data);
}

brl::GfxTexture2d* brl::GfxTexture2d::loadTexture(std::string path)
{
    if (cachedTextures.contains(path))
        return cachedTextures[path];

    return new GfxTexture2d(path);
}

brl::GfxTexture2d::GfxTexture2d(Color32* pixels, int width, int height)
{
    this->width = width;
    this->height = height;
    this->pixels = pixels;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    delete[] pixels;
}

brl::GfxTexture2d* brl::GfxTexture2d::getWhiteTexture()
{

    if (!whiteTexture)
    {
        auto c = new Color32[4];

        c[0] = {255, 255, 255, 255};
        c[1] = {255, 255, 255, 255};
        c[2] = {255, 255, 255, 255};
        c[3] = {255, 255, 255, 255};

        whiteTexture = new GfxTexture2d(c, 2, 2);
    }

    return whiteTexture;

}

brl::GfxTexture2d::~GfxTexture2d()
{
    GfxTexture::~GfxTexture();
    if (pixels)
    {

        delete[] pixels;
        pixels = nullptr;
    }
    for (const auto& cached_texture : cachedTextures)
    {
        if (cached_texture.second == this)
        {
            cachedTextures.erase(std::find(cachedTextures.begin(), cachedTextures.end(), cached_texture));
            break;
        }
    }

}

brl::GfxTexture2dArray::GfxTexture2dArray(Color32* pixels, int width, int height, int layerCount)
{
    this->width = width;
    this->height = height;
    this->pixels = pixels;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA8; // Changed from GL_RGBA

    // Calculate number of mipmap levels
    int mipLevels = 1 + floor(log2(std::max(width, height)));

    // Allocate storage with proper mipmap levels
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, internalFormat, width, height, layerCount);

    // Upload the pixel data
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, format, GL_UNSIGNED_BYTE, pixels);

    delete[] pixels;
}
