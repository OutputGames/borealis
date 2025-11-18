#include "borealis/util/io.h"

brl::IoEngine* brl::IoEngine::engine = nullptr;

brl::IoEngine::IoEngine()
{
    engine = this;
    std::ifstream file("assets.res", std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open resource path");

    /*
    file_map = {};

    // infinite reading loop
    while (true)
    {
        uint32_t strLen = 0;
        file.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));

        IoFile block{};

        // --- Read string ---
        if (strLen > 0)
        {
            std::vector<char> strBuf(strLen);
            file.read(strBuf.data(), strLen);
            block.filePath.assign(strBuf.begin(), strBuf.end());
            std::replace(block.filePath.begin(), block.filePath.end(), '\\', '/');
        }

        // --- Read data size ---
        file.read(reinterpret_cast<char*>(&block.dataSize), sizeof(block.dataSize));

        // --- Read binary data ---
        if (block.dataSize > 0)
        {
            block.data = new unsigned char[block.dataSize];
            file.read(reinterpret_cast<char*>(block.data), block.dataSize);
        }

        file_map.insert({block.filePath, block});

        if (file.eof())
            break;
    }
    */
}

void brl::IoEngine::print(std::string s)
{
    std::cout << s << std::endl;
}

brl::IoFile brl::IoEngine::readFileBinary(std::string path)
{
    std::replace(path.begin(), path.end(), '\\', '/');

    std::ifstream file("assets.res", std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open resource path");

    while (true)
    {
        uint32_t strLen = 0;
        file.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));

        IoFile block{};

        // --- Read string ---
        if (strLen > 0)
        {
            std::vector<char> strBuf(strLen);
            file.read(strBuf.data(), strLen);
            block.filePath.assign(strBuf.begin(), strBuf.end());
            std::replace(block.filePath.begin(), block.filePath.end(), '\\', '/');
        }

        // --- Read data size ---
        file.read(reinterpret_cast<char*>(&block.dataSize), sizeof(block.dataSize));

        if (block.filePath == path) {

            // --- Read binary data ---
            if (block.dataSize > 0)
            {
                block.data = new unsigned char[block.dataSize];
                file.read(reinterpret_cast<char*>(block.data), block.dataSize);
            }

            return block;

            //file_map.insert({block.filePath, block});
        } else {
            file.seekg(block.dataSize, std::ios::cur);
        }

        if (file.eof())
            break;
    }

    return {"",nullptr};
}

std::string brl::IoEngine::readFileString(std::string path)
{
    return reinterpret_cast<const char*>(readFileBinary(path).data);
}
