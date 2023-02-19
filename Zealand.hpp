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

        Coverage refine(const std::vector<VolumeFOV*>& shapes, const std::vector<VolumeFOV*>& not_shapes, int level)
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
        Coverage refine(const Shape& shape, int level)
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

        bool anyShapesIntersect(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes)
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shapes intersect, return true
                if (shapes[k]->intersects(box))
                    return true;
            }
            return false;
        }

        bool allShapesIntersect(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes)
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape fails to intersect, return false
                if (!shapes[k]->intersects(box))
                    return false;
            }
            return true;
        }


        bool anyShapeCovers(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes)
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape contains, return true
                if (shapes[k]->contains(box))
                    return true;
            }
            return false;
        }

        bool allShapesCover(const AlignedBox3& box, const std::vector<VolumeFOV*>& shapes)
        {
            for (int k = 0; k < shapes.size(); k++)
            {
                // if any shape fails to contain, return false
                if (!shapes[k]->contains(box))
                    return false;
            }
            return true;
        }

        void refine(Coverage& coverage, const std::vector<VolumeFOV*>& shapes, const std::vector<VolumeFOV*>& not_shapes)
        {
            Blockset new_partial;
            new_partial.reserve(coverage[0].size());

            // For each partially covered block
            for (int i = 0; i < coverage[0].size(); i++)
            {
                //std::cout << "Block #: " << i << std::endl;
                // Generate 8 children of each partially covered block
                Block8 children = getChildren(coverage[0][i]);
                // Check coverage status of each child
                for (int j = 0; j < 8; j++)
                {
                    AlignedBox3 box = getAlignedBox(children[j]);

                    if (!allShapesIntersect(box,shapes))
                        continue;
                    

                    if (anyShapeCovers(box,not_shapes))
                        continue;

                    // At this point, all shapes intersect and
                    // no not_shape covers, so the box is at least
                    // partially contained

                    // Check whether all shapes cover box
                    if (!allShapesCover(box,shapes))
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

        AlignedBox3 getAlignedBox(unsigned long block)
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

        Vector3 getCenter(unsigned long block)
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

        double getVolume(const Blockset& region)
        {
            double volume = 0;
            for (int i = 0; i < region.size(); i++)
            {
                int level = getLevel(region[i]);
                volume = volume + block_sizes[0][level] * block_sizes[1][level] * block_sizes[2][level];
            }
            return volume;
        }

        double getVolume(const std::vector<unsigned long>& intervals, int multiplicity)
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

        // Public data members
        double scale_x;
        double scale_y;
        double scale_z;
        double block_sizes[3][MAX_LEVEL + 1];
};

#endif