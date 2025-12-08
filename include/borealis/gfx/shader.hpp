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
        int count;
        GfxShaderUniform() = default;
    };

    struct GfxShaderValue
    {
        int intValue;
        float floatValue;
        glm::vec2 v2value;
        glm::vec3 v3value;
        glm::vec4 v4value;
        std::shared_ptr<std::vector<glm::mat4>> m4value = nullptr;
        GfxTexture* txValue = nullptr;
    };

    struct GfxShaderBinding 
    {
        brl::GfxShaderUniform* uniform = nullptr;
        brl::GfxShaderValue* value = nullptr;

        GfxShaderBinding() = default;
        GfxShaderBinding(brl::GfxShaderUniform* u, brl::GfxShaderValue* v) : uniform(u), value(v) {}

        ~GfxShaderBinding();
    };

    struct GfxUniformList : public std::vector<GfxShaderBinding>
    {
        bool contains(GfxShaderUniform* uniform);

        

// Non-const version for read/write access
        GfxShaderValue*& operator[](GfxShaderUniform* uniform)
        {
            for (auto& element1 : *this) // Non-const loop for non-const method
            {
                if (element1.uniform == uniform)
                    return element1.value;
            }

            // Not found - throw exception or add default behavior
            throw std::out_of_range("Uniform not found");
        }

        // Const version for read-only access
        GfxShaderValue* const& operator[](GfxShaderUniform* uniform) const
        {
            for (const auto& element1 : *this)
            {
                if (element1.uniform == uniform)
                    return element1.value;
            }

            throw std::out_of_range("Uniform not found");
        }
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

            GfxShaderValue* val = new GfxShaderValue();
            val->intValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setFloat(std::string name, float value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue* val = new GfxShaderValue();
            val->floatValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec2(std::string name, glm::vec2 value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue* val = new GfxShaderValue();
            val->v2value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec3(std::string name, glm::vec3 value)
        {

            if (shader->getUniform(name) == nullptr)
                return;

            GfxShaderValue* val = new GfxShaderValue();
            val->v3value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec4(std::string name, glm::vec4 value)
        {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue* val = new GfxShaderValue();
            val->v4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setMat4(std::string name, glm::mat4 value, int index=0)
        {
            const auto uniform = shader->getUniform(name);

            if (!uniform)
                return;

            if (overrides.contains(uniform))
            {
                overrides[uniform]->m4value->operator[](index) = value;
            }
            else
            {

                GfxShaderValue* val = new GfxShaderValue();
                val->m4value->reserve(uniform->count);
                val->m4value->operator[](index) = value;
                setOverride({uniform, val});
            }
        }

        void setMat4(std::string name, std::vector<glm::mat4>& value)
        {
            const auto uniform = shader->getUniform(name);

            if (!uniform)
                return;

            if (overrides.contains(uniform))
            {
                overrides[uniform]->m4value = std::make_shared<std::vector<glm::mat4>>(value);
            }
            else
            {

                GfxShaderValue* val = new GfxShaderValue();
                val->m4value = std::make_shared<std::vector<glm::mat4>>(value);
                setOverride({uniform, val});
            }
        }

        void setTexture(std::string name, GfxTexture* value)
        {
            if (!shader->getUniform(name))
                return;

            GfxShaderValue* val = new GfxShaderValue();
            val->txValue = value;
            setOverride({shader->getUniform(name), val});
        }

        GfxShaderValue* getUniform(std::string name)
        {
            for (const auto& uniform : overrides)
            {
                if (uniform.uniform->name == name)
                    return uniform.value;
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

        void setOverride(GfxShaderBinding pair);
    };

    struct GfxMaterialMgr
    {
    private:
        friend GfxMaterial;

        static GfxMaterialMgr* GetInstance();

        std::vector<GfxMaterial*> material_registry;

        void InsertToRegistry(GfxMaterial* material);
        void RemoveFromRegistry(GfxMaterial* material);
        ~GfxMaterialMgr();

    };

    
    struct GfxDrawCall
    {
        GfxMaterial* material;
        GfxAttribBuffer* gfxBuffer;
        const glm::mat4 transform;
        GfxUniformList uniqueOverrides;
    };

    struct GfxInstancedDrawCall
    {
        GfxMaterial* material;
        std::vector<glm::mat4> transforms;
        GfxAttribBuffer* gfxBuffer;
    };

} // namespace brl


#endif // SHADER_HPP
