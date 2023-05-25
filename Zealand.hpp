#ifndef Zealand_hpp
#define Zealand_hpp

#include "VolumeFOV.hpp"
#include "GTEFOV.hpp"
#include "util.hpp"

using namespace libzealand;

class Zealand
{
    public:

        Zealand(double scale) : 
        Zealand (scale, scale, scale)
        {
        }

        Zealand(double scale_x, double scale_y, double scale_z) :
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

        // // Pregenerate boxes up to and including "level"
        // void preprocess(unsigned int level)
        // {
        //     unsigned long largest = getLargestChild(1ul,level+1);
        //     unsigned long smallest = 1ul << 3;

        //     for (unsigned long i = smallest; i <= largest; i++)
        //     {
        //         prep_boxes.emplace_back(getAlignedBox(i));
        //     }
        //     preprocessed = true;
        // }

        Coverage refine(const std::vector<VolumeFOV*>& shapes, const std::vector<VolumeFOV*>& not_shapes, int level) const
        {
            // Initialize as partial coverage of super-block
            //Coverage initial = getInitialCoverage();
            Blockset partial({1ul});
            Blockset full;
            Coverage initial({partial,full});

            for (int i = 0; i <= level; i++)
            {
                //std::cout << i << std::endl;
                refine(initial,shapes,not_shapes);
            }

            return initial;
        }

        template<class Shape>
        Coverage refine(const Shape& shape, int level) const
        {
            // Initialize as partial coverage of super-block
            //Coverage initial = getInitialCoverage();
            Blockset partial({1ul});
            Blockset full;
            Coverage initial({partial,full});

            for (int i = 0; i <= level; i++)
            {
                refine(initial,shape);
            }

            return initial;
        }

