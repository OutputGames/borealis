#if !defined(MATRIX_HPP)
#define MATRIX_HPP

#include "borealis/math/vector.hpp"
#include <string>

namespace brl
{
    struct matrix4x4 {
        vector4 v1, v2,v3,v4;

        vector4& operator[](int idx) {
            idx = idx % 4;
            switch (idx) {
                case 0:
                    return v1;
                case 1:
                    return v2;
                case 2:
                    return v3;
                case 3:
                    return v4;
                default:
                    return v1;
            }
        }

        const vector4& operator[](int idx) const {
            idx = idx % 4;
            switch (idx) {
                case 0:
                    return v1;
                case 1:
                    return v2;
                case 2:
                    return v3;
                case 3:
                    return v4;
                default:
                    return v1;
            }
        }

        matrix4x4& operator+(matrix4x4 m) {
            matrix4x4 o = *this;
            o += m;
            return o;
        }

        matrix4x4& operator+=(matrix4x4 m) {
            v1 += m.v1;
            v2 += m.v2;
            v3 += m.v3;
            v4 += m.v4;
            return *this;
        }

        matrix4x4& operator-(matrix4x4 m) {
            matrix4x4 o = *this;
            o -= m;
            return o;
        }

        matrix4x4& operator-=(matrix4x4 m) {
            v1 -= m.v1;
            v2 -= m.v2;
            v3 -= m.v3;
            v4 -= m.v4;
            return *this;
        }
        
        matrix4x4& operator*(float s) {
            matrix4x4 o = *this;
            o *= s;
            return o;
        }

        matrix4x4& operator*=(float s) {
            v1 *= s;
            v2 *= s;
            v3 *= s;
            v4 *= s;
            return *this;
        }

        matrix4x4 operator*(const matrix4x4& s) const {
            matrix4x4 o = *this;
            o *= s;
            return o;
        }

        matrix4x4& operator*=(const matrix4x4& s) {
            // Store original values since we'll be modifying the matrix in-place
            matrix4x4 temp = *this;

            // for every row
            for (int row = 0; row < 4; ++row) {
                // iterate through every column
                for (int col = 0; col < 4; ++col) {
                    float sum = 0.0f;
                    for (int k = 0; k < 4; ++k) {
                        sum += temp[k][row] * s[col][k];
                    }
                    (*this)[col][row] = sum;
                }
            }

            return *this;
        }

        matrix4x4 operator*(const vector4& s) const {
            matrix4x4 o = *this;
            o *= s;
            return o;
        }

        matrix4x4& operator*=(const vector4& s) {
            // Store original values since we'll be modifying the matrix in-place
            matrix4x4 temp = *this;

            return *this;
        }
        
        std::string toString() {
            std::string s = "";

            for (int y = 0; y < 4; y++)
            {
                s += "[";

                for (int x = 0; x < 4; x++)
                {
                    s += "\t";
                    s += std::to_string((*this)[x][y]);
                }
                
                s += "]";


                    s += "\n";

            }

            
            s += "\n";

            return s;
        }

    };
} // namespace brl


#endif // MATRIX_HPP
