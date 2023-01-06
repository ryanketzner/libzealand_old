#include "Zealand.hpp"

Zealand::Zealand(double scale) : 
scale(scale)
{
    // Pre-generate the values used for
    // adding and removing level codes
    for (unsigned long i = 1; i < 22; i++)
	{
		add_level[i-1] = pow(2,3*i);
        std::bitset<64> bitty(add_level[i-1]);
        bitty.flip();
        remove_level[i-1] = bitty.to_ulong();
	}

    // Pre-generate the block sizes at each level
    for (int i = 0; i < 21; i++)
    {
        int blocks_per_dim = pow(2,i+1);
        block_sizes[i] = scale / blocks_per_dim;
    }

    // Pre-generate the values used for
    // generating children
    unsigned long one_seven[8];
    for (unsigned long i = 0; i < 8; i++)
        one_seven[i] = i;

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // Children are shifted by three at each level
            one_seven[j] = one_seven[j] << 3;
            //Each child is ANDed with level termination bit
            // Children of ith level are at i+1th level
            generate_children[i][j] = one_seven[j] | add_level[i+1];
        }
    }
}

int Zealand::getLevel(unsigned long block)
{
    return MAX_LEVEL - __builtin_clzl(block)/3;
}

Vector3 Zealand::getCenter(unsigned long block)
{
    int level = getLevel(block);
    block = block & remove_level[level];

    uint_fast32_t x,y,z;
    libmorton::morton3D_64_decode(block, x, y, z);

    double center_x, center_y, center_z;
    center_x = (x*block_sizes[level] + block_sizes[level]/2) - block_sizes[0];
    center_y = (z*block_sizes[level] + block_sizes[level]/2) - block_sizes[0];
    center_z = (z*block_sizes[level] + block_sizes[level]/2) - block_sizes[0];

    return Vector3({center_x, center_y, center_z});
}


AlignedBox3 Zealand::getAlignedBox(unsigned long block)
{
    int level = getLevel(block);
    block = block & remove_level[level];

    uint_fast32_t x,y,z;
    libmorton::morton3D_64_decode(block, x, y, z);

    Vector3 min;
    Vector3 max;

    double block_size = block_sizes[level];

    min[0] = x*block_size - block_sizes[0];
    min[1] = y*block_size - block_sizes[0];
    min[2] = z*block_size - block_sizes[0];

    max[0] = min[0] + block_size;
    max[1] = min[1] + block_size;
    max[2] = min[2] + block_size;

    return AlignedBox3(min,max);
}

Block8 Zealand::getChildren(unsigned long block)
{
    Block8 children;
    for (unsigned long i = 0; i < 8; i++)
    {
        // Shift left and or with child number
        children[i] = (block << 3) | i;
    }
    return children;
} 

bool Zealand::covers(const Sphere3& sphere, const AlignedBox3& box)
{
    std::array<Vector3,8> vertices;
    box.GetVertices(vertices);

    for (int i = 0; i < vertices.size(); i++)
    {
        // If any vertex is not in the container,
        // immediately terminate
        if (!gte::InContainer(vertices[i], sphere))
            return false;
    }
    // All vertices are in container
    return true;
}

double Zealand::getVolume(const Blockset& region)
{
    double volume = 0;
    for (int i = 0; i < region.size(); i++)
    {
        int level = getLevel(region[i]);
        volume = volume + pow(block_sizes[level],3);
    }
    return volume;
}

void Zealand::refine(Coverage& coverage, const Sphere3& sphere)
{
    Blockset new_partial;
    Blockset uncovered;

    gte::TIQuery<double,AlignedBox3,Sphere3> query;

    // Loop over each partially covered block
    for (int i = 0; i < coverage[0].size(); i++)
    {
        // Generate 8 children of each partially covered block
        Block8 children = getChildren(coverage[0][i]);
        // Check coverage status of each child
        for (int j = 0; j < 8; j++)
        {
            AlignedBox3 box = getAlignedBox(children[j]);

            if (query(box, sphere).intersect)
            {
                if (covers(sphere,box))
                    coverage[1].push_back(children[j]);
                else
                    new_partial.push_back(children[j]);
            }
        }
    }

    // Update partial coverage blockset
    coverage[0] = new_partial;

    return;
}