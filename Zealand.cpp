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
            // Each child is ANDed with level termination bit
            // Children of ith level are at i+1th level
            generate_children[i][j] = one_seven[j] | add_level[i+1];
        }
    }
}

Blockset Zealand::collapse(const Blockset& blockset, int level)
{
    if (blockset.size() == 0)
        std::cout << "Exception" << std::endl;

    Blockset collapsed;
    int max_level = getLevel(blockset.back());

    if (max_level < level)
    {
        for (int i = 0; i < blockset.size(); i++)
        {
            int block_level = getLevel(blockset[i]);
            int depth = level - block_level;

            int old_size = collapsed.size();
            appendChildren(blockset[i], collapsed, depth);
            int new_size = collapsed.size();
            int added = new_size - old_size;

            if (added != pow(8,depth))
            {
                // std::cout << "Error!" << std::endl;
                // std::cout << added << std:: endl;
                // std::cout << pow(8,depth) << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Exception" << std::endl;
    }
    // Q: Will this automatically call std::move?
    return collapsed;
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

    double block_size = block_sizes[level];

    Vector3 min({x*block_size - block_sizes[0], y*block_size - block_sizes[0], z*block_size - block_sizes[0]});
    Vector3 max({min[0] + block_size, min[1] + block_size, min[2] + block_size});

    return AlignedBox3(min,max);
}

Block8 Zealand::getChildren(unsigned long block)
{
    Block8 children;

    // Shift left by one level and OR with child number
    block = block << 3;
    for (unsigned long i = 0; i < 8; i++)
        children[i] = block | i;

    return children;
}

unsigned long Zealand::set3NBits(int n)
{
    unsigned long mask = 0;
    for (int i = 0; i < n; i++)
    {
        // Set to 111 at i = 0
        // Set to 111111 at i = 1
        // ...
        mask = (mask << 3) | 7;
    }
    return mask;
}

unsigned long Zealand::getSmallestChild(unsigned long block, int depth)
{
    return block << 3*depth;
}

unsigned long Zealand::getLargestChild(unsigned long block, int depth)
{
    return (block << 3*depth) | set3NBits(depth);
}

void Zealand::appendChildren(unsigned long block, Blockset& blockset, int depth)
{
    unsigned long smallest_child = getSmallestChild(block,depth);
    unsigned long largest_child = getLargestChild(block,depth);

    for (unsigned long i = smallest_child; i <= largest_child; i++)
        blockset.push_back(i);
}

// void Zealand::appendChildren(unsigned long block, Blockset& blockset, int depth)
// {
//     int largest_child_level = getLevel(block) + depth;

//     for (int i = 1; i <= depth; i++)
//     {
//         int smallest_child = getSmallestChild(block,i);
//         int largest_child = getLargestChild(block,i);

//         for (unsigned long j = smallest_child; j <= largest_child; j++)
//         {
//             blockset.push_back(j);
//         }
//     }
// }

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