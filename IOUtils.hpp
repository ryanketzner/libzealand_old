#ifndef IOUTIL_HPP
#define IOUTIL_HPP

#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace IOUtils
{
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
    std::array<std::pair<unsigned int, std::array<double,array_size>>,num_lines> inputs_to_numeric(const std::array<std::string, num_lines>& string_lines)
    {
        std::array<std::pair<unsigned int, std::array<double,array_size>>,num_lines> lines;

        std::transform(string_lines.begin(),string_lines.end(),lines.begin(), input_to_numeric<array_size>);

        return lines;
    }

    template <std::size_t array_size>
    std::vector<std::pair<unsigned int, std::array<double,array_size>>> inputs_to_numeric(const std::vector<std::string>& string_lines)
    {
        std::vector<std::pair<unsigned int, std::array<double,array_size>>> lines;

        std::transform(string_lines.begin(),string_lines.end(),lines.begin(), input_to_numeric<array_size>);

        return lines;
    }

    std::vector<std::string> read_n_lines(std::ifstream& ifs, int n)
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
}

#endif