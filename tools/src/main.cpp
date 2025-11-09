#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

int main(int argc, const char* argv[])
{
    if (argc != 3)
        return 1;

    std::string overallPath = argv[1];

    // Open a file for writing in binary mode
    std::filesystem::path outputDir = argv[2];
    std::filesystem::path outputFilePath = outputDir / "assets.res";
    std::ofstream outFile(outputFilePath, std::ios::out | std::ios::binary | std::ios::trunc);
    // Check if the file was opened successfully
    if (!outFile.is_open())
    {
        std::cerr << "Error opening file!" << std::endl;
        return -1;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(overallPath))
    {

        // Converting the path to const char * in the
        // subsequent lines
        std::filesystem::path path = entry.path();

        // Testing whether the path points to a
        // non-directory or not If it does, displays path
        if (!is_directory(path))
        {
            std::filesystem::path relativePath = relative(path, overallPath);

            std::cout << relativePath << std::endl;

            std::ifstream inFile(path, std::ios::in | std::ios::binary);

            if (!inFile.is_open())
            {
                std::cerr << "Error opening file for reading!" << std::endl;
                return -1;
            }

            // Get file size
            inFile.seekg(0, std::ios::end);
            std::streampos fileSize = inFile.tellg();
            inFile.seekg(0, std::ios::beg);

            std::string pathText = relativePath.string();

            {
                uint32_t length = static_cast<uint32_t>(pathText.size());
                outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));

                // 2. Write the actual string data
                outFile.write(pathText.c_str(), length);
            }

            {
                uint32_t length = fileSize;
                outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));

                // Write file data in chunks
                constexpr std::size_t chunkSize = 4096;
                std::vector<char> buffer(chunkSize);
                std::streamsize bytesLeft = fileSize;
                while (bytesLeft > 0)
                {
                    std::streamsize toRead = std::min(bytesLeft, static_cast<std::streamsize>(chunkSize));
                    inFile.read(buffer.data(), toRead);
                    outFile.write(buffer.data(), inFile.gcount());
                    bytesLeft -= inFile.gcount();
                }
            }

            // Close the file
            inFile.close();

        }
    }


    outFile.close();

    return 0;
}
