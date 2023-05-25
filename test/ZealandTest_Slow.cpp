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
TEST_F(ZealandTest, TestZIntervals_Partial)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;


    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
    }
    int level = getLevel(initial_1[0][0]);

    double volume_blockset = instance_.getVolume(initial_1[0]);
    Rangeset set_1 = toIntervalBounds(initial_1[0]);
    Rangeset set_2 = toIntervalBounds(initial_2[0]);

    std::sort(set_1.begin(), set_1.end());
    std::sort(set_2.begin(),set_2.end());

    Rangeset combined;
    std::merge(set_1.begin(),set_1.end(),set_2.begin(),set_2.end(),std::back_inserter(combined));

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);

    double volume = instance_.getVolume(multiplicities[1],1);
    EXPECT_EQ(volume_blockset,volume);
}

// Check that two identical spheres create only
// coverage of multiplicity two. Verify that the volume
// of the resulting z-curve representation is the same as the
// original AABB representation
TEST_F(ZealandTest, TestZIntervals_Full)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;


    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
    }

    double volume_blockset = instance_.getVolume(initial_1[1]);
    Rangeset set_1 = toIntervalBounds(initial_1[1]);
    Rangeset set_2 = toIntervalBounds(initial_2[1]);

    std::sort(set_1.begin(), set_1.end());
    std::sort(set_2.begin(),set_2.end());

    Rangeset combined;
    std::merge(set_1.begin(),set_1.end(),set_2.begin(),set_2.end(),std::back_inserter(combined));

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);

    double volume = instance_.getVolume(multiplicities[1],1);
    EXPECT_EQ(volume_blockset,volume);

    //EXPECT_EQ(multiplicities,expected_multiplicities);
}

// Check that three identical spheres create only
// coverage of multiplicity 3. Verify that the volume
// of the resulting recombined z-curve representation is the same as the
// original
TEST_F(ZealandTest, TestZIntervals_Full_2)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;

    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);
    Sphere3 sphere_3(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();
    Coverage initial_3 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
        instance_.refine(initial_3, sphere_3);
    }

    double volume_blockset = instance_.getVolume(initial_1[1]);
    Rangeset set_1 = toIntervalBounds(initial_1[1]);
    Rangeset set_2 = toIntervalBounds(initial_2[1]);
    Rangeset set_3 = toIntervalBounds(initial_3[1]);

    Rangeset combined = set_1;
    combined.insert(combined.end(),set_2.begin(),set_2.end());
    combined.insert(combined.end(),set_3.begin(),set_3.end());

    std::sort(combined.begin(),combined.end());

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);

    double volume = instance_.getVolume(multiplicities[2],1);

    EXPECT_EQ(volume_blockset,volume);
}


// Check that two identical spheres create only
// coverage of multiplicity two. Verify that the volume
// of the resulting recombined octree representation is the same as the
// original octree representation
TEST_F(ZealandTest, TestRecombine_Full)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;

    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
    }
    int level = getLevel(initial_1[0][0]);

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

    Blockset recombined_blockset = recombine(multiplicities[1]);
    double volume = instance_.getVolume(recombined_blockset);

    EXPECT_EQ(volume_blockset,volume);
}

// Check that two identical spheres create only
// coverage of multiplicity two. Verify that the volume
// of the resulting recombined octree representation is the same as the
// original octree representation
TEST_F(ZealandTest, TestRecombine_Partial)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;

    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
    }

    double volume_blockset = instance_.getVolume(initial_1[0]);
    Rangeset set_1 = toIntervalBounds(initial_1[0]);
    Rangeset set_2 = toIntervalBounds(initial_2[0]);

    std::sort(set_1.begin(), set_1.end());
    std::sort(set_2.begin(),set_2.end());

    Rangeset combined;
    std::merge(set_1.begin(),set_1.end(),set_2.begin(),set_2.end(),std::back_inserter(combined));

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);

    Blockset recombined_blockset = recombine(multiplicities[1]);
    double volume = instance_.getVolume(recombined_blockset);

    EXPECT_EQ(volume_blockset,volume);
}

// Check that three identical spheres create only
// coverage of multiplicity two. Verify that the volume
// of the resulting recombined octree representation is the same as the
// original octree representation
TEST_F(ZealandTest, TestRecombine_Full_2)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 5000;

    Sphere3 sphere_1(center,radius);
    Sphere3 sphere_2(center,radius);
    Sphere3 sphere_3(center,radius);

    Coverage initial_1 = getInitialCoverage();
    Coverage initial_2 = getInitialCoverage();
    Coverage initial_3 = getInitialCoverage();

    for (int i = 0; i < 7; i++)
    {
        instance_.refine(initial_1, sphere_1);
        instance_.refine(initial_2, sphere_2);
        instance_.refine(initial_3, sphere_3);
    }

    double volume_blockset = instance_.getVolume(initial_1[1]);
    Rangeset set_1 = toIntervalBounds(initial_1[1]);
    Rangeset set_2 = toIntervalBounds(initial_2[1]);
    Rangeset set_3 = toIntervalBounds(initial_3[1]);

    Rangeset combined(set_1);
    combined.insert(combined.end(),set_2.begin(),set_2.end());
    combined.insert(combined.end(),set_3.begin(),set_3.end());

    std::sort(combined.begin(),combined.end());

    std::vector<std::vector<unsigned long>> multiplicities = toIntervals(combined);

    Blockset recombined_blockset = recombine(multiplicities[2]); // 3rd multiplicity
    double volume = instance_.getVolume(recombined_blockset);

    EXPECT_EQ(volume_blockset,volume);
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

// Test that the one-fold coverage multiplicity
// from two nested spheres (the outer ring) has the same
// volume when computed by CSG v. z-intersect method
// This is a bit tricky. Must use combined blockset for small
// sphere in multiplicity calculation.
TEST_F(ZealandTest, TestHollowSphere_Z)
{
    Vector3 center({0.0,0.0,0.0});
    double R = 16000.0/2.0;
    double r = 13000.0/2.0;

    VolumeFOV* sphere_big = new SphereView(center,R);
    VolumeFOV* sphere_small = new SphereView(center,r);

    double actual_volume = (4.0/3.0)*M_PI*(R*R*R - r*r*r);

    std::vector<VolumeFOV*> shapes({sphere_big});
    std::vector<VolumeFOV*> not_shapes({sphere_small});

    int level = 7;
    Coverage cov = instance_.refine(shapes, not_shapes, level);

    double expected_full_volume = instance_.getVolume(cov[1]);
    double expected_total_volume = expected_full_volume + instance_.getVolume(cov[0]);

    not_shapes.clear();
    Coverage cov_big= instance_.refine(shapes, not_shapes, level);

    shapes.clear();
    shapes.push_back(sphere_small);
    Coverage cov_small= instance_.refine(shapes, not_shapes, level);
    Blockset cov_small_combined = cov_small[0];
    cov_small_combined.insert(cov_small_combined.end(),cov_small[1].begin(),cov_small[1].end());

    std::vector<Blockset> mults = octreeMultiplicities({cov_big[1],cov_small_combined});
    // Volume covered with multiplicity one should be the disk.
    double full_volume = instance_.getVolume(mults[0]);

    EXPECT_EQ(full_volume, expected_full_volume);

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