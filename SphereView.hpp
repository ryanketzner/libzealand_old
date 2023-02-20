#ifndef SphereView_hpp
#define SphereView_hpp

#include "util.hpp"
#include "RigidView.hpp"

namespace libzealand
{
class SphereView : public RigidView
{
public:

    SphereView(Vector3 center, double radius) :
    sphere(center, radius)
    {
    }

    bool intersects(const AlignedBox3& box) override
    {
        return query(box,sphere).intersect;
    }

    // bool contains(const AlignedBox3& box) override
    // {
    //     std::array<Vector3,8> vertices;
    //     box.GetVertices(vertices);

    //     for (int i = 0; i < vertices.size(); i++)
    //     {
    //         // If any vertex is not in the container,
    //         // immediately terminate
    //         if (!gte::InContainer(vertices[i], sphere))
    //             return false;
    //     }
    //     // All vertices are in container
    //     return true;
    // }

    bool contains(const AlignedBox3& box) override
    {
        if (!gte::InContainer(box.min, sphere))
            return false;
        else if (!gte::InContainer(box.max, sphere))
            return false;

        // Vector3 vertex;

        // // min min max
        // vertex[0] = box.min[0];
        // vertex[1] = box.min[1];
        // vertex[2] = box.max[1];

        // if (!gte::InContainer(box.min, sphere)
        //     return false;

        // // min max max
        // vertex[1] = box.max[1];

        // if (!gte::InContainer(box.min, sphere)
        //     return false;

        // // min max min
        // vertex[2] = box.min[1];
        std::array<Vector3,8> vertices;
        box.GetVertices(vertices);
        for (int i = 1; i < vertices.size() - 1; i++)
        {
            // If any vertex is not in the container,
            // immediately terminate
            if (!gte::InContainer(vertices[i], sphere))
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
        Vector3 center({x,y,z});
        sphere.center = center;
    }

protected:

    Sphere3 sphere;
    gte::TIQuery<double,AlignedBox3,Sphere3> query;
};
}

#endif