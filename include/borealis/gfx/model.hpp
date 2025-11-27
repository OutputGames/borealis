#if !defined(MODEL_HPP)
#define MODEL_HPP

#include <borealis/util/util.h>

#include "buffer.hpp"
#include "engine.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "borealis/ecs/entity.hpp"

namespace tinygltf
{
    class Model;
    struct Material;
    class Node;
}

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

    struct GfxSubMesh
    {
        GfxAttribBuffer* buffer;
        int materialIndex;

        ~GfxSubMesh();
    };

    struct GfxMesh {
        std::string name;
        static GfxMesh* GetPrimitive(GfxPrimitiveType type);

        GfxSubMesh* GetSubMesh(int index) { return subMeshes[index]; }
        int GetSubMeshCount() { return subMeshCount; }

        GfxMesh() = default;
        GfxMesh(GfxAttribBuffer* buffer);

        ~GfxMesh();

    private:
        friend GfxModel;
        friend GfxModelNode;
        friend GfxMeshRenderer;
        GfxSubMesh** subMeshes;
        int subMeshCount;
    };

    struct GfxModelNode {
        std::string name;
        glm::vec3 position = glm::vec3(0);
        glm::quat rotation = glm::quat(); 
        glm::vec3 scale = glm::vec3(1);

        int mesh = -1;
        GfxModelNode** children;

        int childCount = 0;

        EcsEntity* createEntity();

    private:
        friend GfxModel;
        GfxModel* model;
    };

    struct GfxMaterialDescription
    {

        std::string name;

        GfxTexture2d* baseColorTexture = nullptr;
        glm::vec4 baseColorValue;

        GfxMaterial* createMaterial(GfxShaderProgram* shader);

    private:
        friend GfxModel;
    };

    struct GfxModel {
        std::vector<GfxMesh*> meshes;
        std::vector<GfxTexture2d*> textures;
        std::vector<GfxMaterialDescription*> materialDescriptions;
        std::vector<GfxMaterial*> materials;

        static GfxModel* loadModel(std::string path);

        EcsEntity* createEntity();

    private:
        friend GfxEngine;

        GfxModel(std::string path);
        GfxModelNode* rootNode;

        GfxModelNode* processNode(tinygltf::Node node, tinygltf::Model scene);

        static std::map<std::string, brl::GfxModel*> cachedModels;
    };

    struct GfxMeshRenderer : EcsEntity
    {
        GfxMesh* mesh = nullptr;

        GfxMeshRenderer();

        std::vector<GfxMaterial*> materials;
        uint32_t instancingID = 0;

        GfxMaterial* material() { return materials[0]; }
        void setMaterial(GfxMaterial* material)
        {
            materials[0] = material;
        }


        void lateUpdate() override;
    };


} // namespace brl


#endif // MODEL_HPP
