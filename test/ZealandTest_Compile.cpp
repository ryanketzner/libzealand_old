#include "Zealand.hpp"
#include "gtest/gtest.h"
#include "RigidView.hpp"
#include "SphereView.hpp"
#include "SphericalPolyView.hpp"
#include "ConeView.hpp"

using namespace libZealand;

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

TEST_F(ZealandTest, TestSphereView)
{
    Vector3 center({0.0,0.0,0.0});
    double radius = 4;
    RigidView* sphere = new SphereView(center,radius);
}

TEST_F(ZealandTest, TestSphericalPolyView)
{
    Vector3 center({1.0,5.0,7.0});

    Vector3 n1({1.6,4.9,9.0});
    Vector3 n2({4.6,9.9,8.0});
    Vector3 n3({2.6,3.9,7.0});
    Vector3 n4({7.6,6.9,2.0});
    gte::Normalize(n1);
    gte::Normalize(n2);
    gte::Normalize(n3);
    gte::Normalize(n4);

    RigidView* poly = new SphericalPolyView(center, n1, n2, n3, n4);
}

TEST_F(ZealandTest, TestConeView)
{
    Vector3 center({1.0,5.0,7.0});
    Vector3 dir({1.6,4.9,9.0});
    gte::Normalize(dir);
    double angle = M_PI/16;


    RigidView* poly = new ConeView(center,dir,angle);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
