#pragma once

#include<SFML/Graphics.hpp>
#include<math.h>

template<typename T>
class Vec2 {
    public:
    T x =0;
    T y =0;

    Vec2() = default;

    Vec2(T xin, T yin) : x(xin), y(yin) {}

    //conversion from sf::Vector2
    Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {}

    //allows automatic conversion to sf::Vector2
    //this is useful for passing Vec2 to SFML functions
    operator sf::Vector2<T>() {
        return sf::Vector2<T>(x, y);
    }

    Vec2 operator + (const Vec2& rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    Vec2 operator - (const Vec2& rhs) const {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    void normalize () {
        float mag = sqrt((x*x)+(y*y));
        x*=mag;
        y*=mag;
    }

    bool operator == (const Vec2& rhs) const {
        if(x==rhs.x && y==rhs.y)
            return true;
        else 
            return false;
    }

    bool operator != (const Vec2& rhs) const {
        if(x!=rhs.x || y!=rhs.y)
            return true;
        else 
            return false;
    }

    void operator += (const Vec2& rhs) {
        x+=rhs.x;
        y+=rhs.y;
    }

    void operator -= (const Vec2& rhs) {
        x-=rhs.x;
        y-=rhs.y;
    }

    void operator *= (const T val) {
        x*=val;
        y*=val;
    }

    void operator /= (const T val) {
        x/=val;
        y/=val;
    }

    float dist (const Vec2& rhs) const {     
        return sqrt((x-rhs.x)*(x-rhs.x) + (y-rhs.y)*(y-rhs.y));
    }
};

using Vec2f = Vec2<float>;