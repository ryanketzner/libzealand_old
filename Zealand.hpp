#ifndef Zealand_hpp
#define Zealand_hpp

#include "VolumeFOV.hpp"
#include "GTEFOV.hpp"
#include "util.hpp"

using namespace libZealand;

class Zealand
{
    public:

        Zealand(double scale_x, double scale_y, double scale_z);

        void refine(Coverage& coverage, std::vector<VolumeFOV*> shapes, std::vector<VolumeFOV*> not_shapes)
        {
            Blockset new_partial;

            // Loop over each partially covered block
            for (int i = 0; i < coverage[0].size(); i++)
            {
                // Generate 8 children of each partially covered block
                Block8 children = getChildren(coverage[0][i]);
                // Check coverage status of each child
                for (int j = 0; j < 8; j++)
                {
                    AlignedBox3 box = getAlignedBox(children[j]);

                    // Check whether all shapes intersect box
                    for (int k = 0; k < shapes.size(); k++)
                    {
                        // If any shape doesn't intersect, break
                        if (!shapes[k]->intersects(box))
                            break;
                    }

                    for (int k = 0; k < not_shapes.size(); k++)
                    {
                        // If any not_shape fully covers box, break
                        if (not_shapes[k]->contains(box))
                            break;   
                    }

                    // At this point, all shapes intersect and
                    // no not_shape covers, so the box is at least
                    // partially contained

                    // Check whether all shapes cover box
                    for (int k = 0; k < shapes.size(); i++)
                    {
                        // If any shape doesn't fully cover
                        // then the box is only partially contained
                        if (!shapes[k]->contains(box))
                        {
                            new_partial.push_back(children[j]);
                            break;
                        }

                    }

                    for (int k = 0; k < not_shapes.size(); k++)
                    {
                        // If any not_shape intersects box
                        // then the box is only partially contained
                        if (not_shapes[k]->intersects(box))
                        {
                            new_partial.push_back(children[j]);
                            break;
                        }
                    }

                    // Contains
                    coverage[1].push_back(children[j]);
                }
            }

            // Update partial coverage blockset
            coverage[0] = std::move(new_partial);

            return;
        }

        template<class Shape>
        void refine(Coverage& coverage, const Shape& shape)
        {
            Blockset new_partial;

            gte::TIQuery<double,AlignedBox3,Shape> query;

            // Loop over each partially covered block
            for (int i = 0; i < coverage[0].size(); i++)
            {
                // Generate 8 children of each partially covered block
                Block8 children = getChildren(coverage[0][i]);
                // Check coverage status of each child
                for (int j = 0; j < 8; j++)
                {
                    AlignedBox3 box = getAlignedBox(children[j]);

                    if (query(box, shape).intersect)
                    {
                        if (covers(shape,box))
                            coverage[1].push_back(children[j]);
                        else
                            new_partial.push_back(children[j]);
                    }
                }
            }

            // Update partial coverage blockset
            coverage[0] = std::move(new_partial);

            return;
        }


        template<class Shape>
        static bool covers(const Shape& shape, const AlignedBox3& box)
        {
            std::array<Vector3,8> vertices;
            box.GetVertices(vertices);

            for (int i = 0; i < vertices.size(); i++)
            {
                // If any vertex is not in the container,
                // immediately terminate
                if (!gte::InContainer(vertices[i], shape))
                    return false;
            }
            // All vertices are in container
            return true;
        }

        Vector3 getCenter(unsigned long block);
        double getVolume(const Blockset& region);
        double getVolume(const Blockset& intervals, int multiplicity);
        AlignedBox3 getAlignedBox(unsigned long block);

        // Public data members
        double scale_x;
        double scale_y;
        double scale_z;
        double block_sizes[3][MAX_LEVEL + 1];
};

#endif