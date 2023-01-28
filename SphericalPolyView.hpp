#include "util.hpp"

// Abstract base class defining the intersection interface
class SphericalPolyView : public RigidView
{
    public:

        // Pyramid
        // n1, n2, n3 must be normalized
        SphericalPolyView(Vector3 center, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4) :
        {
            // Compute normal vectors
            Vector3 n1 = cross(v1,v2);
            Vector3 n2 = cross(v2,v3);
            Vector3 n3 = cross(v3,v4);
            Vector3 n4 = cross(v4,v1);

            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v4);

            normals.push_back(n1);
            normals.push_back(n2);
            normals.push_back(n3);
            normals.push_back(n4);

            // Compute plane constants
            double c1 = n1*center;
            double c2 = n2*center;
            double c3 = n3*center;
            double c4 = n4*center;

            planes.push_back(n1,c1);
            planes.push_back(n2,c1);
            planes.push_back(n3,c1);
            planes.push_back(n4,c1);

            rays.push_back(center,v1);
            rays.push_back(center,v2);
            rays.push_back(center,v3);
            rays.push_back(center,v4);
        }

        bool intersects (const AlignedBox3& box) override
        {
            TIQuery<double,AlignedBox3,Ray3> query;
            for (int i = 0; i < rays.size(); i++)
            {
                if (query(box, rays[i]).intersect)
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
                    if (planes[i].normal*vertices[i] >= planes[i].constant)
                        return true;
                }
            }

            return false;
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
                        if (!planes[i].normal*vertices[i] >= planes[i].constant)
                            return false;
                    }
                }
            }
            return true;
        }

        // x, y, z should be inertial coordinates
        void updatePosition(double x, double y, double z)
        {
            center = Vector3 ({x,y,z});
            for (int i = 0; i < planes.size(); i++)
            {
                double c = planes[i].normal*center;

                planes[i].constant = c;
                rays[i].origin = center;
            }
        }

        // Must call update orientation before calling update position!
        void updateOrientation(double r1c1, double r1c2, double r1c3, 
                               double r2c1, double r2c2, double r2c3, 
                               double r3c1, double r3c2, double r3c3)
        {
            // sensor to inertial change of basis matrix
            Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});

            for (int i = 0; i < planes.size(); i++)
            {
                // Update halfspace normals and c
                Vector3 inertial_normal = R_NS*normals[i];
                double c = inertial_normal*center;
                planes[i].constant = c;
                planes[i].normal = inertial_normal;

                // Update ray directions (needs renormalization?)
                Vector3 inertial_vertex = R_NS*vertices[i];
                rays[i].direction = inertial_vertex;
            }
        }

        void updatePose(double x, double y, double z,
                        double r1c1, double r1c2, double r1c3, 
                        double r2c1, double r2c2, double r2c3, 
                        double r3c1, double r3c2, double r3c3)
        {
            // sensor to inertial change of basis matrix
            Matrix3x3 R_NS({r1c1, r1c2, r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3});
            center = Vector3 ({x,y,z});

            for (int i = 0; i < planes.size(); i++)
            {
                // Update halfspace normals and c
                Vector3 inertial_normal = R_NS*normals[i];
                double c = inertial_normal*center;
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