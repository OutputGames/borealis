#include "borealis/math/matrix.hpp"
#include "borealis/math/vector.hpp"

#include <iostream>

brl::vector4 &brl::vector4::operator*=(const brl::matrix4x4 &v)
{

    std::cout << toString() << std::endl; 

    vector4 o = v * (*this);

    x = o.x;
    y = o.y;
    z = o.z;
    w = o.w;

    return *this;
}

/**
    brl::vector4 &operator*=(brl::vector4 &_v, const brl::matrix4x4 &v)
    {
        brl::vector4 o = v * _v;
    
        _v.x = o.x;
        _v.y = o.y;
        _v.z = o.z;
        _v.w = o.w;
    
        return _v;
    }
*/