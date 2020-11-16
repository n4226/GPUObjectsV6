#include "Box.h"

glm::dvec2 Box::getEnd()
{
    return start + size;
}

glm::dvec2 Box::getCenter()
{
    return start + (size * 0.5);
}

bool Box::contains(glm::dvec2 point)
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
