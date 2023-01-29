#ifndef ConeView_hpp
#define ConeView_hpp

#include "util.hpp"

class ConeView : public RigidView
{
public:

    // I think in this case, direction will always be aligned with the sensor frame Z-axis
    // with sensor orientation represented by the sensor to body matrix
    ConeView(Vector3 center, Vector3 direction, double angle) :
    cone(Ray3(center, direction), angle),
    direction(direction)
    {
    }

    bool intersects(const AlignedBox3& box) override
    {
        return query(box,cone).intersect;
    }

    bool contains(const AlignedBox3& box) override
    {
        std::array<Vector3,8> vertices;
        box.GetVertices(vertices);

        for (int i = 0; i < vertices.size(); i++)
        {
            // If any vertex is not in the container,
            // immediately terminate
            if (!gte::InContainer(vertices[i], cone))
                return false;
        }
        // All vertices are in container
        return true;
    }

    void updatePose(double x, double y, double z,
                    double r1c1, double r1c2, double r1c3, 
                    double r2c1, double r2c2, double r2c3, 
                    double r3c1, double r3c2, double r3c3)
    {
        // Update position
        Vector3 center({x,y,z});
        cone.ray.origin = center;

        // sensor to inertial change of basis matrix
        Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});

        // Update orientation
        cone.ray.direction = R_NS*direction;
    }

protected:
    // In sensor frame
    Vector3 direction;

    Cone3 cone;
    gte::TIQuery<double,AlignedBox3,Cone3> query;
};

#endif