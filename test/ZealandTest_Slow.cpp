#include "Zealand.hpp"
#include "gtest/gtest.h"
#include "VolumeFOV.hpp"
#include "SphereView.hpp"
#include "ConeView.hpp"

using namespace libzealand;


// Define a test fixture class
class ZealandTest : public ::testing::Test
{
    protected:
        ZealandTest() : 
        instance_(18000.0, 18000.0, 18000.0)
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

TEST_F(ZealandTest, TestHollowSphere)
{
    Vector3 center({0.0,0.0,0.0});
    double R = 16000.0/2.0;
    double r = 13000.0/2.0;

    VolumeFOV* sphere_big = new SphereView(center,R);
    VolumeFOV* sphere_small = new SphereView(center,r);

    double expected_volume = (4.0/3.0)*M_PI*(R*R*R - r*r*r);

    std::vector<VolumeFOV*> shapes({sphere_big});
    std::vector<VolumeFOV*> not_shapes({sphere_small});

    int level = 6;
    Coverage cov = instance_.refine(shapes, not_shapes, level);

    double full_volume = instance_.getVolume(cov[1]);
    double total_volume = full_volume + instance_.getVolume(cov[0]);

    EXPECT_GE(total_volume, expected_volume);
    EXPECT_LE(full_volume,expected_volume);

    // std::cout << full_volume << std::endl;
    // std::cout << expected_volume << std::endl;
    // std::cout << total_volume << std::endl;

}

// Construct a very narrow sphere sensor
// and verify that the partial and full
// blocksets sandwhich the true volume
TEST_F(ZealandTest, TestSphereSector)
{
    Vector3 center({0.0,0.0,0.0});
    Vector3 direction({0.0,0.0,1.0});
    double angle = M_PI/200.0; // .9 degrees
    double R = 16000.0/2.0;
    double r = 13000.0/2.0;

    VolumeFOV* sphere = new SphereView(center,R);
    VolumeFOV* cone = new ConeView(center,direction,angle);
    std::vector<VolumeFOV*> shapes({sphere, cone});
    std::vector<VolumeFOV*> not_shapes;

    double expected_volume = (2.0/3.0)*M_PI*R*R*R*(1-cos(angle));

    int level = 9;
    Coverage cov = instance_.refine(shapes, not_shapes, level);

    double full_volume = instance_.getVolume(cov[1]);
    double total_volume = full_volume + instance_.getVolume(cov[0]);

    EXPECT_GE(total_volume, expected_volume);
    EXPECT_LE(full_volume,expected_volume);

    //
}

TEST_F(ZealandTest, TestSphere)
{
    Vector3 center({0.0,0.0,0.0});
    double R = 16000.0/2.0;
    double r = 13000.0/2.0;


    Sphere3 sphere_big(center,R);
    double expected_volume = (4.0/3.0)*M_PI*(R*R*R + r*r*r);

    int level = 4;
    Coverage cov = instance_.refine(sphere_big, level);
}

TEST_F(ZealandTest, TestPolymorphicSphere)
{
    Vector3 center({0.0,0.0,0.0});
    double R = 16000.0/2.0;
    double r = 13000.0/2.0;

    // Static version
    Sphere3 sphere_big(center,R);
    double expected_volume = (4.0/3.0)*M_PI*(R*R*R + r*r*r);

    int level = 6;
    Coverage cov = instance_.refine(sphere_big, level);

    // Polymorphic version
    VolumeFOV* sphere_big_poly = new SphereView(center,R);
    std::vector<VolumeFOV*> shapes({sphere_big_poly});
    std::vector<VolumeFOV*> not_shapes;
    Coverage cov_poly = instance_.refine(shapes, not_shapes, level);

    EXPECT_EQ(cov_poly[0].size(),cov[0].size());
    EXPECT_EQ(cov_poly[1].size(),cov[1].size());
}
 
int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}