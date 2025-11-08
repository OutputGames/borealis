#include "borealis/util/io.h"

brl::IoEngine* brl::IoEngine::engine = nullptr;

void brl::IoEngine::print(std::string s)
{
    std::cout << s << std::endl;
}

brl::IoBinaryData brl::IoEngine::readFileBinary(std::string path)
{

    // Open the file in binary mode
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << path << std::endl;
        return static_cast<IoBinaryData>(nullptr);
    }

    // Get the file size
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg); // Reset file pointer to the beginning

    // Create a std::vector of unsigned char to store the file content
    std::vector<unsigned char> buffer(file_size);

    // Read the file content into the buffer
    if (file.read(reinterpret_cast<char*>(buffer.data()), file_size))
    {

        file.close(); // Close the file

        IoBinaryData d;

        d.size = buffer.size();
        d.mem = new unsigned char[buffer.size()];

        std::copy(buffer.begin(), buffer.end(), d.mem);

        return d;
    }
    std::cerr << "Error reading file: " << path << std::endl;
    return static_cast<IoBinaryData>(nullptr);
}

std::string brl::IoEngine::readFileString(std::string path)
{
    std::ifstream inputFile(path); // Open the file named "example.txt"
    std::string fileContent;

    if (inputFile.is_open())
    {
        // Check if the file was opened successfully
        std::ostringstream ss; // Create a stringstream object
        ss << inputFile.rdbuf(); // Read the entire file buffer into the stringstream
        fileContent = ss.str(); // Extract the content from the stringstream into the string
        inputFile.close(); // Close the input file stream
    }
    else
    {
        std::cerr << "Error: Could not open the file." << std::endl;
        return "";
    }

    return fileContent;
}
