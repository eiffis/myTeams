#pragma once

#include <vector>
#include <fstream>
#include <map>
#include "Models.hpp"

template <typename T>
bool storeVector(std::ofstream &out, std::vector<T> &vec)
{
    if (out.bad())
        return false;
    size_t size = vec.size();
    out.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    if (size)
        out.write(reinterpret_cast<const char *>(vec.data()), size * sizeof(T));
    if (out.fail())
        return false;
    return true;
}

template <typename T>
bool loadVector(std::ifstream &in, std::vector<T> &vec)
{
    if (in.bad())
        return false;
    size_t size = 0;
    in.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    if (size) {
        vec.resize(size);
        in.read(reinterpret_cast<char *>(vec.data()), size * sizeof(T));
    }
    if (in.fail())
        return false;
    return true;
}

bool storePrivateMessages(std::ofstream &out, std::map<std::string, std::vector<Reply>> &pms);
bool loadPrivateMessages(std::ifstream &in, std::map<std::string, std::vector<Reply>> &pms);