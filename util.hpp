#ifndef util_hpp
#define util_hpp

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

// Convenience templates
using Vector3 = gte::Vector3<double>;
using AlignedBox3 = gte::AlignedBox3<double>;
using Sphere3 = gte::Sphere3<double>;
using Blockset = std::vector<unsigned long>;
using Block8 = std::array<unsigned long,8>;
using Coverage = std::array<Blockset,2>;
using Range = std::pair<unsigned long, bool>;
using Rangeset = std::vector<Range>;
using Interval = std::array<unsigned long, 2>;
using Intervalset = std::vector<Interval>;

namespace libZealand
{
    const int MAX_LEVEL = 20;

    inline int getLevel(unsigned long block)
    {
        return MAX_LEVEL - __builtin_clzl(block)/3;
    }

    // AND with terminator at level to add terminator bit
    // XOR with terminator at level to remove terminator bit
    inline unsigned long terminator(int level)
    {
        return 8ul << 3*level;
    }

    // Convert range representation to points
    // level is kept the same
    // assumes range set is still ordered start, stop, ... , start, stop
    // as if it were just produced by toIntervalBounds function
    inline Blockset collapse(const Rangeset& rangeset)
    {
        Blockset blockset;
        for (int i = 0; i < rangeset.size() - 1; i += 2)
        {
            for (unsigned long j = rangeset[i].first; j <= rangeset[i+1].first; j++)
            {
                blockset.push_back(j);
            }
        }

        // Q: RVO performed here?
        return blockset;
    }

    inline unsigned long set3NBits(int n)
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

    inline unsigned long getSmallestChild(unsigned long block, int depth)
    {
        return block << 3*depth;
    }

    inline unsigned long getLargestChild(unsigned long block, int depth)
    {
        return (block << 3*depth) | set3NBits(depth);
    }

    inline Range getRangeStart(unsigned long block, int depth)
    {
        unsigned long smallest_child = getSmallestChild(block,depth);

        return Range({smallest_child,0});
    }

    inline Range getRangeStop(unsigned long block, int depth)
    {
        unsigned long largest_child = getLargestChild(block,depth);
        return Range({largest_child,1});
    }

    inline Rangeset toIntervalBounds(const Blockset& blockset)
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

    inline Block8 getChildren(unsigned long block)
    {
        Block8 children;

        // Shift left by one level and OR with child number
        block = block << 3;
        for (unsigned long i = 0; i < 8; i++)
            children[i] = block | i;

        return children;
    }

    inline void appendChildren(unsigned long block, Blockset& blockset, int depth)
    {
        unsigned long smallest_child = getSmallestChild(block,depth);
        unsigned long largest_child = getLargestChild(block,depth);

        for (unsigned long i = smallest_child; i <= largest_child; i++)
            blockset.push_back(i);
    }

    inline Blockset collapse(const Blockset& blockset, int level)
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

    inline Rangeset intervalSetToRangeSet(const Intervalset& intervals)
    {
        Rangeset ranges;

        for (int i = 0; i < intervals.size(); i++)
        {
            ranges.push_back({intervals[i][0],0}); // open paren
            ranges.push_back({intervals[i][1],1}); // close paren
        }

        return ranges;
    }

    inline std::vector<std::vector<unsigned long>> toIntervals(const Rangeset& interval_bounds)
    {
        int max_multiplicity = interval_bounds.size()/2;
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

    inline unsigned long locateRegion(unsigned long lower_block, unsigned long upper_block)
    {
        //std::cout << std::bitset<64>(lower_block) << std::endl;
        unsigned long xor_bounds = lower_block ^ upper_block;
        //std:: cout << std::bitset<64>(common) << std::endl;
        //int shift_amt = 3*(getLevel(xor_bounds) + 1);
        int num_matching = (__builtin_clzl(xor_bounds) - 1)/3; // number of matching levels
        int shift_amt = 63 - num_matching*3;
        //std::cout << shift_amt << std::endl;
        unsigned long common_prefix = lower_block >> shift_amt;
        return common_prefix;
    }

    inline Coverage getInitialCoverage()
    {
        // Define the initial partial blockset as the eight level-0 quadrants
        unsigned long terminator = 1 << 3;
        Blockset partial({0,1,2,3,4,5,6,7});
        for (int i = 0; i < partial.size(); i++)
        {
            partial[i] = partial[i] | terminator;
        }

        Blockset full;

        Coverage coverage({partial,full});
        return coverage;
    }

    inline unsigned long encode(unsigned int x, unsigned int y, unsigned int z)
    {
        unsigned long block = libmorton::morton3D_64_encode(x,y,z);
        return block | terminator(MAX_LEVEL);
    }
}

#endif