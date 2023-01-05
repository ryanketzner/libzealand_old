#include "Zealand.hpp"
#include "gtest/gtest.h"

using Vector3 = gte::Vector3<double>;
using AlignedBox3 = gte::AlignedBox3<double>;
using Sphere3 = gte::Sphere3<double>;
using Blockset = std::vector<unsigned long>;
using Block8 = std::array<unsigned long,8>;
using Coverage = std::array<Blockset,2>;

// Define a test fixture class
class ZealandTest : public ::testing::Test
{
    protected:
        ZealandTest() : 
        instance_(1.0)  // Replace with appropriate arguments for the constructor
        {
        }

        Zealand instance_;
};

// Define the test cases

TEST_F(ZealandTest, TestGetLevel)
{
    unsigned long block = 1;
    int level;
    for (int i = 0; i < 21; i++)
    {
        // Increase level of block by 1
        // for i = 0, block is level 0
        block = block << 3;
        level = instance_.getLevel(block);
        // Expected level = i
        EXPECT_EQ(level,i);
    }
}

TEST_F(ZealandTest, TestGetCenter)
{
    // Test bottom-left blocks at each level
    unsigned long block =  1;
    double expected_coord = 0;
    for (int i = 0; i < 21; i++)
    {
        // Increase level of block by 1
        // for i = 0, block is bottom-left block at level 0
        block = block << 3;
        expected_coord = expected_coord -.25 / pow(2,i);
        Vector3 expected_center({expected_coord, expected_coord, expected_coord});
        Vector3 center = instance_.getCenter(block);
        EXPECT_EQ(center, expected_center);
    }

    // Test top-right blocks at each level
    // Level-0 top-right block
    block =  (1 << 3) | 7;
    expected_coord = 0;
    for (int i = 0; i < 20; i++)
    {
        expected_coord = expected_coord + .25 / pow(2,i);
        Vector3 expected_center({expected_coord, expected_coord, expected_coord});
        Vector3 center = instance_.getCenter(block);
        EXPECT_EQ(center, expected_center);

        Block8 children = instance_.getChildren(block);
        block = children[7];
    }
}

TEST_F(ZealandTest, TestGetAlignedBox)
{
    long block = 0;

    // Level-0 top-right box
    AlignedBox3 expected_box_right(Vector3({0, 0, 0}), Vector3({.5, .5, .5}));
    // Level-0 top-right block
    block =  (1 << 3) | 7;
    AlignedBox3 box = instance_.getAlignedBox(block);
    EXPECT_EQ(box, expected_box_right);

    // Level-0 bottom-left box
    AlignedBox3 expected_box_left(Vector3({-.5, -.5, -.5}), Vector3({0, 0, 0}));
    // Level-0 bottom-left block
    block =  (1 << 3);
    box = instance_.getAlignedBox(block);

    double min_coord = 0;
    double max_coord = .5;
    for (int i = 0; i < 20; i++)
    {
        min_coord = min_coord + .25 / pow(2,i);
        AlignedBox3 expected_box(Vector3({min_coord,min_coord,min_coord}), Vector3({max_coord,max_coord,max_coord}));
    }

    EXPECT_EQ(box, expected_box_left);
}

TEST_F(ZealandTest, TestGetChildren)
{
    unsigned long block = 1 << 3;
    Block8 expected_children{{0b00000000000000000000000000000000000000000000000000000000001000000,
                              0b00000000000000000000000000000000000000000000000000000000001001000, 
                              0b00000000000000000000000000000000000000000000000000000000001010000,
                              0b00000000000000000000000000000000000000000000000000000000001011000,
                              0b00000000000000000000000000000000000000000000000000000000001100000,
                              0b00000000000000000000000000000000000000000000000000000000001101000,
                              0b00000000000000000000000000000000000000000000000000000000001110000,
                              0b00000000000000000000000000000000000000000000000000000000001111000}};
    Block8 children = instance_.getChildren(block);
    EXPECT_EQ(children, expected_children);
}

TEST_F(ZealandTest, TestCovers)
{
    // Define sphere
    Vector3 center({0.0,0.0,0.0});
    double radius = .23;
    Sphere3 sphere(center,radius);

    unsigned long block = 0b00000000000000000000000000000000000000000000000000000001111111000;
    AlignedBox3 box = instance_.getAlignedBox(block);

    std::cout << box.min[0] << ", " << box.min[1] << ", " << box.min[2] << std::endl;
    std::cout << box.max[0] << ", " << box.max[1] << ", " << box.max[2] << std::endl;

    bool covered = instance_.covers(sphere,box);
    ASSERT_EQ(covered,1);
}

TEST_F(ZealandTest, DISABLED_TestRefine)
{
    // Define the initial partial blockset as the eight level-0 quadrants
    unsigned long terminator = 1 << 3;
    Blockset partial({0,1,2,3,4,5,6,7});
    for (int i = 0; i < partial.size(); i++)
    {
        partial[i] = partial[i] | terminator;
        std::cout << std::bitset<64>(partial[i]) << std::endl;
    }

    Blockset full;

    // Define sphere of radius 1/3
    Vector3 center({0.001,0.001,0.001});
    double radius = .23;
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 4; i++)
    {
        instance_.refine(coverage, sphere);
        std::cout << "Covered size: " << coverage[1].size() << std::endl;
        std::cout << "Partial size: " << coverage[0].size() << std::endl;
    }

    EXPECT_EQ(1,1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
