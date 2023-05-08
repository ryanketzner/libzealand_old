#ifndef IOUTILS_HPP
#define IOUTILS_HPP

#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <fmt/format.h>

#include "util.hpp"
#include "Zealand.hpp"

namespace IOUtils
{
    using namespace libzealand;
    // Template parameter is array size.
    // Takes a single line of a csv string as input. 
    // Returns an std::pair. The first item is an integer,
    // and the second item is an array of doubles.
    template <std::size_t array_size>
    inline std::pair<unsigned int, std::array<double,array_size>> input_to_numeric(const std::string& line)
    {
        std::array<double,array_size> values;

        std::istringstream iss(line);
        std::string token;

        std::getline(iss,token,',');
        unsigned int time = std::stoi(token);

        for (int i = 0; i < array_size; i++)
        {
            std::getline(iss, token, ',');
            double value = std::stod(token);
            values[i] = value;
        }

        return {time,values};
    }

    template <std::size_t array_size, std::size_t num_lines>
    inline std::array<std::pair<unsigned int, std::array<double,array_size>>,num_lines> inputs_to_numeric(const std::array<std::string, num_lines>& string_lines)
    {
        std::array<std::pair<unsigned int, std::array<double,array_size>>,num_lines> lines;

        std::transform(string_lines.begin(),string_lines.end(),lines.begin(), input_to_numeric<array_size>);

        return lines;
    }

    template <std::size_t array_size>
    inline std::vector<std::pair<unsigned int, std::array<double,array_size>>> inputs_to_numeric(const std::vector<std::string>& string_lines)
    {
        std::vector<std::pair<unsigned int, std::array<double,array_size>>> lines;

        std::transform(string_lines.begin(),string_lines.end(),lines.begin(), input_to_numeric<array_size>);

        return lines;
    }

    inline std::vector<std::string> read_n_lines(std::ifstream& ifs, int n)
    {
        std::vector<std::string> lines;
        if (ifs.is_open())
        {
            std::string line;

            int i = 0;
            while (i < n && std::getline(ifs,line))
            {
                lines.push_back(line);
                i++;
            }
        }
        else
        {
            throw std::runtime_error("Could not open file.");
        }

        return lines;
    }
    
    template <typename T>
    inline void append_line(const std::vector<T>& vec, std::string filename)
    {
        std::ofstream ofs(filename);

        if (ofs.is_open())
        {
            ofs << fmt::format("{}\n", fmt::join(vec, ","));
        }
    }

    inline void print_blockset(const Zealand& octree, Blockset blocks, std::string filename)
    {
        std::ofstream ofs(filename);
        std::vector<AlignedBox3> boxes(blocks.size());

        std::transform(blocks.begin(),blocks.end(),boxes.begin(), [&octree](unsigned long block){return octree.getAlignedBox(block);});


        for (int i = 0; i < boxes.size(); i++)
        {
            AlignedBox3 box = boxes[i];
            ofs << fmt::format("{},{},{},{},{},{}\n", box.min[0], box.min[1], box.min[2], box.max[0], box.max[1], box.max[2]);
        }

        ofs.close();
    }
}

#endif
