#if !defined(GFX_HPP)
#define GFX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{

    struct GfxDrawCall;

    struct GfxWindow {
        bool isOpen() { return !glfwWindowShouldClose((GLFWwindow*)window); }
    private:
        friend struct GfxEngine;
        void* window;

        int width, height;

        GfxWindow(int w, int h, const char* title);

        void pollEvents();
        void swapBuffers();
        void clear();

        int getWidth();
        int getHeight();
    };
    
    struct GfxEngine
    {
        GfxEngine();
        ~GfxEngine();

        void initialize();
        void shutdown();
        void update();
        bool isRunning() {return mainWindow->isOpen();}
        void insertCall(GfxDrawCall* call);
        
    private:
        bool initialized = false;
        GfxWindow* mainWindow = nullptr;
        std::vector<GfxDrawCall*> calls;
    };
    
    struct GfxShader {
        GfxShader(GLenum format,const void* binary, size_t binarySize) = delete;
        GfxShader(GLenum type,std::string data);

        void destroy();

    private:
        friend struct GfxShaderProgram;
        unsigned int id;
        GLenum type;
    };

    struct GfxShaderUniform {
        std::string name;
        GLenum type;
    };

    union GfxShaderValue {
        int intValue;
        float floatValue;
    };

    struct GfxShaderProgram 
    {
        GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad);

        void use();

    private:
        unsigned int id;
        GfxShaderUniform* uniforms;
    };

    struct GfxMaterial {

        GfxShaderProgram* getShader() {return shader;}
        
        void setInt(std::string name, int value) {
            GfxShaderValue val;
            val.intValue = value;
            overrides.insert({name, val});
        }

        void setFloat(std::string name, float value) {
            GfxShaderValue val;
            val.floatValue = value;
            overrides.insert({name, val});
        }

        void draw(AttribGfxBuffer* buffer);

    private:
        GfxShaderProgram* shader;
        std::map<std::string, GfxShaderValue> overrides;
    };

    struct IGfxBuffer 
    {


        virtual void use() {}
        virtual void destroy() {};
        

    private:
        friend struct GfxBuffer;
        friend struct AttribGfxBuffer;

        IGfxBuffer() {};
        unsigned int id = -1;
    };

    struct GfxBuffer : public IGfxBuffer {
        GfxBuffer(GLenum format);

        void use();
        void updateData(GLenum usage, const void* data, size_t size);
        void destroy();

    private:
        friend struct AttribGfxBuffer;
        GLenum format;
        size_t size;
    };

    struct GfxAttribute {
        int size;
        int stride;
        const void* pointer;

        GLenum format = GL_FLOAT;
        bool normalized = false;
    };

    struct AttribGfxBuffer : public IGfxBuffer {
        AttribGfxBuffer();

        void assignBuffer(GfxBuffer* buffer);
        void assignElementBuffer(GfxBuffer* buffer, GLenum format);
        void insertAttribute(GfxAttribute attribute);

        void use();
        void destroy();

        int getSize();

    private:
        friend GfxMaterial;
        GfxBuffer* vbo, *ebo;
        GLenum eboFormat = GL_UNSIGNED_INT;
        int attributeCount = 0;
        size_t vertexSize;
    };

    struct GfxDrawCall {
        GfxMaterial* material;
        AttribGfxBuffer* gfxBuffer;
    };

} // namespace 


#endif // GFX_HPP
