#if !defined(GFX_HPP)
#define GFX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>
#include <borealis/math/vector.hpp>

namespace brl
{

    struct GfxDrawCall;
    struct AttribGfxBuffer;

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
        int location;
    };

    struct GfxShaderValue {
        int intValue;
        float floatValue;
        vector2 v2value;
        vector3 v3value;
        vector4 v4value;
    };

    struct GfxShaderProgram 
    {
        GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad);

        void use();

    private:
        friend struct GfxMaterial;
        unsigned int id;

        GfxShaderUniform* uniforms;
        int uniformCount;

        GfxShaderUniform* getUniform(std::string name) {
            for(int i=0; i<uniformCount; i++){
                if (uniforms[i].name == name)
                    return &uniforms[i]; 
            }
            return nullptr;
        }
    };

    struct GfxTexture2d {

        GfxTexture2d(std::string path);

        int getWidth() {return width;}
        int getHeight() {return height;}

    private:
        unsigned id;
        int width, height;
    };

    struct GfxMaterial {

        GfxMaterial(GfxShaderProgram* shader) {
            this->shader = shader;
        }

        GfxShaderProgram* getShader() {return shader;}
        
        void setInt(std::string name, int value) {

            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.intValue = value;
            overrides.insert({shader->getUniform(name), val});
        }

        void setFloat(std::string name, float value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.floatValue = value;
            overrides.insert({shader->getUniform(name), val});
        }

        void setVec2(std::string name, vector2 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v2value = value;
            overrides.insert({shader->getUniform(name), val});
        }

        void setVec3(std::string name, vector3 value) {

            if (shader->getUniform(name) == nullptr)
                return;

            GfxShaderValue val;
            val.v3value = value;
            overrides.insert({shader->getUniform(name), val});
        }

        void setVec4(std::string name, vector4 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v4value = value;
            overrides.insert({shader->getUniform(name), val});
        }


        void draw(AttribGfxBuffer* buffer);

    private:
        GfxShaderProgram* shader;
        std::map<GfxShaderUniform*, GfxShaderValue> overrides;
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
