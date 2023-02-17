#include "Zealand.hpp"
#include "gtest/gtest.h"

using namespace libzealand;

// Helper functions
std::string longestPrefix(unsigned long block_1, unsigned long block_2)
{
    std::string str1 = std::bitset<64>(block_1).to_string();
    std::string str2 = std::bitset<64>(block_2).to_string();

    int i = 0;
	while(i < str2.length() && str1[i] == str2[i])
		i++;

    std::string prefix = str1.substr(0,i);
    while (prefix.size() % 3 != 1)
        prefix.pop_back();

    while (prefix.size() < 64)
        prefix.insert(0,"0");

    return prefix;
}

// Define a test fixture class
class ZealandTest : public ::testing::Test
{
    protected:
        ZealandTest() : 
        instance_(1.0,1.0,1.0)  // Replace with appropriate arguments for the constructor
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
        level = getLevel(block);
        // Expected level = i
        EXPECT_EQ(level,i);
    }
}

// I think this function is actually still broken
// needs more tests
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

        Block8 children = getChildren(block);
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
                              0b00000000000000000000000000000000000000000000000000000000001000001, 
                              0b00000000000000000000000000000000000000000000000000000000001000010,
                              0b00000000000000000000000000000000000000000000000000000000001000011,
                              0b00000000000000000000000000000000000000000000000000000000001000100,
                              0b00000000000000000000000000000000000000000000000000000000001000101,
                              0b00000000000000000000000000000000000000000000000000000000001000110,
                              0b00000000000000000000000000000000000000000000000000000000001000111}};
    Block8 children = getChildren(block);
    EXPECT_EQ(children, expected_children);
}

TEST_F(ZealandTest, TestCovers)
{
    // Define sphere
    Vector3 center({0.0,0.0,0.0});
    double radius = .23;
    Sphere3 sphere(center,radius);

    // level 2 block
    unsigned long block = 0b00000000000000000000000000000000000000000000000000000001000111111;
    AlignedBox3 box = instance_.getAlignedBox(block);

    // std::cout << box.min[0] << ", " << box.min[1] << ", " << box.min[2] << std::endl;
    // std::cout << box.max[0] << ", " << box.max[1] << ", " << box.max[2] << std::endl;

    bool covered = instance_.covers(sphere,box);
    ASSERT_EQ(covered,1);
}

TEST_F(ZealandTest, TestRefine)
{
    // Define the initial partial blockset as the eight level-0 quadrants
    unsigned long terminator = 1 << 3;
    Blockset partial({0,1,2,3,4,5,6,7});
    for (int i = 0; i < partial.size(); i++)
    {
        partial[i] = partial[i] | terminator;
    }

    Blockset full;

    // Define sphere of radius 1/3
    Vector3 center({0.0,0.0,0.0});
    double radius = 1/3;
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(coverage, sphere);
        // std::cout << "Covered size: " << coverage[1].size() << std::endl;
        // std::cout << "Partial size: " << coverage[0].size() << std::endl;
    }
    EXPECT_EQ(1,1);
}

TEST_F(ZealandTest, TestGetVolume)
{
    // Define the initial partial blockset as the eight level-0 quadrants
    unsigned long terminator = 1 << 3;
    Blockset partial({0,1,2,3,4,5,6,7});
    for (int i = 0; i < partial.size(); i++)
    {
        partial[i] = partial[i] | terminator;
    }

    Blockset full;

    // Define sphere of radius cbrt(3/4)
    // Volume of resulting sphere should be pi/16
    Vector3 center({0.0,0.0,0.0});
    double radius = cbrt(3.0/64.0);
    double volume = (4.0/3.0)*M_PI*pow(radius,3);
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(coverage, sphere);
        double full_vol = instance_.getVolume(coverage[1]);
        double partial_vol = full_vol + instance_.getVolume(coverage[0]);
        // Full coverage should always be less than actual volume
        EXPECT_LE(full_vol,volume);
        // Full + partial coverage should always be greater than actual volume
        EXPECT_GE(partial_vol,volume);

        // Check that the coverage lists are sorted
        ASSERT_TRUE(std::is_sorted(coverage[0].begin(),coverage[0].end()));
        ASSERT_TRUE(std::is_sorted(coverage[1].begin(),coverage[1].end()));
        // std::cout << "Covered size: " << coverage[1].size() << std::endl;
        // std::cout << "Partial size: " << coverage[0].size() << std::endl;
        // std::cout << "Covered volume: " << instance_.getVolume(coverage[1]) << std::endl;
        // std::cout << "Partial volume: " << instance_.getVolume(coverage[0]) << std::endl;
        // std::cout << "Covered + Partial volume: " << instance_.getVolume(coverage[0]) + instance_.getVolume(coverage[1]) << std::endl;
    }
    std::cout << coverage[1].size() << std::endl;
    std::cout << coverage[0].size() << std::endl;
}

