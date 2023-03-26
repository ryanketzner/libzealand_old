#ifndef RigidView_hpp
#define RigidView_hpp

#include "util.hpp"
#include "VolumeFOV.hpp"

namespace libzealand
{
// Abstract base class defining the intersection interface
class RigidView : public VolumeFOV
{
    public:
        virtual RigidView* clone() const = 0;
        virtual void updatePose(double x, double y, double z,
                                double r1c1, double r1c2, double r1c3, 
                                double r2c1, double r2c2, double r2c3, 
                                double r3c1, double r3c2, double r3c3) = 0;
};
}

#endif