        bool anyShapesIntersect(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes) const
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shapes intersect, return true
                if (shapes[k]->intersects(box))
                    return true;
            }
            return false;
        }

        template <typename T>
        bool allShapesIntersect(const T& t, const std::vector<VolumeFOV*>& shapes) const
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape fails to intersect, return false
                if (!shapes[k]->intersects(t))
                    return false;
            }
            return true;
        }


        bool anyShapeCovers(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes) const
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape contains, return true
                if (shapes[k]->contains(box))
                    return true;
            }
            return false;
        }

        template <typename T>
        bool allShapesCover(const T& t, const std::vector<VolumeFOV*>& shapes) const
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape fails to contain, return false
                if (!shapes[k]->contains(t))
                    return false;
            }
            return true;
        }

        void refine(Coverage& coverage, const std::vector<VolumeFOV*>& shapes, const std::vector<VolumeFOV*>& not_shapes) const
        {
            Blockset new_partial;
            new_partial.reserve(coverage[0].size()*4);

            // For each partially covered block
            for (int i = 0; i < coverage[0].size(); i++)
            {
                unsigned long block = coverage[0][i];
                //std::cout << "Block #: " << i << std::endl;
                // Generate 8 children of each partially covered block
                Block8 children = getChildren(block);

                // OPTIMIZATION CANDIDATE
                Vector3 center = getCenter(block);

                bool all_intersect = false;
                bool all_no_cover = false;
                if (allShapesCover(center,shapes))
                    all_intersect = true; // we know all intersect
                else
                    all_no_cover = true; // we not all don't cover

                // END OPTIMIZATION CANDIDATE


                // Check coverage status of each child
                for (int j = 0; j < 8; j++)
                {
                    const AlignedBox3 box = getAlignedBox(children[j]);

                    if (all_intersect)
                    {
                    }
                    else if (!allShapesIntersect(box,shapes))
                        continue;
                    

                    if (anyShapeCovers(box,not_shapes))
                        continue;

                    // At this point, all shapes intersect and
                    // no not_shape covers, so the box is at least
                    // partially contained

                    // Check whether all shapes cover box
                    if (all_no_cover || !allShapesCover(box,shapes))
                    {
                        // If any shape doesn't fully cover
                        // then the box is only partially contained
                        new_partial.push_back(children[j]);
                        continue;
                    }

                    if (anyShapesIntersect(box,not_shapes))
                    {
                        // If any not_shape intersects box
                        // then the box is only partially contained
                        new_partial.push_back(children[j]);
                        continue;
                    }

                    // Contains
                    coverage[1].push_back(children[j]);
                    //std::cout << "Finished." << std::endl;
                }
            }

            // Update partial coverage blockset
            coverage[0] = std::move(new_partial);

            return;
        }

        // Watch out for arithmetic precision errors!
        Blockset alignedSlice(const Blockset& blocks, int axis, double value) const
        {
            Blockset sliced_blocks;

            for (int i = 0; i < blocks.size(); i++)
            {
                unsigned long block = blocks[i];
                AlignedBox3 box = getAlignedBox(block);

                if (box.min[axis] < value && box.max[axis] >= value)
                    sliced_blocks.push_back(block);
            }
            return sliced_blocks;
        }

        Blockset alignedLeq(const Blockset& blocks, int axis, double value) const
        {
            Blockset result;

            for (int i = 0; i < blocks.size(); i++)
            {
                unsigned long block = blocks[i];
                AlignedBox3 box = getAlignedBox(block);

                if (box.max[axis] <= value)
                    result.push_back(block);
            }
            return result;
        }

        template<class Shape>
        void refine(Coverage& coverage, const Shape& shape) const
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

        AlignedBox3 getAlignedBox(unsigned long block) const
        {
            int level = getLevel(block);
            block = block ^ terminator(level);

            uint_fast32_t x,y,z;
            libmorton::morton3D_64_decode(block, x, y, z);

            int blocks_dim = getBlocksDim(level);
            double block_size_x = scale_x/blocks_dim;
            double block_size_y = scale_y/blocks_dim;
            double block_size_z = scale_z/blocks_dim;

            Vector3 min({x*block_size_x - scale_x/2, y*block_size_y - scale_y/2, z*block_size_z - scale_z/2});
            Vector3 max({min[0] + block_size_x, min[1] + block_size_y, min[2] + block_size_z});

            return AlignedBox3(min,max);
        }

        // Vector3 getCenter(unsigned long block)
        // {
        //     int level = getLevel(block);
        //     block = block ^ terminator(level);

        //     uint_fast32_t x,y,z;
        //     libmorton::morton3D_64_decode(block, x, y, z);

        //     int blocks_dim = getBlocksDim(level);
        //     double block_size_x = scale_x/blocks_dim;
        //     double block_size_y = scale_y/blocks_dim;
        //     double block_size_z = scale_z/blocks_dim;

        //     double center_x, center_y, center_z;
        //     center_x = (x*block_size_x + block_size_x/2) - scale_x/2;
        //     center_y = (z*block_size_y + block_size_y/2) - scale_y/2;
        //     center_z = (z*block_size_z + block_size_z/2) - scale_z/2;

        //     return Vector3({center_x, center_y, center_z});
        // }

        Vector3 getCenter(unsigned long block) const
        {
            int level = getLevel(block);
            block = block ^ terminator(level);

            uint_fast32_t x,y,z;
            libmorton::morton3D_64_decode(block, x, y, z);

            int blocks_dim = getBlocksDim(level);
            double block_size_x = scale_x/blocks_dim;
            double block_size_y = scale_y/blocks_dim;
            double block_size_z = scale_z/blocks_dim;

            Vector3 center({x*block_size_x - scale_x/2 + block_size_x/2, 
                            y*block_size_y - scale_y/2 + block_size_y/2,
                            z*block_size_z - scale_z/2 + block_size_z/2});
            
            return center;
        }

        double getArea(const Blockset& region, int axis_1, int axis_2) const
        {
            double area = 0;
            for (int i = 0; i < region.size(); i++)
            {
                int level = getLevel(region[i]);
                area = area + block_sizes[axis_1][level] * block_sizes[axis_2][level];
            }
            return area;
        }

        double getVolume(const Blockset& region) const
        {
            if (region.size() == 0)
                return 0;

            double volume = 0;
            for (int i = 0; i < region.size(); i++)
            {
                int level = getLevel(region[i]);
                volume = volume + block_sizes[0][level] * block_sizes[1][level] * block_sizes[2][level];
            }
            return volume;
        }

        double getVolume(const std::vector<unsigned long>& intervals, int multiplicity) const
        {
            if (intervals.size() == 0)
                return 0;

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

        // Public data members
        const double scale_x;
        const double scale_y;
        const double scale_z;
        double block_sizes[3][MAX_LEVEL + 1];

        // std::vector<AlignedBox3> prep_boxes;
        // bool preprocessed = false;
};

#endif