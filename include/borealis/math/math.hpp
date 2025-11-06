#ifndef MATH_HPP
#define MATH_HPP

struct vector2 {
    float x = 0;
    float y = 0;

    vector2 operator+(const vector2& v) {
        return vector2{x + v.x, y + v.y};
    }

    vector2 operator-(const vector2& v) {
        return vector2{x - v.x, y - v.y};
    }

    vector2 operator*(const float& v) {
        return vector2{x *v, y * v};
    }

    vector2 operator/(const float& v) {
        return vector2{x /v, y / v};
    }

};

struct vector3 {
    float x = 0;
    float y = 0;
    float z = 0;

    vector3 operator+(const vector3& v) {
        return vector3{x + v.x, y + v.y, z + v.z};
    }

    vector3 operator-(const vector3& v) {
        return vector3{x - v.x, y - v.y, z - v.z};
    }

    vector3 operator*(const float& v) {
        return vector3{x *v, y * v, z * v};
    }

    vector3 operator/(const float& v) {
        return vector3{x /v, y / v, z / v};
    }

};

struct vector4 {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    vector4 operator+(const vector4& v) {
        return vector3{x + v.x, y + v.y, z + v.z, w + v.w};
    }

    vector4 operator-(const vector4& v) {
        return vector4{x - v.x, y - v.y, z - v.z, w - v.w};
    }

    vector4 operator*(const float& v) {
        return vector4{x *v, y * v, z * v, w * v};
    }

    vector4 operator/(const float& v) {
        return vector4{x /v, y / v, z / v, w / v};
    }

};

#endif