#if !defined(SHADER_HPP)
#define SHADER_HPP

#include <GLFW/glfw3.h>
#include <borealis/util/util.h>
#include <glad/glad.h>

#include <memory>
#include <vector>

#include "ui.hpp"

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
        std::shared_ptr<std::vector<glm::mat4>> m4value; // Shared ownership
        GfxTexture* txValue = nullptr;

        ~GfxShaderValue();
    };

    struct GfxShaderBinding 
    {
        brl::GfxShaderUniform* uniform = nullptr;
        std::shared_ptr<GfxShaderValue> value = nullptr;

        GfxShaderBinding() = default;
        GfxShaderBinding(brl::GfxShaderUniform* u, std::shared_ptr<GfxShaderValue> v) : uniform(u), value(v) {}

        ~GfxShaderBinding();

    private:
        bool real = true;
    };

    struct GfxUniformList :  std::vector<GfxShaderBinding>
    {
        bool contains(GfxShaderUniform* uniform);

// Non-const version for read/write access
        std::shared_ptr<GfxShaderValue>& operator[](GfxShaderUniform* uniform)
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
        std::shared_ptr<GfxShaderValue> const& operator[](GfxShaderUniform* uniform) const
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

#define CONVERT_UNIFORM_MAT4(m) std::make_shared<std::vector<glm::mat4>>(m)

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

            auto val = std::make_shared<GfxShaderValue>();
            val->intValue = value;

            auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(std::move(override));
        }

        void setFloat(std::string name, float value)
        {


            if (!shader->getUniform(name))
                return;

            auto val = std::make_shared<GfxShaderValue>();
            val->floatValue = value;

                        auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(std::move(override));
        }

        void setVec2(std::string name, glm::vec2 value)
        {


            if (!shader->getUniform(name))
                return;

            auto val = std::make_shared<GfxShaderValue>();
            val->v2value = value;

                        auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(std::move(override));
        }

        void setVec3(std::string name, glm::vec3 value)
        {

            if (shader->getUniform(name) == nullptr)
                return;

            auto val = std::make_shared<GfxShaderValue>();
            val->v3value = value;

                        auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(std::move(override));
        }

        void setVec4(std::string name, glm::vec4 value)
        {


            if (!shader->getUniform(name))
                return;

            auto val = std::make_shared<GfxShaderValue>();
            val->v4value = value;

                        auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(std::move(override));
        }

        void setMat4(std::string name, glm::mat4 value, int index=0)
        {
            const auto uniform = shader->getUniform(name);

            if (!uniform)
                return;

            if (overrides.contains(uniform))
            {
                overrides[uniform]->m4value->at(index) = value;
            }
            else
            {

                auto val = std::make_shared<GfxShaderValue>();
                val->m4value->reserve(uniform->count);
                val->m4value->at(index) = value;

                auto override = GfxShaderBinding(uniform, val);
                setOverride((override));
            }
        }

        void setMat4(std::string name, std::vector<glm::mat4>& value)
        {
            const auto uniform = shader->getUniform(name);

            if (!uniform)
                return;

            if (overrides.contains(uniform))
            {
                overrides[uniform]->m4value = CONVERT_UNIFORM_MAT4(value);
            }
            else
            {

                auto val = std::make_shared<GfxShaderValue>();
                val->m4value = CONVERT_UNIFORM_MAT4(value);
                ;
                auto override = GfxShaderBinding(uniform, val);
                setOverride(std::move(override));
            }
        }

        void setTexture(std::string name, GfxTexture* value)
        {
            if (!shader->getUniform(name))
                return;

            auto val = std::make_shared<GfxShaderValue>();
            val->txValue = value;

            auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(override);
        }

        void setTexture(std::string name, const GfxTexture* value)
        {
            if (!shader->getUniform(name))
                return;

            auto val = std::make_shared<GfxShaderValue>();
            //val->txValue = const_cast<GfxTexture*>(value);

            auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(override);
        }

        GfxShaderValue* getUniform(std::string name)
        {
            if (!shader->getUniform(name))
                return nullptr;

            for (const auto& uniform : overrides)
            {
                if (uniform.uniform->name == name)
                    return uniform.value.get();
            }

            auto val = std::make_shared<GfxShaderValue>();


            auto override = GfxShaderBinding(shader->getUniform(name), val);
            setOverride(override);

            return val.get();

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
        void applyUniform(const brl::GfxShaderBinding& _override, brl::GfxShaderProgram* shader);
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
