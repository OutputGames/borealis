#if !defined(DEBUG_HPP)
#define DEBUG_HPP
#include "borealis/gfx/gfx.hpp"

namespace brl_debug
{

    void drawMesh(brl::GfxAttribBuffer* buffer, glm::mat4 transform);
    void drawLine(glm::vec3 start, glm::vec3 end);

} // namespace brl


#endif // DEBUG_HPP
