#ifndef CCWOLF_CONFIG_H
#define CCWOLF_CONFIG_H

#include <unordered_map>
#include <string>

namespace CcWolf
{
    std::unordered_map<std::string, std::string> get_config();

    void read_config(std::string path);
    void write_config(std::string path);
}

#endif // CCWOLF_CONFIG_H