// Test with a bounding box instead of a sphere
// bounding box in this test has same length as
// sphere radius in previous test
TEST_F(ZealandTest, TestGetVolume_2)
{
    // Define the initial partial blockset as the eight level-0 quadrants
    unsigned long terminator = 1 << 3;
    Blockset partial({0,1,2,3,4,5,6,7});
    for (int i = 0; i < partial.size(); i++)
    {
        partial[i] = partial[i] | terminator;
    }

    Blockset full;

    // Define sphere of radius cbrt(3/4)
    // Volume of resulting sphere should be pi/16
    Vector3 center({0.0,0.0,0.0});
    double halflength = cbrt(3.0/64.0);
    double volume = pow(2*halflength,3);
    Vector3 min({-halflength,-halflength,-halflength});
    Vector3 max({halflength,halflength,halflength});
    AlignedBox3 box(min, max);

    // Define coverage object
    Coverage coverage({partial,full});

    // Should take down to level 8
    for (int i = 0; i < 7; i++)
    {
        instance_.refine(coverage, box);
        double full_vol = instance_.getVolume(coverage[1]);
        double partial_vol = full_vol + instance_.getVolume(coverage[0]);
        // Full coverage should always be less than actual volume
        EXPECT_LE(full_vol,volume);
        // Full + partial coverage should always be greater than actual volume
        EXPECT_GE(partial_vol,volume);

        // Check that the coverage lists are sorted
        ASSERT_TRUE(std::is_sorted(coverage[0].begin(),coverage[0].end()));
        ASSERT_TRUE(std::is_sorted(coverage[1].begin(),coverage[1].end()));

        // std::cout << "Covered size: " << coverage[1].size() << std::endl;
        // std::cout << "Partial size: " << coverage[0].size() << std::endl;
        // std::cout << "Covered volume: " << instance_.getVolume(coverage[1]) << std::endl;
        // std::cout << "Partial volume: " << instance_.getVolume(coverage[0]) << std::endl;
        // std::cout << "Covered + Partial volume: " << instance_.getVolume(coverage[0]) + instance_.getVolume(coverage[1]) << std::endl;
    }
}

TEST_F(ZealandTest, TestSet3NBits)
{
    unsigned long expected_1 = 0b111;
    unsigned long expected_2 = 0b111111;
    unsigned long expected_3 = 0b111111111;
    unsigned long expected_7 = 0b111111111111111111111;

    EXPECT_EQ(expected_1, set3NBits(1));
    EXPECT_EQ(expected_2, set3NBits(2));
    EXPECT_EQ(expected_3, set3NBits(3));
    EXPECT_EQ(expected_7, set3NBits(7));
}

TEST_F(ZealandTest, TestGetLargestChild)
{
    // A level-0 block
    unsigned long block = 0b1011;

    unsigned long expected_1 = 0b1011111;
    unsigned long expected_2 = 0b1011111111;
    unsigned long expected_3 = 0b1011111111111;

    EXPECT_EQ(expected_1, getLargestChild(block,1));
    EXPECT_EQ(expected_2, getLargestChild(block,2));
    EXPECT_EQ(expected_3, getLargestChild(block,3));
}

TEST_F(ZealandTest, TestGetSmallestChild)
{
    // A level-0 block
    unsigned long block = 0b1011;
    unsigned long expected_1 = 0b1011000;
    unsigned long expected_2 = 0b1011000000;
    unsigned long expected_3 = 0b1011000000000;

    EXPECT_EQ(expected_1, getSmallestChild(block,1));
    EXPECT_EQ(expected_2, getSmallestChild(block,2));
    EXPECT_EQ(expected_3, getSmallestChild(block,3));
}

TEST_F(ZealandTest, TestAppendChildren)
{
    // Level-0 bottom-left block
    unsigned int block = 1 << 3;

    // Generate 1 level of children
    int levels = 1;
    Blockset children;
    appendChildren(block, children, levels);

    // There should be 8^1 = 8 children
    EXPECT_EQ(children.size(), pow(8, levels));

    // Generate 4 levels of children
    levels = 4;
    Blockset children_4;
    appendChildren(block, children_4, levels);

    // There should be 8^4 = 4096 children
    int expected_children = pow(8, levels);

    EXPECT_EQ(children_4.size(), expected_children);

    // Level-3 bottom-left block
    block = block << 3*3;
    levels = 1;
    children.clear();
    appendChildren(block, children, levels);

    // There should be 8^1 = 8 children
    expected_children = pow(8, levels);
    EXPECT_EQ(children.size(), expected_children);
}

