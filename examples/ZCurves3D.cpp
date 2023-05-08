#include "morton.h"
#include <string>
#include <fmt/format.h>
#include <fstream>
#include <vector>

int main(void)
{
    std::string path = std::string(PROJECT_ROOT_DIR) + "/build/examples/output/3D_Z";
    std::string term = ".csv";
    const int levels = 6;

    std::vector<std::ofstream> ofs_vector;
    for (int i = 0; i < levels; i++)
    {
        std::string filename = path + std::to_string(i) + term;
        ofs_vector.emplace_back(std::ofstream(filename));
    }

    uint_fast32_t x,y,z;
    for(int i = 0; i < levels; i++)
    {
        int num_z = pow(8,i+1);
        for (uint_fast32_t morton = 0; morton < num_z; morton++)
        {
            libmorton::morton3D_64_decode(morton, x, y,z);
            ofs_vector[i] << fmt::format("{},{},{}\n", x,y,z);
        }
    }
}