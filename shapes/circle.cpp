#define _USE_MATH_DEFINES
#include <cmath>
#include "circle.h"

Circle::Circle(string name, ostream &os, Distance radius)
    : Shape(name, os), m_radius(radius) {
        m_os << "Construyendo Circle: " << GetName() << endl;
    }

Circle::~Circle() {
    m_os << "Destruyendo  Circle: " << GetName() << endl;
}

Area Circle::GetArea() const {
    return M_PI * m_radius * m_radius;
}