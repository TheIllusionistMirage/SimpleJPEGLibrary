#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>

namespace jpeg
{

bool openFile( std::ifstream& file, const std::string& filename, bool binary = false );

bool readPPMFile( std::ifstream& file, Image& image );

bool readJFIFFile( std::ifstream& file );

}

#endif // FILE_HPP