// TEST_F(ZealandTest, TestAppendChildren)
// {
//     // Level-0 bottom-left block
//     unsigned int block = 1 << 3;

//     // Generate 1 level of children
//     int levels = 1;
//     Blockset children;
//     appendChildren(block, children, levels);

//     // There should be 8^1 = 8 children
//     EXPECT_EQ(children.size(), pow(8, levels));

//     // Generate 4 levels of children
//     levels = 4;
//     Blockset children_4;
//     appendChildren(block, children_4, levels);

//     // There should be 8^1 + 8^2 + 8^3 + 8^4 = 8 + 64 + 512 + 4096 = 4680 children
//     int expected_children = 0;
//     for (int i = 1; i <= levels; i++)
//         expected_children += pow(8,i);

//     EXPECT_EQ(children_4.size(), expected_children);
// }

TEST_F(ZealandTest, TestCollapse)
{
    // Define the initial partial blockset as the eight level-0 quadrants
    unsigned long terminator = 1 << 3;
    Blockset partial({0,1,2,3,4,5,6,7});
    for (int i = 0; i < partial.size(); i++)
    {
        partial[i] = partial[i] | terminator;
    }

    Blockset full;

    // Define sphere of radius cbrt(3/64)
    // Volume of resulting sphere should be pi/16
    Vector3 center({0.0,0.0,0.0});
    double radius = cbrt(3.0/64.0);
    double volume = (4.0/3.0)*M_PI*pow(radius,3);
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 4; i++)
    {
        instance_.refine(coverage, sphere);

        // Check that the coverage lists are sorted
        ASSERT_TRUE(std::is_sorted(coverage[0].begin(),coverage[0].end()));
        ASSERT_TRUE(std::is_sorted(coverage[1].begin(),coverage[1].end()));

        // std::cout << "Covered size: " << coverage[1].size() << std::endl;
        // std::cout << "Partial size: " << coverage[0].size() << std::endl;
        // std::cout << "Covered volume: " << instance_.getVolume(coverage[1]) << std::endl;
        // std::cout << "Partial volume: " << instance_.getVolume(coverage[0]) << std::endl;
        // std::cout << "Covered + Partial volume: " << instance_.getVolume(coverage[0]) + instance_.getVolume(coverage[1]) << std::endl;
    }

    int level = 7;
    Blockset partial_9 = collapse(coverage[0],level);
    Blockset full_9 = collapse(coverage[1],level);

    // Volume after collapse should stay the same
    EXPECT_EQ(instance_.getVolume(coverage[0]), instance_.getVolume(partial_9));
    EXPECT_EQ(instance_.getVolume(coverage[1]), instance_.getVolume(full_9));

    // Centers of all full cells should be inside sphere
    for (int i = 0; i < full_9.size(); i++)
    {
        // getCenter is broken! fix tomorrow
        //Vector3 center = instance_.getCenter(full_9[i]);
        AlignedBox3 boxy = instance_.getAlignedBox(full_9[i]);
        Vector3 center;
        Vector3 extent;
        boxy.GetCenteredForm(center,extent);
        EXPECT_TRUE(gte::InContainer(center,sphere));
    }

    std::cout << partial_9.size() << std::endl;
    std::cout << full_9.size() << std::endl;

    // // Every cell should be level 9
    // for (int i = 0; i < partial_9.size(); i++)
    //     EXPECT_EQ(getLevel(partial_9[i]),level);

    // for (int i = 0; i < full_9.size(); i++)
    //     EXPECT_EQ(getLevel(full_9[i]),level);
}

// This is not a real test
TEST_F(ZealandTest, TestToIntervals)
{
    int max_multiplicity = 5;

    //Rangeset interval_bounds({{0,0},{0,0},{3,1},{4,0},{5,0},{5,1},{6,0},{6,1},{7,1},{15,0},{15,1},{15,1}});
    Rangeset interval_bounds({{0,0},{3,1},{4,0},{5,0},{5,1},{6,0},{6,1},{7,1},{9,0},{9,1},{15,0},{15,1}});


    toIntervals(interval_bounds);

    EXPECT_TRUE(1);
}

