#include "util.hpp"

// Abstract base class defining the intersection interface
class VolumeFOV
{
    public:

        virtual bool intersects (const AlignedBox3& box) = 0;
        virtual bool contains (const AlignedBox3& box) = 0;
};