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

    std::string path_full = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/rider_full.csv";
    std::string path_partial = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/rider_partial.csv";

    int level = 7;
    double y_slice = .01;
    const double deg2rad = M_PI/180.0;
    // full cone angle
    double zeta = 11.63 * deg2rad;

    // constellation parameters
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

    Zealand octree(20000);

    Vector3 center({0.0, 0.0, 0.0});
    Vector3 v_s({0.0,0.0,r_s});
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

    IOUtils::print_blockset(octree,octree.alignedLeq(cov[0],1,y_slice),path_partial);
    IOUtils::print_blockset(octree,octree.alignedLeq(cov[1],1,y_slice),path_full);

    return 0;
}