#include "gtest/gtest.h"
#include "ConeView.hpp"
#include "RigidView.hpp"

using namespace libzealand;

// Test that clone implements proper copy semantics.
// Changes to cloned objects should not impact each other
TEST(CONE_TESTS,test_clone)
{
    RigidView* cone1 = new ConeView();
    RigidView* cone2 = cone1->clone();

    ConeView* cone1_d = dynamic_cast<ConeView*>(cone1);
    ConeView* cone2_d = dynamic_cast<ConeView*>(cone2);

    EXPECT_TRUE(cone1_d->cone.ray.direction == cone2_d->cone.ray.direction);

    cone1->updatePose(1,2,3,4,5,6,7,8,9,10,11,12);

    EXPECT_FALSE(cone1_d->cone.ray.direction == cone2_d->cone.ray.direction);

    delete(cone1);
    delete(cone2);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
