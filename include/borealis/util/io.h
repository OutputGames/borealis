#if !defined(IO_H)
#define IO_H

#include <string>
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace brl
{


    struct IoBinaryData
    {
        unsigned char* mem;
        size_t size;
    };

    struct IoEngine
    {
        static IoEngine* engine;

        void print(std::string s);
        IoBinaryData readFileBinary(std::string path);
        std::string readFileString(std::string path);
    };

    inline void print(std::string s) { IoEngine::engine->print(s); }
    inline IoBinaryData readFileBinary(std::string path) { return IoEngine::engine->readFileBinary(path); }
    inline std::string readFileString(std::string path) { return IoEngine::engine->readFileString(path); }


} // namespace brl


#endif // IO_H
