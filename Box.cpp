#include "Box.h"
#include "pch.h"

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

std::ostream& operator<<(std::ostream& strm, const Box& a)
{
    return strm << "(" << a.start.x << a.start.y << ")" << "_origin--" << "(" << a.size.x << a.size.y << ")" << "_size";
}
