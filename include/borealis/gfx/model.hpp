#if !defined(MODEL_HPP)
#define MODEL_HPP

#include <borealis/util/util.h>

#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "borealis/ecs/entity.hpp"

struct aiMaterial;
struct aiScene;
struct aiNode;

namespace brl
{
    struct GfxModelNode;
    struct GfxMeshRenderer;
    struct GfxModel;

    struct GfxVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct GfxMesh {
        std::string name;
        int materialIndex;
    private:
        friend GfxModel;
        friend GfxModelNode;
        GfxAttribBuffer* buffer;
    };

    struct GfxModelNode {
        std::string name;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        unsigned int* meshIndices;
        GfxModelNode** children;

        int meshCount;
        int childCount;

        EcsEntity* createEntity();

    private:
        friend GfxModel;
        GfxModel* model;
    };

    struct GfxMaterialDescription
    {
        struct GfxMaterialKey
        {
            std::string id;
            unsigned int type;
            unsigned int idx;

            bool operator<(const GfxMaterialKey& other) const
            {
                if (id != other.id)
                    return id < other.id;
                if (type != other.type)
                    return type < other.type;
                return idx < other.idx;
            }
        };

        std::string name;

        std::map<GfxMaterialKey, glm::vec3> color_uniforms;
        std::map<unsigned, GfxTexture2d*> texture_uniforms;

        GfxMaterial* createMaterial(GfxShaderProgram* shader);

    private:
        friend GfxModel;

        void deriveColor(aiMaterial* material, std::string str, unsigned type, unsigned idx);
        void deriveTexture(aiMaterial* material, unsigned type, const aiScene* scn=nullptr);
    };

    struct GfxModel {
        std::vector<GfxMesh*> meshes;
        std::vector<GfxTexture2d*> textures;
        std::vector<GfxMaterialDescription*> materials;
        
        GfxModel(std::string path);
        EcsEntity* createEntity();

    private:
        GfxModelNode* rootNode;

        GfxModelNode* processNode(aiNode* node, const aiScene* scene);
    };

    struct GfxMeshRenderer : EcsEntity
    {
        GfxAttribBuffer* mesh;
        GfxMaterial* material;
        void lateUpdate() override;
    };


} // namespace brl


#endif // MODEL_HPP
