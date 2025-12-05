#if !defined(DEBUG_HPP)
#define DEBUG_HPP
#include "borealis/gfx/gfx.hpp"

namespace brl_debug
{

    /**
     * This function draws a mesh quickly without having to clarify a material. Used for debug purposes.
     * @param buffer The mesh to be drawn.
     * @param transform The model matrix for the mesh to be drawn.
     */
    void drawMesh(brl::GfxAttribBuffer* mesh, glm::mat4 transform);


    /**
     * Thsi function draws a line between two points. Used for debug purposes.
     * @param start The start of the line.
     * @param end The end of thr line.
     */
    void drawLine(glm::vec3 start, glm::vec3 end);

} // namespace brl


#endif // DEBUG_HPP
