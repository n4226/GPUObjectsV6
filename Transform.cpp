#include "Transform.h"

glm::mat4 MyStruct::matrix()
{

    auto mat = glm::translate(glm::mat4(1.f), position);
}
