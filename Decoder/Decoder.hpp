#ifndef DECODER_HPP
#define DECODER_HPP

#include "JPEG/Utility.hpp"
#include "JPEG/File.hpp"
#include "JPEG/Image.hpp"
#include "JPEG/MCU.hpp"

namespace jpeg
{

class Decoder
{

public:

    enum class Status
    {
        SUCCESS    ,
        FAILED     ,
        TERMINATED
    };

    Decoder();
    
    ~Decoder();
    
    bool loadJFIFFile( const std::string& filename );
    
    bool saveToPPMFile( const std::string& filename );
    
    Status decodeJFIFFile();
    
    Image* getRawImage();

private:

    ///

    Status readJFIFSegment();
    
    Status readCommentSegment();
    
    Status readQuantizationTableSegment();
    
    Status readSOF0Segment();
    
    Status readHuffmanTableSegment();
    
    Status readSOSSegment();
    
    Status readScanData();
    
    ///
    
    Status removeByteStuffing();
    
    Status decodeScanData();
    
    ///
    
    Status createImage(); // putting together the MCUs, IDCT & colorspace transformation

private:

    std::string m_filename;
    std::ifstream m_JFIFFile;
    Image m_image;
    std::vector<std::vector<UInt16>> m_quantizationTable;
    HuffmanTable m_huffmanTable[2][2];
    HuffmanTree m_huffmanTree[2][2];
    std::string m_scanData;
    std::vector<MCU> m_MCU;

};

}

#endif // DECODER_HPP
