#include "Zealand.hpp"


Zealand::Zealand(double scale_x, double scale_y, double scale_z) : 
scale_x(scale_x),
scale_y(scale_y),
scale_z(scale_z)
{
    double scales[3] = {scale_x, scale_y, scale_z};
    for(int i = 0; i < 3; i++)
    {
        // Pre-generate the block sizes at each level
        for (int j = 0; j < 21; j++)
        {
            int blocks_per_dim = pow(2,j+1);
            block_sizes[i][j] = scales[i] / blocks_per_dim;
        }
    }
}

AlignedBox3 Zealand::getAlignedBox(unsigned long block)
{
    int level = getLevel(block);
    block = block ^ terminator(level);

    uint_fast32_t x,y,z;
    libmorton::morton3D_64_decode(block, x, y, z);

    double block_size_x = block_sizes[0][level];
    double block_size_y = block_sizes[1][level];
    double block_size_z = block_sizes[2][level];

    Vector3 min({x*block_size_x - block_sizes[0][0], y*block_size_y - block_sizes[1][0], z*block_size_z - block_sizes[2][0]});
    Vector3 max({min[0] + block_size_x, min[1] + block_size_y, min[2] + block_size_z});

    return AlignedBox3(min,max);
}

Vector3 Zealand::getCenter(unsigned long block)
{
    int level = getLevel(block);
    block = block ^ terminator(level);

    uint_fast32_t x,y,z;
    libmorton::morton3D_64_decode(block, x, y, z);

    double center_x, center_y, center_z;
    center_x = (x*block_sizes[0][level] + block_sizes[0][level]/2) - block_sizes[0][0];
    center_y = (z*block_sizes[1][level] + block_sizes[1][level]/2) - block_sizes[1][0];
    center_z = (z*block_sizes[2][level] + block_sizes[2][level]/2) - block_sizes[2][0];

    return Vector3({center_x, center_y, center_z});
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
        volume = volume + block_sizes[0][level] * block_sizes[1][level] * block_sizes[2][level];
    }
    return volume;
}

double Zealand::getVolume(const std::vector<unsigned long>& intervals, int multiplicity)
{
    int level = getLevel(intervals[0]);
    double volume = 0;
    double block_volume = block_sizes[0][level] * block_sizes[1][level] * block_sizes[2][level];
    for (int i = 0; i < intervals.size() - 1; i += 2)
    {
        int num_blocks = 1 + intervals[i+1] - intervals[i];
        volume += num_blocks*block_volume;
    }
    return volume;
}