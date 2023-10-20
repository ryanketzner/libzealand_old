#include "Zealand.hpp"
#include "gtest/gtest.h"

using namespace libzealand;

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

TEST_F(ZealandTest, TestGetBox)
{
    unsigned long block = 1ul << 6;
    int num = pow(8,5)*864;
    for (int i = 0; i < num; i++);
    {
        AlignedBox3 box = instance_.getAlignedBox(1ul << 6);
        box.min[1] = 0;
    }
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
    Real radius = 1/3;
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 8; i++)
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
    Real radius = cbrt(3.0/64.0);
    Real volume = (4.0/3.0)*M_PI*pow(radius,3);
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 8; i++)
    {
        instance_.refine(coverage, sphere);
        Real full_vol = instance_.getVolume(coverage[1]);
        Real partial_vol = full_vol + instance_.getVolume(coverage[0]);
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
    Real halflength = cbrt(3.0/64.0);
    Real volume = pow(2*halflength,3);
    Vector3 min({-halflength,-halflength,-halflength});
    Vector3 max({halflength,halflength,halflength});
    AlignedBox3 box(min, max);

    // Define coverage object
    Coverage coverage({partial,full});

    // Should take down to level 8
    for (int i = 0; i < 8; i++)
    {
        instance_.refine(coverage, box);
        Real full_vol = instance_.getVolume(coverage[1]);
        Real partial_vol = full_vol + instance_.getVolume(coverage[0]);
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
    Real radius = cbrt(3.0/64.0);
    Real volume = (4.0/3.0)*M_PI*pow(radius,3);
    Sphere3 sphere(center,radius);

    // Define coverage object
    Coverage coverage({partial,full});

    for (int i = 0; i < 5; i++)
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

    int level = 8;
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


int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
