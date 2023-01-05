#include <bitset>
#include <vector>
#include <iostream>
#include "morton.h"
#include "Mathematics/Vector3.h"
#include "Mathematics/AlignedBox.h"
#include "Mathematics/Hypersphere.h"
#include "Mathematics/IntrAlignedBox3Sphere3.h"
#include "Mathematics/ContSphere3.h"

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

        // Blockset intersect(const Blockset&, const Sphere3&, int);
        void refine(Coverage& coverage, const Sphere3& sphere);
        bool covers(const Sphere3& sphere, const AlignedBox3& box);


        Vector3 getCenter(unsigned long block);
        int getLevel(unsigned long block);
        AlignedBox3 getAlignedBox(unsigned long block);
        Block8 getChildren(unsigned long block);

    protected:

        int scale;
        const static int MAX_LEVEL = 20;

        unsigned long remove_level[MAX_LEVEL + 1];
        unsigned long add_level[MAX_LEVEL + 1];
        double block_sizes[MAX_LEVEL + 1];
        unsigned long generate_children[MAX_LEVEL][8];
};