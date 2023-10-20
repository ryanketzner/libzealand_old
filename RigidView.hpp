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
        virtual void updatePose(Real x, Real y, Real z,
                                Real r1c1, Real r1c2, Real r1c3, 
                                Real r2c1, Real r2c2, Real r2c3, 
                                Real r3c1, Real r3c2, Real r3c3) = 0;
};
}

#endif
