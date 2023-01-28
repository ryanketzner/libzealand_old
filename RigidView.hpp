#include "util.hpp"

// Abstract base class defining the intersection interface
class RigidView : public VolumeFOV
{
    public:
        virtual bool updatePose() = 0;
};