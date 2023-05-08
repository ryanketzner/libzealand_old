#include <string>
#include <fmt/format.h>
#include <fstream>
#include <vector>

#include "VolumeFOV.hpp"
#include "Zealand.hpp"
#include "IOUtils.hpp"
#include "ConeView.hpp"

int main(void)
{
    std::string path_full = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/cone_full";
    std::string path_partial = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/cone_partial";

    std::string term = ".csv";
    const int levels = 6;

    std::vector<std::string> filename_partial;
    std::vector<std::string> filename_full;
    for (int i = 0; i < levels; i++)
    {
        std::string partial = path_partial + std::to_string(i) + term;
        std::string full = path_full + std::to_string(i) + term;

        filename_partial.emplace_back(partial);
        filename_full.emplace_back(full);
    }

    for(int i = 0; i < levels; i++)
    {
        const double scale = 10.0;
        Zealand octree(scale);

        Vector3 center({0.0,0.0,0.0});
        Vector3 dir({1.0,0.0,0.0});
        double x_slice = .01;

        std::vector<VolumeFOV*> shapes;
        std::vector<VolumeFOV*> not_shapes;
        VolumeFOV* cone = new ConeView();
        shapes.push_back(cone);

        Coverage cov = octree.refine(shapes,not_shapes,i);
        
        IOUtils::print_blockset(octree,octree.alignedLeq(cov[0],0,x_slice),filename_partial[i]);
        IOUtils::print_blockset(octree,octree.alignedLeq(cov[1],0,x_slice),filename_full[i]);
        delete(cone);
    }
}