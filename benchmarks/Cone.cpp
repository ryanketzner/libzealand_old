#include <vector>
#include <stdlib.h>
#include <valgrind/callgrind.h>

#include "VolumeFOV.hpp"
#include "Zealand.hpp"
#include "ConeView.hpp"

int main(int argc, char *argv[])
{
    int level = atoi(argv[1]);

    const double scale = 10.0;
    Zealand octree(scale);

    Vector3 center({0.0,0.0,0.0});
    Vector3 dir({1.0,0.0,0.0});

    std::vector<VolumeFOV*> shapes;
    std::vector<VolumeFOV*> not_shapes;
    VolumeFOV* cone = new ConeView();
    shapes.push_back(cone);


    CALLGRIND_START_INSTRUMENTATION;
    Coverage cov = octree.refine(shapes,not_shapes,level);
    CALLGRIND_STOP_INSTRUMENTATION;

    delete(cone);
    return 0;
}