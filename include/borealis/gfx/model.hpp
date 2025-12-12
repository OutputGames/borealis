#if !defined(MODEL_HPP)
#define MODEL_HPP

#include <borealis/util/util.h>

#include "buffer.hpp"
#include "engine.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "borealis/ecs/entity.hpp"

#include <vector>

namespace tinygltf
{
    class Model;
    struct Material;
    class Node;
}

namespace brl
{
    struct GfxSkeleton;
    struct GfxModelEntity;
    struct GfxSkin;
    struct GfxSkinnedMeshRenderer;
    struct GfxModelNode;
    struct GfxMeshRenderer;
    struct GfxModel;

    struct GfxVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct GfxSkinnedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        glm::ivec4 boneIds;
        glm::vec4 weights;
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
        friend GfxSkinnedMeshRenderer;
        GfxSubMesh** subMeshes;
        int subMeshCount;
    };

    struct GfxModelNode {
        std::string name;
        glm::vec3 position = glm::vec3(0);
        glm::quat rotation = glm::identity<glm::quat>();
        glm::vec3 scale = glm::vec3(1);

        int mesh = -1;
        int skin = -1;
        std::vector<GfxModelNode*> children;

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

    struct GfxAnimation {

        enum ChannelInterpolation {
            LINEAR,
            STEP,
            CUBICSPLINE
        };

        enum ChannelType {
            TRANSLATION,
            ROTATION,
            SCALE
        };

        struct AnimationFrame {
            float time;
        };

        struct Vec3AnimationFrame : AnimationFrame {
            glm::vec3 value;
        };

        struct QuatAnimationFrame : AnimationFrame {
            glm::quat value;
        };

        struct Channel {
            int boneIndex;
            ChannelInterpolation interpolation;
            ChannelType type;

            std::vector<AnimationFrame> frames;
        };

        std::string name;

        std::vector<Channel> channels;

    };


    struct GfxModel {
        std::vector<GfxMesh*> meshes;
        std::vector<GfxTexture2d*> textures;
        std::vector<GfxMaterialDescription*> materialDescriptions;
        std::vector<GfxMaterial*> materials;
        std::vector<GfxSkin*> skins;
        std::vector<GfxAnimation*> animations;

        static GfxModel* loadModel(std::string path);

        GfxModelEntity* createEntity();

    private:
        friend GfxEngine;

        GfxModel(std::string path);
        GfxModelNode* rootNode;

        GfxModelNode* processNode(tinygltf::Node node, tinygltf::Model scene, int index=-1);

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


    struct GfxBone
    {
        std::string name;
        glm::vec3 position = glm::vec3(0);
        glm::quat rotation = glm::identity<glm::quat>();
        glm::vec3 scale = glm::vec3(1);
        glm::mat4 inverseBindMatrix = glm::mat4(1.0);
        glm::mat4 worldMatrix = glm::mat4(1.0);

        int parent = -1;

        std::vector<int> children;

        glm::mat4 calculateLocalTransform();
        void validate();
        void apply();

    private:
        friend GfxSkeleton;
        bool changed = true;

        glm::vec3 lastCheckedPosition, lastCheckedScale;
        glm::quat lastCheckedRotation;
        
    };

    struct GfxSkin
    {
        std::string name;
        std::vector<GfxBone*> bones;

    private:
        friend GfxModel;
        void initialize();
        void _calcTransform(brl::GfxBone* bone, const glm::mat4& parentTransform, int index);
    };

    struct GfxSkeleton : EcsEntity
    {
        std::vector<GfxBone*> bones;

        void earlyUpdate() override;
        std::vector<glm::mat4> calculateTransforms();
        

    private:

        std::vector<glm::mat4> jointMatrices;
        void _calcTransform(brl::GfxBone* bone, const glm::mat4& parentTransform, int index,
                                 bool parentChanged);

    };

    struct GfxSkinnedMeshRenderer : GfxMeshRenderer
    {
        GfxSkinnedMeshRenderer();

        void start() override;

        int skeletonIndex = -1;

        void lateUpdate() override;

    private:
        GfxModelEntity* model = nullptr;
    };



    struct GfxAnimator : EcsEntity {

        GfxAnimation* animation;

        GfxAnimator();

        void update() override;

    private:
        GfxModelEntity* model= nullptr;

        float time = 0;
    };

    struct GfxModelEntity : EcsEntity
    {
        std::vector<GfxSkeleton*> skeletons;
        GfxModel* model;
    };


} // namespace brl


#endif // MODEL_HPP
