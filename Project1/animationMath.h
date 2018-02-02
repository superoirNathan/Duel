#pragma once

#include <glm/vec3.hpp>
//#include <GLM/gtc/matrix_access.hpp>
//#include <GLM\gtx/string_cast.hpp>
#include <iostream>


namespace Math
{
    // Linear interpolation
    template <typename T>
    T lerp(T d0, T d1, float t)
    {
        return (1 - t) * d0 + d1 * t;
    }

    // inverse lerp
    // Regular lerp: given p0, p1 and a t value you get a point p between p0 and p1
    // Inverse lerp: given p0, p1 and p between p0 and p1 you will get the t value to compute p
    template <typename T>
    float invLerp(T d, T d0, T d1)
    {
        return (d - d0) / (d1 - d0);
    }


    // Returns a point between p1 and p2
    // it comes from expanding and simplifying the UMP formula 
    template <typename T>
    T catmull(T p0, T p1, T p2, T p3, float t)
    {
        return	((p1 * 2.0f) + (-p0 + p2) * t +
            ((p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3) * (t * t) +
            (-p0 + (p1 * 3.0f) - (p2 * 3.0f) + p3) * (t * t * t)) * 0.5f;
    }
    template <typename T>
    T bezier(T p0, T t0, T t1, T p1, float t)
    {
        return	(pow((1 - t), 3)*p0 + 3 * t*pow((1 - t), 2)*t0 + 3 * t*t*(1 - t)*t1 + t * t*t*p1);

    }


    template <typename T>

    glm::vec3 deCastelya(T p0, T p1, T p2, T p3, float t) {
        //return (1 - t) * d0 + d1 * t; 1 lerp

        //Var names dont really tell you anything btw
        //--- first lerps ---//
        T pLerp1((1 - t) * p0 + p1 * t);
        T pLerp2((1 - t) * p1 + p2 * t);
        T pLerp3((1 - t) * p2 + p3 * t);

        //--- 2nd Stage Lerps ---//
        T p2Lerp1((1 - t) * pLerp1 + pLerp2 * t);
        T p2Lerp2((1 - t) * pLerp2 + pLerp3 * t);

        //--- Final Lerp ---//
       T finalLerp((1 - t) * p2Lerp1 + p2Lerp2 * t);

        return finalLerp;
    }

}
