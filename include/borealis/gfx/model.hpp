#if !defined(MODEL_HPP)
#define MODEL_HPP

#include <borealis/util/util.h>

#include "buffer.hpp"
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
    };

    struct GfxMesh {
        std::string name;
        static GfxMesh* GetPrimitive(GfxPrimitiveType type);

        GfxMesh() = default;
        GfxMesh(GfxAttribBuffer* buffer);

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

        int mesh;
        GfxModelNode** children;

        int childCount;

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
        std::vector<GfxMaterialDescription*> materials;

        static GfxModel* loadModel(std::string path);

        EcsEntity* createEntity();

    private:
        GfxModel(std::string path);
        GfxModelNode* rootNode;

        GfxModelNode* processNode(tinygltf::Node node, tinygltf::Model scene);
    };

    struct GfxMeshRenderer : EcsEntity
    {
        GfxMesh* mesh = nullptr;

        GfxMeshRenderer();

        std::vector<GfxMaterial*> materials;

        GfxMaterial* material() { return materials[0]; }
        void setMaterial(GfxMaterial* material)
        {
            materials[0] = material;
        }


        void lateUpdate() override;
    };


} // namespace brl


#endif // MODEL_HPP
