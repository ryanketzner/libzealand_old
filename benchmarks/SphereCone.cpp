#include <string>
#include <vector>
#include <stdlib.h>
#include <valgrind/callgrind.h>

#include "VolumeFOV.hpp"
#include "Zealand.hpp"
#include "ConeView.hpp"
#include "SphereView.hpp"

int main(int argc, char *argv[])
{
    int level = atoi(argv[1]);

    const double scale = 10.0;
    Zealand octree(scale);

    Vector3 center({0.0,0.0,0.0});
    Vector3 dir({1.0,0.0,0.0});
    double radius = 3.0;

    std::vector<VolumeFOV*> shapes;
    std::vector<VolumeFOV*> not_shapes;

    // Add shapes to representation
    VolumeFOV* cone = new ConeView();
    VolumeFOV* sphere = new SphereView(center,radius);
    not_shapes.push_back(cone);
    shapes.push_back(sphere);

    CALLGRIND_START_INSTRUMENTATION;
    Coverage cov = octree.refine(shapes,not_shapes,level);
    CALLGRIND_STOP_INSTRUMENTATION;

    delete(cone);
    delete(sphere);

    return 0;
}