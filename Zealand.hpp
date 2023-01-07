#include <bitset>
#include <vector>
#include <iostream>
#include "morton.h"
#include "Mathematics/Vector3.h"
#include "Mathematics/AlignedBox.h"
#include "Mathematics/Hypersphere.h"
#include "Mathematics/IntrAlignedBox3Sphere3.h"
#include "Mathematics/ContSphere3.h"
#include "Mathematics/ContAlignedBox.h"
#include "Mathematics/IntrAlignedBox3AlignedBox3.h"

using Vector3 = gte::Vector3<double>;
using AlignedBox3 = gte::AlignedBox3<double>;
using Sphere3 = gte::Sphere3<double>;
using Blockset = std::vector<unsigned long>;
using Block8 = std::array<unsigned long,8>;
using Coverage = std::array<Blockset,2>;

class Zealand
{
    public:

        Zealand(double scale);

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


        Blockset collapse(const Blockset&, int level);
        Vector3 getCenter(unsigned long block);
        int getLevel(unsigned long block);
        double getVolume(const Blockset& region);
        AlignedBox3 getAlignedBox(unsigned long block);
        Block8 getChildren(unsigned long block);

        void appendChildren(unsigned long block, Blockset& blockset, int levels);
        unsigned long set3NBits(int n);
        unsigned long getLargestChild(unsigned long block, int depth);
        unsigned long getSmallestChild(unsigned long block, int depth);

    protected:

        double scale;
        const static int MAX_LEVEL = 20;

        unsigned long remove_level[MAX_LEVEL + 1];
        unsigned long add_level[MAX_LEVEL + 1];
        double block_sizes[MAX_LEVEL + 1];
        unsigned long generate_children[MAX_LEVEL][8];
};