/**
 * @file   Point.cpp
 * @date   Nov 5, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "Point.h"
#include <cmath>

namespace pink {

std::ostream& operator << (std::ostream& os, Point p)
{
    return os << "(" << p.x << "," << p.y << ")";
}

} // namespace pink
