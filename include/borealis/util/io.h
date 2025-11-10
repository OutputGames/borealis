#if !defined(IO_H)
#define IO_H

#include <string>
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

namespace brl
{
    struct IoFile
    {
        std::string filePath;
        unsigned char* data;
        uint32_t dataSize;
    };

    struct IoEngine
    {
        static IoEngine* engine;

        std::map<std::string, IoFile> file_map;
        IoEngine();

        void print(std::string s);
        IoFile readFileBinary(std::string path);
        std::string readFileString(std::string path);
    };

    inline void print(std::string s) { IoEngine::engine->print(s); }
    inline IoFile readFileBinary(std::string path) { return IoEngine::engine->readFileBinary(path); }
    inline std::string readFileString(std::string path) { return IoEngine::engine->readFileString(path); }


} // namespace brl


#endif // IO_H
