#if !defined(SHADER_HPP)
#define SHADER_HPP

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
    using GfxUniformList = std::map<GfxShaderUniform*, GfxShaderValue>;
    
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
        glm::vec2 v2value;
        glm::vec3 v3value;
        glm::vec4 v4value;
        glm::mat4 m4value;
        GfxTexture* txValue;
    };

    struct GfxShaderProgram 
    {
        GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad);

        void use();

    private:
        friend struct GfxMaterial;
        friend struct GfxCamera;
        friend struct GfxFramebufferAttachment;
        unsigned int id;

        GfxShaderUniform* uniforms;
        int uniformCount;

        std::string* textures;
        int textureSlots;

        GfxShaderUniform* getUniform(std::string name) {
            for(int i=0; i<uniformCount; i++){
                if (uniforms[i].name == name)
                {
                    uniforms[i].location = glGetUniformLocation(id, name.c_str());
                    return &uniforms[i];
                }
            }
            return nullptr;
        }

        int getTextureIndex(std::string name)
        {
            for (int i = 0; i < textureSlots; i++)
            {
                if (textures[i] == name)
                {
                    return i;
                }
            }
            return -1;
        }
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
            setOverride({shader->getUniform(name), val});
        }

        void setFloat(std::string name, float value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.floatValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec2(std::string name, glm::vec2 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v2value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec3(std::string name, glm::vec3 value) {

            if (shader->getUniform(name) == nullptr)
                return;

            GfxShaderValue val;
            val.v3value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec4(std::string name, glm::vec4 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setMat4(std::string name, glm::mat4 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.m4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setTexture(std::string name, GfxTexture* value)
        {
            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.txValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void draw(GfxAttribBuffer* buffer, GfxUniformList runtimeOverrides = {});


    private:
        GfxShaderProgram* shader;
        GfxUniformList overrides;

        void setOverride(std::pair<GfxShaderUniform*, GfxShaderValue> pair);
    };
} // namespace brl


#endif // SHADER_HPP
