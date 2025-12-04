#if !defined(RANDOM_HPP)
#define RANDOM_HPP

#include "borealis/util/util.h"

namespace brl
{
    inline int random(int min, int max) {
        return  min + (std::rand() % (max - min + 1));
    }

    inline float random_float(float min, float max) {
        float scale = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return min + scale * (max - min);
    }

} // namespace brl



#endif // RANDOM_HPP
