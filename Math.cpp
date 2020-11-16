#include "Math.h"



glm::dvec3 Math::LlatoGeo(glm::dvec3 lla, glm::dvec3 origin, double radius)
{
    lla.x += 90;
    lla.y += 180;

    radius += lla.z;
    glm::dvec3 xyz;

    xyz.x = (-radius * sin(glm::radians(lla.x)) * cos(glm::radians(lla.y)));
    xyz.y = (-radius * cos(glm::radians(lla.x)));
    xyz.z = (-radius * sin(glm::radians(lla.x)) * sin(glm::radians(lla.y)));
    return (xyz - origin);
}

glm::vec3 Math::LlatoGeo(glm::vec3 lla, glm::vec3 origin, float radius)
{
    lla.x += 90;
    lla.y += 180;

    radius += lla.z;
    glm::vec3 xyz;

    xyz.x = (-radius * sin(glm::radians(lla.x)) * cos(glm::radians(lla.y)));
    xyz.y = (-radius * cos(glm::radians(lla.x)));
    xyz.z = (-radius * sin(glm::radians(lla.x)) * sin(glm::radians(lla.y)));
    return (xyz - origin);
}

glm::dvec3 Math::GeotoLla(glm::dvec3 geo, glm::float64 radius, glm::dvec3 origin)
{
    geo += origin;
    if (radius == 0)
        radius = glm::length(geo);

    glm::dvec3 lla;

    lla.x = glm::degrees(M_PI - acos(geo.y / radius));
    lla.y = glm::degrees(atan2(geo.z, geo.x) + M_PI);

    lla.x -= 90;
    lla.y -= 180;

    return lla;
}

glm::vec3 Math::GeotoLla(glm::vec3 geo, glm::float32 radius, glm::vec3 origin)
{
    geo += origin;
    if (radius == 0)
        radius = glm::length(geo);

    glm::vec3 lla;

    lla.x = glm::degrees(M_PI - acos(geo.y / radius));
    lla.y = glm::degrees(atan2(geo.z, geo.x) + M_PI);

    lla.x -= 90;
    lla.y -= 180;

    return lla;
}
