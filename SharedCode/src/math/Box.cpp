#include "pch.h"
#include "Box.h"
#include <string>

Box::Box()
    : start(0), size(0)
{
}
Box::Box(glm::dvec2 start, glm::dvec2 size)
    : start(start), size(size)
{
}

glm::dvec2 Box::getEnd() const
{
    return start + size;
}

glm::dvec2 Box::getCenter() const
{
    return start + (size * 0.5);
}

bool Box::contains(glm::dvec2 point) const
{
    if (point.x < start.x || point.y < start.y) {
            return false;
    }
    else if (point.x > getEnd().x || point.y > getEnd().y) {
        return false;
    }
    return true;
}


//bool Box::contains(Box other)
//{
//    assert(false);
//    return false;
//}

std::string Box::toString() const
{
    return "(" + std::to_string(start.x) + "," + std::to_string(start.y) + ")" + "_origin--" + "(" + std::to_string(size.x) + "," + std::to_string(size.y) + ")" + "_size";
}

std::ostream& operator<<(std::ostream& strm, const Box& a)
{
    return strm << a.toString();
}
