#if !defined(MODEL_HPP)
#define MODEL_HPP

#include <borealis/util/util.h>

#include "buffer.hpp"

namespace brl
{
    struct GfxVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct GfxMesh {
        GfxAttribBuffer* buffer;
    private:
    };

    struct GfxModelNode {
        std::string name;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        int meshIndex;
        int materialIndex;
    };

    struct GfxModel {
        std::vector<GfxMesh*> meshes;   
        GfxModelNode* rootNode;
        
        GfxModel(std::string path);

    private:
        
    };


} // namespace brl


#endif // MODEL_HPP
