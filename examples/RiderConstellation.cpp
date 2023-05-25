#include "Zealand.hpp"
#include "util.hpp"
#include "VolumeFOV.hpp"
#include "ConeView.hpp"
#include "SphereView.hpp"
#include "IOUtils.hpp"

#include <algorithm>
#include <vector>

int main(void)
{
    using namespace libzealand;

    int s = 18;

    std::string path_full = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/rider_full";
    std::string path_partial = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/rider_partial";

    std::string term = ".csv";

    std::vector<std::string> filename_partial;
    std::vector<std::string> filename_full;
    std::vector<std::string> filename_full_m;
    std::vector<std::string> filename_partial_m;
    for (int i = 1; i <= s; i++)
    {
        std::string partial = path_partial + std::to_string(i) + term;
        std::string full = path_full + std::to_string(i) + term;

        std::string full_m = path_full + "_m" + std::to_string(i) + term;
        std::string partial_m = path_partial + "_m" + std::to_string(i) + term;

        filename_partial.emplace_back(partial);
        filename_full.emplace_back(full);

        filename_full_m.emplace_back(full_m);
        filename_partial_m.emplace_back(partial_m);
    }

    int level = 7;
    double y_slice = .01;
    const double deg2rad = M_PI/180.0;
    // full cone angle
    double zeta = 11.63 * deg2rad;

    // constellation parameters

    // in-plane angle spacing
    double theta = (360.0/18) * deg2rad;

    double h_s = 1883;
    double h_t = 100;
    double h_T = 200;
    double h_T_prime = 1000;
    double r_e = 6378;

    double r_t = h_t + r_e;
    double r_s = h_s + r_e;
    double r_T = h_T + r_e;
    double r_T_prime = h_T_prime + r_e;
    double R_max = 6456;

    double s_cone_angle = asin(r_t/r_s); // horizon half-cone
    double b_cone_angle = s_cone_angle + zeta;

    std::vector<Blockset> fulls(s);
    std::vector<Blockset> partials(s);

    Zealand octree(20000);
    for (int i = 0; i < s; i++)
    {
        Vector3 center({0.0, 0.0, 0.0});
        Vector3 v_s({r_s*sin(i*theta),0.0,r_s*cos(i*theta)});
        Vector3 axis = -v_s; // need to normalize? 
        gte::Normalize(axis);

        // Horizon Cone
        VolumeFOV* s_cone = new ConeView(v_s, axis, s_cone_angle);
        // Sensor FOR
        VolumeFOV* b_cone = new ConeView(v_s, axis, b_cone_angle);
        VolumeFOV* range = new SphereView(v_s, R_max);

        VolumeFOV* UTAS = new SphereView(center, r_T_prime);
        VolumeFOV* LTAS = new SphereView(center, r_T);

        std::vector<VolumeFOV*> shapes({b_cone, range, UTAS});
        std::vector<VolumeFOV*> not_shapes({s_cone, LTAS});

        Coverage cov = octree.refine(shapes,not_shapes,level);

        IOUtils::print_blockset(octree,octree.alignedLeq(cov[0],1,y_slice),filename_partial[i]);
        IOUtils::print_blockset(octree,octree.alignedLeq(cov[1],1,y_slice),filename_full[i]);

        fulls[i] = std::move(cov[1]);
        //partials[i] = std::move(cov[0]);
        partials[i] = std::move(cov[0]);
        partials[i].insert(partials[i].end(),fulls[i].begin(),fulls[i].end()); //combined
    }

    std::cout << "Finished Coverage." << std::endl;

    std::vector<Blockset> full_mults = octreeMultiplicities(fulls);

    std::cout << "Finished Full Multiplicities." << std::endl;

    std::vector<Blockset> partial_mults = octreeMultiplicities(partials);

    std::cout << "Finished Partial Multiplicities" << std::endl;

    for (int i = 0; i < s; i++)
    {
        IOUtils::print_blockset(octree,octree.alignedLeq(full_mults[i],1,y_slice),filename_full_m[i]);
        IOUtils::print_blockset(octree,octree.alignedLeq(partial_mults[i],1,y_slice),filename_partial_m[i]);
    }

    std::cout << "Full Volume 1: " << octree.getVolume(full_mults[0]) << std::endl;
    std::cout << "Full Volume 2: " << octree.getVolume(full_mults[1]) << std::endl;
    std::cout << "Full Volume 3: " << octree.getVolume(full_mults[2]) << std::endl;
    std::cout << "Full Volume 4: " << octree.getVolume(full_mults[3]) << std::endl;
    std::cout << "Full Volume 5: " << octree.getVolume(full_mults[4]) << std::endl;
    std::cout << "Full Volume 6: " << octree.getVolume(full_mults[5]) << std::endl;



    return 0;
}