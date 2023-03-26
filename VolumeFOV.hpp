#ifndef VolumeFOV_hpp
#define VolumeFOV_hpp

#include "util.hpp"

namespace libzealand
{
// Abstract base class defining the intersection interface
class VolumeFOV
{
    public:
        virtual VolumeFOV* clone() const = 0;
        virtual bool intersects (const AlignedBox3& box) = 0;
        virtual bool contains (const AlignedBox3& box) = 0;
        virtual bool contains (const Vector3& point) = 0;
};
}

#endif