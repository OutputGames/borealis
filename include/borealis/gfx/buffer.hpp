#if !defined(BUFFER_HPP)
#define BUFFER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxShaderValue;
    struct GfxShaderUniform;
    struct GfxTexture;
    struct GfxDrawCall;
    struct GfxAttribBuffer;
    struct GfxMaterial;
    
    struct IGfxBuffer 
    {


        virtual void use() {}
        virtual void destroy() {};
        

    private:
        friend struct GfxBuffer;
        friend struct GfxAttribBuffer;

        IGfxBuffer() {};
        unsigned int id = -1;
    };

    struct GfxBuffer : public IGfxBuffer {
        GfxBuffer(GLenum format);

        /**
         * @brief 
         * This function binds the graphics buffer object for usage.
         */
        void use();

        /**
         * @brief 
         * This function replaces the data in the graphics buffer.
         * @param usage 
         * @param data 
         * @param size 
         */
        void updateData(GLenum usage, const void* data, size_t size);


        /// @brief This function deinitializes and destroys the buffer.
        void destroy();

    private:
        friend struct GfxAttribBuffer;
        
        GLenum format;
        size_t size = 0;
    };

    struct GfxAttribute {
        int size;
        int stride;
        const void* pointer;

        GLenum format = GL_FLOAT;
        bool normalized = false;

        int divisor = -1;
    };

    enum GfxPrimitiveType
    {
        QUAD,
        CIRCLE,
        LINE
    };
 
    struct GfxAttribBuffer : public IGfxBuffer {
        GfxAttribBuffer();
        ~GfxAttribBuffer();

        void assignBuffer(GfxBuffer* buffer);
        void assignElementBuffer(GfxBuffer* buffer, GLenum format);
        void insertAttribute(GfxAttribute attribute, GfxBuffer* bufferToUse = nullptr);

        void ensureReadyForInstancing();
        void updateInstanceBuffer(std::vector<glm::mat4> instances);

        void use();
        void destroy();

        int getSize();

        size_t getHash();

        GLenum mode = GL_TRIANGLES;

    private:
        friend GfxMaterial;
        GfxBuffer* vbo = nullptr, *ebo = nullptr;
        GLenum eboFormat = GL_UNSIGNED_INT;
        int attributeCount = 0;
        size_t vertexSize;

        bool setupAttributesForInstancing = false;
        GfxBuffer* instanceBuffer;

    }; // <-- Add a semicolon here to terminate the struct definition


} // namespace brl


#endif // BUFFER_HPP
