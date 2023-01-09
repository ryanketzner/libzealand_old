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

Rangeset Zealand::toIntervalBounds(const Blockset& blockset)
{
    if (blockset.size() == 0)
        std::cout << "Exception" << std::endl;

    int max_level = getLevel(blockset.back());
    Rangeset intervals;

    for (int i = 0; i < blockset.size(); i++)
    {
        int block_level = getLevel(blockset[i]);
        int depth = max_level - block_level;
        intervals.emplace_back(getRangeStart(blockset[i],depth));
        intervals.emplace_back(getRangeStop(blockset[i],depth));
    }

    // Q: RVO performed here?
    return intervals;
}

std::vector<std::vector<unsigned long>> Zealand::toIntervals(const Rangeset& interval_bounds, int max_multiplicity)
{
    std::vector<std::vector<unsigned long>> multiplicities(max_multiplicity);

    bool run = false;
    int run_mult = 0;
    unsigned long run_start;
    int multiplicity = 0;
    int i;
    for (i = 0; i < interval_bounds.size() - 1; i++)
    {
        unsigned long block = interval_bounds[i].first;
        if (interval_bounds[i].second == 0) // open
        {
            multiplicity++;
            std::cout << "Open at: " << block << std::endl;

            // open -> closed OR open -> diff
            if (interval_bounds[i+1].second == 1 || interval_bounds[i+1].first != block) // has volume!
            {
                if (run)
                {
                    if (multiplicity == run_mult)
                    {
                        // keep running !
                    }
                    else
                    {
                        // the run stops here
                        unsigned long run_stop = block - 1;
                        // the current block STARTS a run of different multiplicity then the current run

                        if (run_mult > 0)
                        {
                            multiplicities[run_mult - 1].emplace_back(run_start);
                            multiplicities[run_mult - 1].emplace_back(run_stop);
                        }
                        //std::cout << "Ended run at: " << run_stop << " with multiplicity " << run_mult << std::endl;
                        std::cout << "Made run (" << run_start << ", " << run_stop << ") with multiplicity " << run_mult << std::endl;

                        // start the next run !
                        run = true;
                        run_mult = multiplicity;
                        run_start = block;
                        //std::cout << "Starting run at: " << run_start << " with multiplicity " << run_mult << std::endl;
                    }
                }
                else // start running !
                {
                    run = true;
                    run_mult = multiplicity;
                    run_start = block;

                    //std::cout << "Starting run at: " << run_start << " with multiplicity " << run_mult << std::endl;
                }
            }
            else // doesn't have volume!
            {
                // I think if there is no volume on an open block
                // we can't be running yet
                // next loop iter
            }
        }
        else // close
        {
            multiplicity--;
            //std::cout << "Closed at: " << block << std::endl;

            // closed -> diff closed OR closed -> nonconsec  open
            if ( (interval_bounds[i+1].second == 1 && interval_bounds[i+1].first != block) || // has volume!
                 (interval_bounds[i+1].first != (block+1) && interval_bounds[i+1].second == 0) )
            {
                if (!run)
                {
                    std::cout << "Volume! I thought this would never happen..." << std::endl;
                }
                else // we are in a run
                {
                    if (multiplicity == run_mult)
                    {
                    }// keep running!
                    else
                    {
                        // the run stops here
                        unsigned long run_stop = block;
                        // the current block ENDS a run
                        
                        // Just ignore runs = 0
                        if (run_mult > 0)
                        {
                            //std::cout << "Ending run at: " << run_stop << " with multiplicity " << run_mult << std::endl;
                            multiplicities[run_mult - 1].push_back(run_start);
                            multiplicities[run_mult - 1].push_back(run_stop);
                        }
                        std::cout << "Made run (" << run_start << ", " << run_stop << ") with multiplicity " << run_mult << std::endl;

                        // start the next run !
                        // when starting run from a closed block
                        // we want to start from the next consecutive block (even if it isn't in the bounds list)
                        run = true;
                        run_mult = multiplicity;
                        run_start = block + 1;

                        //std::cout << "Starting run at: " << run_start << " with multiplicity " << run_mult << std::endl;
                    }
                }
            }
            else // doesn't have volume!
            {
                if (run)
                {
                    // std::cout << "running" << std::endl;
                }
                else
                {
                    std::cout << "No volume! I thought this would never happen..." << std::endl;
                }
            }
        }
    }
    // obviously, we've reached the last boundary so there can't be any runs after this!
    unsigned long run_stop = interval_bounds[i].first;
    multiplicities[run_mult - 1].emplace_back(run_start);
    multiplicities[run_mult - 1].emplace_back(run_stop);
    std::cout << "Made run (" << run_start << ", " << run_stop << ") with multiplicity " << run_mult << std::endl;

    return multiplicities;
}

Range Zealand::getRangeStart(unsigned long block, int depth)
{
    unsigned long smallest_child = getSmallestChild(block,depth);

    return Range({smallest_child,0});
}

Range Zealand::getRangeStop(unsigned long block, int depth)
{
    unsigned long largest_child = getLargestChild(block,depth);
    return Range({largest_child,1});
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

            appendChildren(blockset[i], collapsed, depth);
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