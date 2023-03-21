#include <vector>
#include <algorithm>
#include <iterator>
#include <execution>
#include <functional>

#include "Zealand.hpp"
#include "VolumeFOV.hpp"
#include "SphereView.hpp"
#include "ConeView.hpp"

int main(void)
{
    using namespace libzealand;
    Zealand zealand(17000.0, 17000.0, 17000.0);
    Vector3 center({0.0,0.0,0.0});
    double R = 6378.0 + 2000.0;
    double r = 6378.0 + 400.0;
    double sma = 7000.0;
    Vector3 position({0.0,0.0,sma});
    double range = 5000;

    VolumeFOV* sphere_big = new SphereView(center,R);
    VolumeFOV* sphere_small = new SphereView(center,r);
    VolumeFOV* sat = new SphereView(position,range);

    std::vector<VolumeFOV*> shapes({sat,sphere_big});
    std::vector<VolumeFOV*> not_shapes({sphere_small});

    int level = 7;
    std::vector<Coverage> results(864);

    std::transform(std::execution::par_unseq,results.begin(), results.end(), results.begin(), [&](const auto&) 
    {
        return zealand.refine(shapes, not_shapes, level);
    });

    return 0;
}
