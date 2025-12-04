#if !defined(RANDOM_HPP)
#define RANDOM_HPP

#include "borealis/util/util.h"

namespace brl
{
    inline int random(int min, int max) {
        return  min + (std::rand() % (max - min + 1));
    }
} // namespace brl



#endif // RANDOM_HPP
