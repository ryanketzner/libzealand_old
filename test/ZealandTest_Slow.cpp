#include "Zealand.hpp"
#include "gtest/gtest.h"

using namespace libZealand;


// Define a test fixture class
class ZealandTest : public ::testing::Test
{
    protected:
        ZealandTest() : 
        instance_(16000.0, 16000.0, 16000.0)
        {
        }

        Zealand instance_;
};

// Check that two identical spheres create only
// coverage of multiplicity two. Verify that the volume
// of the resulting z-curve representation is the same as the
// original AABB representation
TEST_F(ZealandTest, Testy)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;


    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();

    for (int i = 0; i < 3; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
    }
    int level = getLevel(initial_1[0][0]);
    std::cout << level << std::endl;

    std::cout << instance_.block_sizes[0][level] << std::endl;

    double volume_blockset = instance_.getVolume(initial_1[1]);
    Rangeset set_1 = toIntervalBounds(initial_1[1]);
    Rangeset set_2 = toIntervalBounds(initial_2[1]);

    //EXPECT_TRUE(std::is_sorted(set_1.begin(), set_1.end()));
    //EXPECT_TRUE(std::is_sorted(set_2.begin(), set_2.end()));
    std::sort(set_1.begin(), set_1.end());
    std::sort(set_2.begin(),set_2.end());

    Rangeset combined;
    std::merge(set_1.begin(),set_1.end(),set_2.begin(),set_2.end(),std::back_inserter(combined));

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);
    std::vector<std::vector<unsigned long>> expected_multiplicities({{}});

    double volume = instance_.getVolume(multiplicities[1],1);
    EXPECT_EQ(volume_blockset,volume);

    //EXPECT_EQ(multiplicities,expected_multiplicities);
}
 
int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}