TEST_F(ZealandTest, TestToIntervals_2)
{
    Intervalset intervals({{0,45},{0,7},{7,30},{29,40},{0,5},{0,6},{0,11},{50,51}});
    Rangeset interval_bounds = intervalSetToRangeSet(intervals);
    int max_multiplicity = intervals.size();
    std::sort(interval_bounds.begin(),interval_bounds.end());

    std::vector<std::vector<unsigned long>> multiplicities;
    multiplicities = toIntervals(interval_bounds);

    std::vector<std::vector<unsigned long>> expected_multiplicities({{41,45,50,51},{12,28,31,40},{8,11,29,30},{6,7},{0,5},{},{},{}});

    EXPECT_EQ(multiplicities, expected_multiplicities);
}

TEST_F(ZealandTest, TestToIntervals_3)
{
    Intervalset intervals({{0,45},{0,7},{7,30},{29,40},{0,5},{0,6},{0,11},{50,51},{4,7},{5,5}});
    Rangeset interval_bounds = intervalSetToRangeSet(intervals);
    int max_multiplicity = intervals.size();
    std::sort(interval_bounds.begin(),interval_bounds.end());

    std::vector<std::vector<unsigned long>> multiplicities;
    multiplicities = toIntervals(interval_bounds);

    std::vector<std::vector<unsigned long>> expected_multiplicities({{41, 45, 50, 51 },{ 12, 28, 31, 40 },{ 8, 11, 29, 30 },{},{ 0, 3, 6, 7 },{ 4, 4 },{ 5, 5 },{},{},{}});

    EXPECT_EQ(multiplicities, expected_multiplicities);
}

TEST_F(ZealandTest, TestToIntervalBounds)
{
    // A somewhat arbitrary set of blocks 
    // smallest block is level 2
    Blockset blockset({0b1000,0b1000000,0b1000001,0b1000001001,0b1000001010,0b1000101101});
    double volume = instance_.getVolume(blockset);

    Rangeset ranges = toIntervalBounds(blockset);

    double block_len = .125;
    double block_vol = pow(.125,3);
    double vol = 0;
    for (int i = 0; i < ranges.size() - 1; i = i + 2)
    {
        int num = (ranges[i+1].first - ranges[i].first) + 1;
        vol += num*block_vol;
    }

    EXPECT_EQ(volume,vol);
}

// Create an arbitrary blockset, convert to interval bounds,
// then collapse the interval bounds and verify the resulting
// blockset has the same volume as the original
TEST_F(ZealandTest, TestCollapse_2)
{
    Blockset blockset({0b1000,0b1000000,0b1000001,0b1000001001,0b1000001010,0b1000101101});
    double volume = instance_.getVolume(blockset);

    Rangeset ranges = toIntervalBounds(blockset);

    Blockset collapsed = collapse(ranges);
    double collapsed_volume = instance_.getVolume(collapsed); 

    EXPECT_EQ(volume,collapsed_volume);
}

// Not a complete test yet
TEST_F(ZealandTest, TestRefine_3)
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

    // Define shapes
    Vector3 center1({0.0,0.0,0.0});
    Vector3 center2({.1,.1,.1});

    double radius1 = .05;
    double radius2 = .25;
    double radius3 = .005;

    VolumeFOV* ltas = new GTEFOV<Sphere3>(Sphere3(center1,radius1));
    VolumeFOV* utas = new GTEFOV<Sphere3>(Sphere3(center1,radius2));
    VolumeFOV* sat = new GTEFOV<Sphere3>(Sphere3(center2,radius3));

    std::vector<VolumeFOV*> shapes({utas,sat});
    std::vector<VolumeFOV*> not_shapes({ltas});

    instance_.refine(coverage,shapes,not_shapes);
}

TEST_F(ZealandTest, TestLocateRegion)
{
    unsigned int x1[5] = {20,25,23545,55,0};
    unsigned int y1[5] = {23,68,323,67,0};
    unsigned int z1[5] = {23,68,323,67,0};

    unsigned int x2[5] = {98798,245,65,76,UINT32_MAX};
    unsigned int y2[5] = {767,2,44,6,UINT32_MAX};
    unsigned int z2[5] = {879,22,11,478,UINT32_MAX};

    unsigned long blocks1[5];
    unsigned long blocks2[5];

    for (int i = 0; i < 5; i++)
    {
        blocks1[i] = encode(x1[i],y1[i],z1[i]);
        blocks2[i] = encode(x2[i],y2[i],z2[i]);

        std::string expected_prefix = longestPrefix(blocks1[i],blocks2[i]);
        unsigned long prefix = locateRegion(blocks1[i],blocks2[i]);
        std::string prefix_str = std::bitset<64>(prefix).to_string();

        //std::cout << std::bitset<64>(blocks1[i]) << std::endl;
        //std::cout << std::bitset<64>(blocks2[i]) << std::endl;
        //std::cout << prefix_str << std::endl;
        EXPECT_EQ(expected_prefix, prefix_str);
    }
}


int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
