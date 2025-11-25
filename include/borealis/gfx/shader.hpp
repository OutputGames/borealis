#if !defined(SHADER_HPP)
#define SHADER_HPP

#include <GLFW/glfw3.h>
#include <borealis/util/util.h>
#include <glad/glad.h>

namespace brl
{
    struct GfxShaderProgram;

    struct GfxShaderValue;
    struct GfxShaderUniform;
    struct GfxTexture;
    struct GfxDrawCall;
    struct GfxAttribBuffer;
    struct GfxMaterial;
    using GfxUniformList = std::unordered_map<GfxShaderUniform*, GfxShaderValue>;

    struct GfxShader
    {
        GfxShader(GLenum type, std::string data);

        void destroy();

    private:
        friend struct GfxShaderProgram;
        unsigned int id;

        bool isInstanced = false;

        GLenum type;
    };

    enum GfxUniformType
    {
        INT            = GL_INT,
        FLOAT          = GL_FLOAT,
        VEC2           = GL_FLOAT_VEC2,
        VEC3           = GL_FLOAT_VEC3,
        VEC4           = GL_FLOAT_VEC4,
        MAT4           = GL_FLOAT_MAT4,
        TEXTURE2D      = GL_SAMPLER_2D,
        TEXTURE2DARRAY = GL_SAMPLER_2D_ARRAY,
        BOOL           = GL_BOOL,
    };

    struct GfxShaderUniform
    {
        std::string name;
        GfxUniformType type;
        int location;
        GfxShaderUniform() = default;
    };

    struct GfxShaderValue
    {
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
        GfxShaderProgram(std::string compositeSource);
        ~GfxShaderProgram();

        void use();

        static GfxShaderProgram* GetDefaultShader();

        GfxShaderUniform* getUniform(std::string name)
        {
            for (int i = 0; i < uniformCount; i++)
            {
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

    private:
        friend struct GfxMaterial;
        friend struct GfxCamera;
        friend struct GfxFramebufferAttachment;
        friend GfxEngine;
        friend struct GfxImage;
        unsigned int id;

        bool instancingEnabled = false;

        GfxShaderUniform* uniforms;
        int uniformCount;

        std::string* textures;
        int textureSlots;

        void process();

    };

    struct GfxMaterial
    {

        GfxMaterial(GfxShaderProgram* shader);

        ~GfxMaterial();

        GfxShaderProgram* getShader() { return shader; }
        void reloadShader(GfxShaderProgram* shader);

        size_t getHash();

        void setInt(std::string name, int value)
        {

            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.intValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setFloat(std::string name, float value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.floatValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec2(std::string name, glm::vec2 value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v2value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec3(std::string name, glm::vec3 value)
        {

            if (shader->getUniform(name) == nullptr)
                return;

            GfxShaderValue val;
            val.v3value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec4(std::string name, glm::vec4 value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setMat4(std::string name, glm::mat4 value)
        {


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

        GfxShaderValue getUniform(std::string name)
        {
            for (const auto& uniform : overrides)
            {
                if (uniform.first->name == name)
                    return uniform.second;
            }

            return {};

        }

        void draw(GfxAttribBuffer* buffer, GfxUniformList runtimeOverrides = {});
        void drawInstanced(std::vector<glm::mat4> transforms, GfxAttribBuffer* gfxBuffer,
                           GfxUniformList runtimeOverrides = {});

    private:
        friend struct GfxMaterialMgr;
        GfxShaderProgram* shader;
        GfxUniformList overrides;
        unsigned int registryIndex = UINT_MAX;

        void setOverride(std::pair<GfxShaderUniform*, GfxShaderValue> pair);
    };

    struct GfxMaterialMgr
    {
    private:
        friend GfxMaterial;

        static GfxMaterialMgr* GetInstance();

        std::vector<GfxMaterial*> material_registry;

        void InsertToRegistry(GfxMaterial* material);
        void RemoveFromRegistry(GfxMaterial* material);

    };

} // namespace brl


#endif // SHADER_HPP
