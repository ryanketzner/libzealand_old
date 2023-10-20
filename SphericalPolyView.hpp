#ifndef SphericalPolyView_hpp
#define SphericalPolyView_hpp

#include "util.hpp"
#include "Mathematics/Vector3.h"
#include "RigidView.hpp"

namespace libzealand
{
// Abstract base class defining the intersection interface
class SphericalPolyView : public RigidView
{
    public:

        // Pyramid
        // n1, n2, n3 must be normalized
        SphericalPolyView(Vector3 center, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4)
        {
            // Compute normal vectors
            Vector3 n1 = gte::Cross(v1,v2);
            Vector3 n2 = gte::Cross(v2,v3);
            Vector3 n3 = gte::Cross(v3,v4);
            Vector3 n4 = gte::Cross(v4,v1);

            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v4);

            normals.push_back(n1);
            normals.push_back(n2);
            normals.push_back(n3);
            normals.push_back(n4);

            // Compute plane constants
            Real c1 = gte::Dot(n1,center);
            Real c2 = gte::Dot(n2,center);
            Real c3 = gte::Dot(n3,center);
            Real c4 = gte::Dot(n4,center);

            planes.push_back({n1,c1});
            planes.push_back({n2,c1});
            planes.push_back({n3,c1});
            planes.push_back({n4,c1});

            rays.push_back({center,v1});
            rays.push_back({center,v2});
            rays.push_back({center,v3});
            rays.push_back({center,v4});
        }

        SphericalPolyView* clone() const override
        {
            return new SphericalPolyView(*this);
        }

        bool intersects (const AlignedBox3& box) override
        {
            gte::TIQuery<Real,Ray3,AlignedBox3> query;
            for (int i = 0; i < rays.size(); i++)
            {
                if (query(rays[i], box).intersect)
                {
                    // The box is partially covered.
                    // It can't be fully covered since a corner
                    // intersects the box
                    return true;
                    // Set to make the subsequent contains() check faster
                    not_covered = true;
                }
            }

            // If this point is reached, none of the rays intersect
            // the box.
            std::array<Vector3,8> vertices;
            box.GetVertices(vertices);

            for (int i = 0; i < planes.size(); i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (gte::Dot(planes[i].normal,vertices[i]) >= planes[i].constant)
                        return true;
                }
            }

            return false;
        }

        // AHH! Not Implemented!
        bool contains(const Vector3& vector) override
        {
            return 0;
        }

        bool contains (const AlignedBox3& box) override
        {
            if (not_covered)
            {
                not_covered = 0;
                return false;
            }
            else
            {
                for (int i = 0; i < planes.size(); i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (!gte::Dot(planes[i].normal,vertices[i]) >= planes[i].constant)
                            return false;
                    }
                }
            }
            return true;
        }

        // x, y, z should be inertial coordinates
        void updatePosition(Real x, Real y, Real z)
        {
            center = Vector3 ({x,y,z});
            for (int i = 0; i < planes.size(); i++)
            {
                Real c = gte::Dot(planes[i].normal,center);

                planes[i].constant = c;
                rays[i].origin = center;
            }
        }

        // Must call update orientation before calling update position!
        void updateOrientation(Real r1c1, Real r1c2, Real r1c3, 
                               Real r2c1, Real r2c2, Real r2c3, 
                               Real r3c1, Real r3c2, Real r3c3)
        {
            // sensor to inertial change of basis matrix
            Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});

            for (int i = 0; i < planes.size(); i++)
            {
                // Update halfspace normals and c
                Vector3 inertial_normal = R_NS*normals[i];
                Real c = gte::Dot(inertial_normal,center);
                planes[i].constant = c;
                planes[i].normal = inertial_normal;

                // Update ray directions (needs renormalization?)
                Vector3 inertial_vertex = R_NS*vertices[i];
                rays[i].direction = inertial_vertex;
            }
        }

        void updatePose(Real x, Real y, Real z,
                        Real r1c1, Real r1c2, Real r1c3, 
                        Real r2c1, Real r2c2, Real r2c3, 
                        Real r3c1, Real r3c2, Real r3c3)
        {
            // sensor to inertial change of basis matrix
            Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});
            center = Vector3 ({x,y,z});

            for (int i = 0; i < planes.size(); i++)
            {
                // Update halfspace normals and c
                Vector3 inertial_normal = R_NS*normals[i];
                Real c = gte::Dot(inertial_normal,center);
                planes[i].constant = c;
                planes[i].normal = inertial_normal;

                // Update ray directions (needs renormalization?)
                Vector3 inertial_vertex = R_NS*vertices[i];
                rays[i].direction = inertial_vertex;
                rays[i].origin = center;
            }
        }

    protected:

        // In sensor frame
        std::vector<Vector3> normals;
        std::vector<Vector3> vertices;
        // In inertial frame
        Vector3 center;

        // Normals and directions stored in shape
        // objects in inertial frame
        std::vector<Halfspace3> planes;
        std::vector<Ray3> rays;

        bool not_covered;
};
}

#endif
