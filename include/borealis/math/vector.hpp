#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <string>

namespace brl
{

    struct matrix4x4;

    struct vector2
    {
        float x = 0;
        float y = 0;

        vector2 operator+(const vector2 &v)
        {
            return vector2{x + v.x, y + v.y};
        }

        vector2 operator-(const vector2 &v)
        {
            return vector2{x - v.x, y - v.y};
        }

        vector2 operator*(const float &v)
        {
            return vector2{x * v, y * v};
        }

        vector2 operator/(const float &v)
        {
            return vector2{x / v, y / v};
        }
    };

    struct vector3
    {
        float x = 0;
        float y = 0;
        float z = 0;

        vector3 operator+(const vector3 &v)
        {
            return vector3{x + v.x, y + v.y, z + v.z};
        }

        vector3 operator-(const vector3 &v)
        {
            return vector3{x - v.x, y - v.y, z - v.z};
        }

        vector3 operator*(const float &v)
        {
            return vector3{x * v, y * v, z * v};
        }

        vector3 operator/(const float &v)
        {
            return vector3{x / v, y / v, z / v};
        }
    };

    struct vector4
    {
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;

        vector4& operator+(const vector4 &v)
        {
            vector4 o = *this;

            o += v;

            return o;
        }

        vector4& operator+=(const vector4 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        vector4& operator-(const vector4 &v)
        {
            vector4 o = *this;

            o -= v;

            return o;
        }

        vector4& operator-=(const vector4 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        vector4& operator*(float v)
        {
            vector4 o = *this;

            o *= v;

            return o;
        }

        vector4& operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }

        float& operator[](int idx) {
            idx = idx % 4;
            switch (idx) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                case 3:
                    return w;
            }
        }

        const float& operator[](int idx) const {
            idx = idx % 4;
            switch (idx) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                case 3:
                    return w;
            }
        }

        vector4& operator*(const matrix4x4& v) const
        {
            vector4 o = *this;

            o *= v;
            
            return o;
        }

        vector4& operator*=(const matrix4x4& v);

        std::string toString() {
            std::string s = "";

            s += "[";

            for (int i = 0; i < 4; i++)
            {
                s += std::to_string((*this)[i]);
            }

            s += "]";

            return s;
        }

    };

    
}

#endif