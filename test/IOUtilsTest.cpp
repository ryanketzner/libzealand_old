#include <array>

#include "IOUtils.hpp"
#include "util.hpp"
#include "gtest/gtest.h"
#include "SphereView.hpp"
#include "GTEFOV.hpp"

using namespace libzealand;

TEST(IOUtils,test_input_to_numeric)
{
    const std::string line("1, 3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4\n");
    std::pair<unsigned int,std::array<double,6>> result = IOUtils::input_to_numeric<6>(line);

    std::pair<unsigned int,std::array<double,6>> expected({1,{3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4}});
    EXPECT_EQ(result,expected);
}

TEST(IOUtils,test_inputs_to_numeric)
{
    const std::size_t num_lines = 3;
    const std::size_t num_items = 6;
    const std::string line1("1, 3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4\n");
    const std::string line2("1, 3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4\n");
    const std::string line3("1, 3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4\n");
    std::array<std::string,num_lines> lines({line1,line2,line3});

    std::array<std::pair<unsigned int,std::array<double,num_items>>,num_lines> result = IOUtils::inputs_to_numeric<num_items, num_lines>(lines);

    std::pair<unsigned int,std::array<double,6>> expected1({1,{3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4}});
    std::pair<unsigned int,std::array<double,6>> expected2({1,{3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4}});
    std::pair<unsigned int,std::array<double,6>> expected3({1,{3.5694, 3453, 43345.44, 4543.0, 34534.4, 34534.4}});
    std::array<std::pair<unsigned int,std::array<double,num_items>>,num_lines> expected({expected1,expected2,expected3});

    EXPECT_EQ(result,expected);
}

TEST(IOUtils, test_read_n_lines)
{
  const std::size_t num_lines = 10;
  const std::size_t num_items = 6;
  std::string filename = std::string(PROJECT_ROOT_DIR) + "/test/input/state_cartesian.csv";

  std::ifstream ifs(filename);
  std::vector<std::string> lines = IOUtils::read_n_lines(ifs, num_lines);

  EXPECT_EQ(lines.size(),num_lines);
}

TEST(IOUtils, test_print_blockset)
{
  std::string filename = std::string(PROJECT_ROOT_DIR) + "/build/test/output/blockset.csv";

  const Real scale = 10.0;
  Zealand octree(scale);

  Vector3 center({0.0,0.0,0.0});
  Vector3 dir({1.0,0.0,0.0});
  Real radius = 3.0;
  Real x_slice = .01;

  std::vector<VolumeFOV*> shapes;
  std::vector<VolumeFOV*> not_shapes;
  VolumeFOV* sphere = new SphereView(center,radius);
  shapes.push_back(sphere);

  // Halfspace3 cutting(center,0.0);
  //VolumeFOV* half = new GTEFOV<Halfspace3>(cutting);

  Coverage cov = octree.refine(shapes,not_shapes,3);
  
  IOUtils::print_blockset(octree,octree.alignedLeq(cov[1],0,x_slice),filename);
}

// TEST(IOUtils, test_inputs_to_numeric_and_read_n_lines)
// {
//   const std::size_t num_lines = 10;
//   const std::size_t num_items = 6;
//   std::string filename = std::string(PROJECT_ROOT_DIR) + "/test/input/state_cartesian.csv";

//   std::ifstream ifs(filename);
//   std::vector<std::string> lines = IOUtils::read_n_lines(ifs, num_lines);

  
// }

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
