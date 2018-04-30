#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <fstream>

#include "Utility.hpp"
#include "Image.hpp"

namespace jpeg
{

class Encoder
{

public:

    enum class Status
    {
        SUCCESS,
        FAILED,
        TERMINATED
    };
    
    Encoder();
    
    ~Encoder();
    
    bool loadPPMFile( const std::string& filename ); // load a PPMG image file
    
    bool saveToJFIFFile( const std::string& filename ); // save encoded data to JFIF file
    
    Status encodePPMFile(); // Convert Image object to scan data (bit stream)

private:

    ///
    
    //Status readPPMFile(); // read PPM file to Image object
    
    ///
    
    Status convertColorspace(); // R-G-B to Y-Cb-Cr
    
    // we skip chroma subsamplling...
    
    //Status createMCUs(); // make 8x8 blocks, level shift
    
    Status transformMCUs(); // apply FDCT
    
    Status quantizeMCUs(); // quantization
    
    Status encodeMCUs(); // perform entropy coding: RLE & Huffman coding for AC coeffs, Huffman for DC
    
    Status byteStuffScanData(); // perform byte stuffing on scan data
    

private:

    std::string m_filename;
    std::ifstream m_PPMFile;
    Image m_image;
    std::vector<std::array<FlMatrix8x8, 3>> m_dctCoeffs;
    std::vector<std::array<IMatrix8x8, 3>> m_qtCoeffs;
    std::array<Int16, 3> m_DCDiff;
    std::string m_scanData;

};

}

#endif // ENCODER_HPP
