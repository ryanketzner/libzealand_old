#ifndef VolumeFOV_hpp
#define VolumeFOV_hpp

#include "util.hpp"

namespace libzealand
{
// Abstract base class defining the intersection interface
class VolumeFOV
{
    public:

        virtual bool intersects (const AlignedBox3& box) = 0;
        virtual bool contains (const AlignedBox3& box) = 0;
};
}

#endif