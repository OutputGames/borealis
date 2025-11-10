#include "borealis/gfx/gfx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

brl::GfxTexture2d::GfxTexture2d(std::string path)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;

    IoFile file = readFileBinary(path);


    unsigned char* data = stbi_load_from_memory(file.data, file.dataSize, &width, &height, &nrChannels, 4);
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
        glGenerateMipmap(GL_TEXTURE_2D);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
}
