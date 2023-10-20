#ifndef ConeView_hpp
#define ConeView_hpp

#include "util.hpp"
#include "RigidView.hpp"

namespace libzealand
{
class ConeView : public RigidView
{
public:

    // I think in this case, direction will always be aligned with the sensor frame Z-axis
    // with sensor orientation represented by the sensor to body matrix
    ConeView(Vector3 center, Vector3 direction, Real angle) :
    cone(Ray3(center, direction), angle),
    direction(direction)
    {
    }

    // Defaults to direction aligned along Z axis

    ConeView() :
    cone(Ray3(Vector3({0.0,0.0,0.0}),Vector3({0.0,0.0,1.0})), M_PI/4),
    direction(Vector3({0.0,0.0,1.0}))
    {
    }

    ConeView* clone() const override
    {
        return new ConeView(*this);
    }

    bool intersects(const AlignedBox3& box) override
    {
        return query(box,cone).intersect;
    }

    bool contains(const Vector3& vector) override
    {
        return gte::InContainer(vector,cone);
    }

    bool contains(const AlignedBox3& box) override
    {
        if (!gte::InContainer(box.min, cone))
            return false;
        else if (!gte::InContainer(box.max, cone))
            return false;

        std::array<Vector3,8> vertices;
        box.GetVertices(vertices);
        for (int i = 1; i < vertices.size() - 1; i++)
        {
            // If any vertex is not in the container,
            // immediately terminate
            if (!gte::InContainer(vertices[i], cone))
                return false;
        }
        // All vertices are in container
        return true;
    }

    void updatePose(Real x, Real y, Real z,
                    Real r1c1, Real r1c2, Real r1c3, 
                    Real r2c1, Real r2c2, Real r2c3, 
                    Real r3c1, Real r3c2, Real r3c3)
    {
        // Update position
        Vector3 center({x,y,z});
        cone.ray.origin = center;

        // sensor to inertial change of basis matrix
        Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});

        // Update orientation
        cone.ray.direction = R_NS*direction;
    }

    // Cone stored as public member
    Cone3 cone;

protected:
    // In sensor frame
    Vector3 direction;

    gte::TIQuery<Real,AlignedBox3,Cone3> query;
};
}

#endif
