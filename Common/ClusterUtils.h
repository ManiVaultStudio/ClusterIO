#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace utils {
    struct DataContent {
        uint32_t numClusters = 0;
        std::vector<uint32_t> clusterSizes = {};
        std::vector<std::string> clusterNames = {};
        std::vector<int32_t> clusterColors = {};
        std::vector<uint32_t> clusterIndices = {};

        std::string parentName = {};
        uint32_t parentNumPoints = 0;
    };

    template <typename T>
    void readVal(T& num, std::ifstream& in)
    {
        in.read(reinterpret_cast<char*>(&num), sizeof(T));
    }

    template <typename T>
    void writeVal(const T& num, std::ofstream& fout)
    {
        fout.write(reinterpret_cast<const char*>(&num), sizeof(T));
    }

    template <typename T>
    void readVec(std::vector<T>& vec, std::ifstream& in)
    {
        size_t length = 0;
        in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
        vec.resize(length);
        for (size_t i = 0; i < length; i++)
            in.read(reinterpret_cast<char*>(&vec[i]), sizeof(T));
    }

    template <typename T>
    void writeVec(const std::vector<T>& vec, std::ofstream& fout)
    {
        const size_t length = vec.size();
        fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
        fout.write(reinterpret_cast<const char*>(vec.data()), length * sizeof(T));
    }

    static void readString(std::string& s, std::ifstream& in)
    {
        size_t length = 0;
        in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
        s.resize(length);
        in.read(&s[0], length);
    };

    static void writeString(const std::string& s, std::ofstream& fout)
    {
        const size_t length = s.size();
        fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
        fout.write(reinterpret_cast<const char*>(s.c_str()), length);
    };

    static void readVecOfStrings(std::vector<std::string>& vec, std::ifstream& in)
    {
        size_t length = 0;
        in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
        vec.resize(length);
        for (size_t i = 0; i < length; i++) {
            readString(vec[i], in);
        }
    }

    static void writeVecOfStrings(const std::vector<std::string>& vec, std::ofstream& fout)
    {
        const size_t length = vec.size();
        fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
        for (const std::string& s : vec) {
            writeString(s, fout);
        }
    }

} // utils
