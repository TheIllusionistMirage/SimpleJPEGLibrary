#include <arpa/inet.h> // htons
#include <iostream>
#include <cmath>
#include <bitset>

#include "Markers.hpp"
#include "Constants.hpp"
#include "Encoder.hpp"

namespace jpeg
{

Encoder::Encoder() :
 m_filename(""),
 m_image{},
 m_dctCoeffs{},
 m_DCDiff{ {0, 0, 0} },
 m_scanData{""}
{
}

////////////////////////////////////////////////////////////////////////////////
    
Encoder::~Encoder()
{
}

////////////////////////////////////////////////////////////////////////////////
    
bool Encoder::loadPPMFile( const std::string& filename )
{
    m_PPMFile.open( filename, std::ios::in );
    
    if ( !m_PPMFile.is_open() || !m_PPMFile.good() )
    {
        std::cout << "Error in opening image file: \'" + filename + "\'" << std::endl;
        return false;
    }
    
    Int8 marker[2];
    Int8 byte;
    
    m_PPMFile >> marker[0] >> marker[1];
    
    if ( marker[0] != 'P' || marker[1] != '3' )
    {
        m_PPMFile.close();    
        return false;
    }
    
    std::cout << "Identified PPM marker: " << marker[0] << marker[1] << std::endl;
    
    m_PPMFile >> byte;
    
    if ( byte == '#' )
    {
        std::cout << "Comment:";
     
        do
        {
            m_PPMFile >> std::noskipws >> byte;
            std::cout << byte;
        } while ( byte != '\n' );
    }
    
    Int16 width, height;    
    m_PPMFile >> std::skipws >> width >> height;    
    
    //width = ( width / 8 ) * 8 + 8;
    //height = ( height / 8 ) * 8 + 8;
    
    std::cout << "Width: " << width << ", Height: " << height << std::endl;
    
    m_image.resize( width, height );
    
    UInt16 maxIntensity;    
    m_PPMFile >> maxIntensity;
    
    std::cout << "Max. Intensity: " << maxIntensity << std::endl;
    
    for ( UInt16 y = 0; y < height; ++y )
    {
        for ( UInt16 x = 0; x < width; ++x )
        {
            Pixel px;
            m_PPMFile >> px[0] >> px[1] >> px[2];
            
            m_image( y, x ) = px;
        }
    }
    
    /*for ( UInt16 y = 0; y < height; ++y )
    {
        for ( UInt16 x = 0; x < width; ++x )
        {
            auto px = m_image( y, x );
            std::cout << "(" << px[0] << "," << px[1] << "," << px[2] << ")  ";
        }
        std::cout << std::endl;
    }*/
    
    m_filename = filename;
    m_PPMFile.close();    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
    
bool Encoder::saveToJFIFFile( const std::string& filename )
{
    std::ofstream m_outputJPEG( filename, std::ios::out | std::ios::binary );
    
    if ( !m_outputJPEG.is_open() || !m_outputJPEG.good() )
        return false;
    
    m_outputJPEG.unsetf( std::ios::skipws );
    
    m_outputJPEG << JFIF_BYTE_FF << JFIF_SOI;
    
    m_outputJPEG << JFIF_BYTE_FF << JFIF_APP0;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x10;
    m_outputJPEG << (UInt8)0x4A << (UInt8)0x46 << (UInt8)0x49 << (UInt8)0x46 << (UInt8)0x00; // 'J', 'F', 'I', 'F', '\0'
    
    m_outputJPEG << (UInt8)0x01;
    m_outputJPEG << (UInt8)0x01;
    
    m_outputJPEG << (UInt8)0x01; // We use DPI for denoting pixel density
        
    // Write the X & Y densities
    // We set a DPI of 72 in both X & Y directions
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x48;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x48;
    
    // Write the thumbnail width & height
    // We don't encode the thumbnail data
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x00;
    
    
    ////////////////////////////////////
    // Write the comment segment
    ////////////////////////////////////
    
    // Write the comment marker
    m_outputJPEG << JFIF_BYTE_FF << JFIF_COM;
    
    //std::string comment = "Encoded with libKPEG (https://github.com/TheIllusionistMirage/libKPEG) - Easy to use baseline JPEG library";
    //std::string comment = "Created with GIMP lal alalala";
    std::string comment = "Foo barz";//", foo barz, etc. etc., blah blah...";
    
    // Write the length of the comment segment
    // NOTE: The length includes the two bytes that denote the length
    m_outputJPEG << (UInt8)( (UInt8)( ( comment.length()+2 ) >> 8 ) & (UInt8)0xFF00 ); // the first 8 MSBs
    m_outputJPEG << (UInt8)( (UInt8)( comment.length()+2 ) & (UInt8)0x00FF ); // the next 8 LSBs
            
    // Write the comment (only ASCII characters allowed)
    m_outputJPEG << comment;
    
    ////////////////////////////////////
    // Write Quantization Tables
    ////////////////////////////////////
    
    /*
     * Quantization table for luminance (Y) 
     */
    
    // Write DQT marker
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DQT;
    
    // Write the length of the DQT segment
    // NOTE: The length includes the two bytes that denote the length
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x43;
    
    // Write quantization table info
    // NOTE: Bits 7-4 denote QT#, bits 3-0 denote QT precision
    // libKPEG supports only 8-bit JPEG images, so bits 7-4 are 0
    m_outputJPEG << (UInt8)0x00;
    
    // Write the 64 entries of the QT in zig-zag order
    for ( int i = 0; i < 64; ++i )
    {
        auto index = zigzagToCoords( i );
        m_outputJPEG << (UInt8)M_QT_MAT_LUMA[index.first][index.second];
    }
    
    /**
     * Quantization table for chrominance (Cb & Cr)
     */
    
    // Write DQT marker
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DQT;
    
    // Write the length of the DQT segment
    // NOTE: The length includes the two bytes that denote the length
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x43;
    
    // Write quantization table info
    // NOTE: Bits 7-4 denote QT#, bits 3-0 denote QT precision
    // libKPEG supports only 8-bit JPEG images, so bits 7-4 are 0
    m_outputJPEG << (UInt8)0x01;
    
    // Write the 64 entries of the QT in zig-zag order
    for ( int i = 0; i < 64; ++i )
    {
        auto index = zigzagToCoords( i );
        m_outputJPEG << (UInt8)M_QT_MAT_CHROMA[index.first][index.second];
    }
    
    
    ////////////////////////////////////
    // Write start of frame 0 segment
    ////////////////////////////////////
    
    // Write SOF-0 marker identifier
    m_outputJPEG << JFIF_BYTE_FF << JFIF_SOF0;
    
    // Write SOF-0 segment length
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x11; // 8 + 3 * 3
    
    // Write data precision
    m_outputJPEG << (UInt8)0x08;
    
    // Write image dimensions
    
    // Height
    m_outputJPEG << (UInt8)( (UInt8)( m_image.height() >> 8 ) & (UInt8)0xFF00 ); // the first 8 MSBs
    m_outputJPEG << (UInt8)( (UInt8)m_image.height() & (UInt8)0x00FF ); // the next 8 LSBs
    
    // Width
    m_outputJPEG << (UInt8)( (UInt8)( m_image.width() >> 8 ) & (UInt8)0xFF00 ); // the first 8 MSBs
    m_outputJPEG << (UInt8)( (UInt8)m_image.width() & (UInt8)0x00FF ); // the next 8 LSBs
    
    // Write the number of components
    // NOTE: For now, libKPEG doesn't actually remove the chroma components; they're just set to all 0s
    m_outputJPEG << (UInt8)0x03;
    
    // Write component info for each of the components (each component takes 3 bytes)
    
    // Luminance (Y)
    m_outputJPEG << (UInt8)0x01; // Component ID (Y=1, Cb=2, Cr=3)
    m_outputJPEG << (UInt8)0x11; // Sampling factors (Bits 7-4: Horizontal, Bits 3-0: Vertical)
    m_outputJPEG << (UInt8)0x00; // Quantization table #
    
    // Chrominance (Cb)
    m_outputJPEG << (UInt8)0x02; // Component ID (Y=1, Cb=2, Cr=3)
    m_outputJPEG << (UInt8)0x11; // Sampling factors (Bits 7-4: Horizontal, Bits 3-0: Vertical)
    m_outputJPEG << (UInt8)0x01; // Quantization table #
    
    // Chrominance (Cr)
    m_outputJPEG << (UInt8)0x03; // Component ID (Y=1, Cb=2, Cr=3)
    m_outputJPEG << (UInt8)0x11; // Sampling factors (Bits 7-4: Horizontal, Bits 3-0: Vertical)
    m_outputJPEG << (UInt8)0x01; // Quantization table #
    
    ////////////////////////////////////
    // Write DHT segments
    ////////////////////////////////////
    
    // Luminance, DC HT
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DHT;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x1F; // DHT segment length (including the length bytes)
    // The symbol count for each symbol from 1-bit length to 16-bit length
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x01 << (UInt8)0x05 << (UInt8)0x01
                 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01
                 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00
                 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x01 << (UInt8)0x02
                 << (UInt8)0x03 << (UInt8)0x04 << (UInt8)0x05 << (UInt8)0x06 << (UInt8)0x07
                 << (UInt8)0x08 << (UInt8)0x09 << (UInt8)0x0A << (UInt8)0x0B;
//                  << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00
//                  << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00
//                  << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00;
    
    // Luminance, AC HT
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DHT;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0xB5; // DHT segment length (including the length bytes)
    m_outputJPEG << (UInt8)0x10 << (UInt8)0x00 << (UInt8)0x02 << (UInt8)0x01 << (UInt8)0x03
                 << (UInt8)0x03 << (UInt8)0x02 << (UInt8)0x04 << (UInt8)0x03 << (UInt8)0x05
                 << (UInt8)0x05 << (UInt8)0x04 << (UInt8)0x04 << (UInt8)0x00 << (UInt8)0x00
                 << (UInt8)0x01 << (UInt8)0x7D << (UInt8)0x01 << (UInt8)0x02 << (UInt8)0x03
                 << (UInt8)0x00 << (UInt8)0x04 << (UInt8)0x11 << (UInt8)0x05 << (UInt8)0x12
                 << (UInt8)0x21 << (UInt8)0x31 << (UInt8)0x41 << (UInt8)0x06 << (UInt8)0x13
                 << (UInt8)0x51 << (UInt8)0x61 << (UInt8)0x07 << (UInt8)0x22 << (UInt8)0x71
                 << (UInt8)0x14 << (UInt8)0x32 << (UInt8)0x81 << (UInt8)0x91 << (UInt8)0xA1
                 << (UInt8)0x08 << (UInt8)0x23 << (UInt8)0x42 << (UInt8)0xB1 << (UInt8)0xC1
                 << (UInt8)0x15 << (UInt8)0x52 << (UInt8)0xD1 << (UInt8)0xF0 << (UInt8)0x24
                 << (UInt8)0x33 << (UInt8)0x62 << (UInt8)0x72 << (UInt8)0x82 << (UInt8)0x09
                 << (UInt8)0x0A << (UInt8)0x16 << (UInt8)0x17 << (UInt8)0x18 << (UInt8)0x19
                 << (UInt8)0x1A << (UInt8)0x25 << (UInt8)0x26 << (UInt8)0x27 << (UInt8)0x28
                 << (UInt8)0x29 << (UInt8)0x2A << (UInt8)0x34 << (UInt8)0x35 << (UInt8)0x36
                 << (UInt8)0x37 << (UInt8)0x38 << (UInt8)0x39 << (UInt8)0x3A << (UInt8)0x43
                 << (UInt8)0x44 << (UInt8)0x45 << (UInt8)0x46 << (UInt8)0x47 << (UInt8)0x48
                 << (UInt8)0x49 << (UInt8)0x4A << (UInt8)0x53 << (UInt8)0x54 << (UInt8)0x55
                 << (UInt8)0x56 << (UInt8)0x57 << (UInt8)0x58 << (UInt8)0x59 << (UInt8)0x5A
                 << (UInt8)0x63 << (UInt8)0x64 << (UInt8)0x65 << (UInt8)0x66 << (UInt8)0x67
                 << (UInt8)0x68 << (UInt8)0x69 << (UInt8)0x6A << (UInt8)0x73 << (UInt8)0x74
                 << (UInt8)0x75 << (UInt8)0x76 << (UInt8)0x77 << (UInt8)0x78 << (UInt8)0x79
                 << (UInt8)0x7A << (UInt8)0x83 << (UInt8)0x84 << (UInt8)0x85 << (UInt8)0x86
                 << (UInt8)0x87 << (UInt8)0x88 << (UInt8)0x89 << (UInt8)0x8A << (UInt8)0x92 << (UInt8)0x93
                 << (UInt8)0x94 << (UInt8)0x95 << (UInt8)0x96 << (UInt8)0x97 << (UInt8)0x98
                 << (UInt8)0x99 << (UInt8)0x9A << (UInt8)0xA2 << (UInt8)0xA3 << (UInt8)0xA4
                 << (UInt8)0xA5 << (UInt8)0xA6 << (UInt8)0xA7 << (UInt8)0xA8 << (UInt8)0xA9
                 << (UInt8)0xAA << (UInt8)0xB2 << (UInt8)0xB3 << (UInt8)0xB4 << (UInt8)0xB5
                 << (UInt8)0xB6 << (UInt8)0xB7 << (UInt8)0xB8 << (UInt8)0xB9 << (UInt8)0xBA
                 << (UInt8)0xC2 << (UInt8)0xC3 << (UInt8)0xC4 << (UInt8)0xC5 << (UInt8)0xC6
                 << (UInt8)0xC7 << (UInt8)0xC8 << (UInt8)0xC9 << (UInt8)0xCA << (UInt8)0xD2
                 << (UInt8)0xD3 << (UInt8)0xD4 << (UInt8)0xD5 << (UInt8)0xD6 << (UInt8)0xD7
                 << (UInt8)0xD8 << (UInt8)0xD9 << (UInt8)0xDA << (UInt8)0xE1 << (UInt8)0xE2
                 << (UInt8)0xE3 << (UInt8)0xE4 << (UInt8)0xE5 << (UInt8)0xE6 << (UInt8)0xE7
                 << (UInt8)0xE8 << (UInt8)0xE9 << (UInt8)0xEA << (UInt8)0xF1 << (UInt8)0xF2
                 << (UInt8)0xF3 << (UInt8)0xF4 << (UInt8)0xF5 << (UInt8)0xF6 << (UInt8)0xF7
                 << (UInt8)0xF8 << (UInt8)0xF9 << (UInt8)0xFA;
    
    // Chrominance, DC HT
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DHT;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x1F;
    m_outputJPEG << (UInt8)0x01 << (UInt8)0x00 << (UInt8)0x03 << (UInt8)0x01 << (UInt8)0x01
                 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x01
                 << (UInt8)0x01 << (UInt8)0x01 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00
                 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x00 << (UInt8)0x01 << (UInt8)0x02
                 << (UInt8)0x03 << (UInt8)0x04 << (UInt8)0x05 << (UInt8)0x06 << (UInt8)0x07
                 << (UInt8)0x08 << (UInt8)0x09 << (UInt8)0x0A << (UInt8)0x0B;
    
    // Chrominance, AC HT
    m_outputJPEG << JFIF_BYTE_FF << JFIF_DHT;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0xB5; // DHT segment length (including the length bytes)
    m_outputJPEG << (UInt8)0x11 << (UInt8)0x00 << (UInt8)0x02 << (UInt8)0x01 << (UInt8)0x02
                 << (UInt8)0x04 << (UInt8)0x04 << (UInt8)0x03 << (UInt8)0x04 << (UInt8)0x07
                 << (UInt8)0x05 << (UInt8)0x04 << (UInt8)0x04 << (UInt8)0x00 << (UInt8)0x01
                 << (UInt8)0x02 << (UInt8)0x77 << (UInt8)0x00 << (UInt8)0x01 << (UInt8)0x02
                 << (UInt8)0x03 << (UInt8)0x11 << (UInt8)0x04 << (UInt8)0x05 << (UInt8)0x21
                 << (UInt8)0x31 << (UInt8)0x06 << (UInt8)0x12 << (UInt8)0x41 << (UInt8)0x51
                 << (UInt8)0x07 << (UInt8)0x61 << (UInt8)0x71 << (UInt8)0x13 << (UInt8)0x22
                 << (UInt8)0x32 << (UInt8)0x81 << (UInt8)0x08 << (UInt8)0x14 << (UInt8)0x42
                 << (UInt8)0x91 << (UInt8)0xA1 << (UInt8)0xB1 << (UInt8)0xC1 << (UInt8)0x09
                 << (UInt8)0x23 << (UInt8)0x33 << (UInt8)0x52 << (UInt8)0xF0 << (UInt8)0x15
                 << (UInt8)0x62 << (UInt8)0x72 << (UInt8)0xD1 << (UInt8)0x0A << (UInt8)0x16
                 << (UInt8)0x24 << (UInt8)0x34 << (UInt8)0xE1 << (UInt8)0x25 << (UInt8)0xF1
                 << (UInt8)0x17 << (UInt8)0x18 << (UInt8)0x19 << (UInt8)0x1A << (UInt8)0x26
                 << (UInt8)0x27 << (UInt8)0x28 << (UInt8)0x29 << (UInt8)0x2A << (UInt8)0x35
                 << (UInt8)0x36 << (UInt8)0x37 << (UInt8)0x38 << (UInt8)0x39 << (UInt8)0x3A
                 << (UInt8)0x43 << (UInt8)0x44 << (UInt8)0x45 << (UInt8)0x46 << (UInt8)0x47
                 << (UInt8)0x48 << (UInt8)0x49 << (UInt8)0x4A << (UInt8)0x53 << (UInt8)0x54
                 << (UInt8)0x55 << (UInt8)0x56 << (UInt8)0x57 << (UInt8)0x58 << (UInt8)0x59
                 << (UInt8)0x5A << (UInt8)0x63 << (UInt8)0x64 << (UInt8)0x65 << (UInt8)0x66
                 << (UInt8)0x67 << (UInt8)0x68 << (UInt8)0x69 << (UInt8)0x6A << (UInt8)0x73
                 << (UInt8)0x74 << (UInt8)0x75 << (UInt8)0x76 << (UInt8)0x77 << (UInt8)0x78
                 << (UInt8)0x79 << (UInt8)0x7A << (UInt8)0x82 << (UInt8)0x83 << (UInt8)0x84
                 << (UInt8)0x85 << (UInt8)0x86 << (UInt8)0x87 << (UInt8)0x88 << (UInt8)0x89 << (UInt8)0x8A
                 << (UInt8)0x92 << (UInt8)0x93 << (UInt8)0x94 << (UInt8)0x95 << (UInt8)0x96
                 << (UInt8)0x97 << (UInt8)0x98 << (UInt8)0x99 << (UInt8)0x9A << (UInt8)0xA2
                 << (UInt8)0xA3 << (UInt8)0xA4 << (UInt8)0xA5 << (UInt8)0xA6 << (UInt8)0xA7
                 << (UInt8)0xA8 << (UInt8)0xA9 << (UInt8)0xAA << (UInt8)0xB2 << (UInt8)0xB3
                 << (UInt8)0xB4 << (UInt8)0xB5 << (UInt8)0xB6 << (UInt8)0xB7 << (UInt8)0xB8
                 << (UInt8)0xB9 << (UInt8)0xBA << (UInt8)0xC2 << (UInt8)0xC3 << (UInt8)0xC4
                 << (UInt8)0xC5 << (UInt8)0xC6 << (UInt8)0xC7 << (UInt8)0xC8 << (UInt8)0xC9
                 << (UInt8)0xCA << (UInt8)0xD2 << (UInt8)0xD3 << (UInt8)0xD4 << (UInt8)0xD5
                 << (UInt8)0xD6 << (UInt8)0xD7 << (UInt8)0xD8 << (UInt8)0xD9 << (UInt8)0xDA
                 << (UInt8)0xE2 << (UInt8)0xE3 << (UInt8)0xE4 << (UInt8)0xE5 << (UInt8)0xE6
                 << (UInt8)0xE7 << (UInt8)0xE8 << (UInt8)0xE9 << (UInt8)0xEA << (UInt8)0xF2
                 << (UInt8)0xF3 << (UInt8)0xF4 << (UInt8)0xF5 << (UInt8)0xF6 << (UInt8)0xF7
                 << (UInt8)0xF8 << (UInt8)0xF9 << (UInt8)0xFA;
    
    ////////////////////////////////////
    // Write start of scan segment
    ////////////////////////////////////
    m_outputJPEG << JFIF_BYTE_FF << JFIF_SOS;
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x0C; // Length of SOS header
    m_outputJPEG << (UInt8)0x03; // # of components
    m_outputJPEG << (UInt8)0x01 << (UInt8)0x00; // HT info for component #1
    m_outputJPEG << (UInt8)0x02 << (UInt8)0x11; // HT info for component #2
    m_outputJPEG << (UInt8)0x03 << (UInt8)0x11; // HT info for component #3
    m_outputJPEG << (UInt8)0x00 << (UInt8)0x3F << (UInt8)0x00; // Skip bytes
    
    for ( UInt64 i = 0; i <= m_scanData.size() - 8; i += 8 )
    {
        std::bitset<8> word( m_scanData.substr( i, 8 ) );
        UInt8 w = (UInt8)word.to_ulong();
        
        m_outputJPEG << w;
        
        if ( w == JFIF_BYTE_FF )
            m_outputJPEG << (UInt8)0x00;
    }
    
    ////////////////////////////////////
    // Write end marker
    ////////////////////////////////////
    m_outputJPEG << JFIF_BYTE_FF << JFIF_EOI;
    
    m_outputJPEG.close();    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
    
Encoder::Status Encoder::encodePPMFile()
{
    convertColorspace();
    transformMCUs();
    quantizeMCUs();
    encodeMCUs();
    //byteStuffScanData();
}

////////////////////////////////////////////////////////////////////////////////
   
Encoder::Status Encoder::convertColorspace()
{
    for ( UInt16 y = 0; y < m_image.height(); ++y )
    {
        for ( UInt16 x = 0; x < m_image.width(); ++x )
        {
            rgbToYCbCr( m_image( y, x ) );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////    

Encoder::Status Encoder::transformMCUs()
{
    // Build the MCUs
    
    UInt16 mcuCount = ( m_image.width() * m_image.height() ) / 64;
    
    for ( UInt16 i = 0; i < mcuCount; ++i )
    {
        IMatrix8x8 mcu_i_Y = m_image.getMCU( i, Y );        
        IMatrix8x8 mcu_i_Cb = m_image.getMCU( i, Cb );        
        IMatrix8x8 mcu_i_Cr = m_image.getMCU( i, Cr );
        
        // Level shifting
        for ( int y = 0; y < 8; ++y )
        {
            for ( int x = 0; x < 8; ++x )
            {
                mcu_i_Y[y][x] -= 128;
                mcu_i_Cb[y][x] -= 128;
                mcu_i_Cr[y][x] -= 128;
            }
        }
        
        m_dctCoeffs.push_back( { forwardDCT( mcu_i_Y ), forwardDCT( mcu_i_Cb ), forwardDCT( mcu_i_Cr ) } );
    }
    
    std::cout << "MCU Count: " << mcuCount << std::endl;
    
    //for ( auto x = 0; m_dctCoeffs.size(); ++x )
    /*{
        for ( auto&& MCU : m_dctCoeffs[0] )
        {
            for ( auto&& v : MCU[0] )
                //for ( auto&& v : row )
                    std::cout << v << " ";
                std::cout << std::endl;
        }
        
        std::cout << std::endl;
    }*/
    
    /*for ( auto x = 0; m_dctCoeffs.size(); ++x )
    {
        for ( auto&& MCU : m_dctCoeffs )
        {
            for ( auto&& block : MCU )
                for ( auto&& row : block )
                    for ( auto&& v : row )
                        std::cout << v << " ";
                    std::cout << std::endl;
        }
        
        std::cout << std::endl;
    }*/
    
    /*for ( int z = 0; z < 4; ++z )
    {
    auto mcu = m_image.getMCU( z, Y );
    
    for ( int y = 0; y < 8; ++y )
    {
        for ( int x = 0; x < 8; ++x )
            //std::cout << m_image(y, x)[0] << "," << m_image(y, x)[1] << "," << m_image(y, x)[2] << " ";
            std::cout << mcu[y][x] << " ";
        std::cout << std::endl;
    }
    }
    */
    /*for ( int y = 0; y < 8; ++y )
    {
        for ( int x = 0; x < 8; ++x )
            std::cout << m_dctCoeffs[0][Y][y][x] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;*/
}

////////////////////////////////////////////////////////////////////////////////
    
Encoder::Status Encoder::quantizeMCUs()
{
    UInt16 mcuCount = ( m_image.width() * m_image.height() ) / 64;
    
    for ( UInt16 i = 0; i < mcuCount; ++i )
    {
        m_qtCoeffs.push_back( {} );
        
        for ( int y = 0; y < 8; ++y )
        {
            for ( int x = 0; x < 8; ++x )
            {
                m_qtCoeffs[i][Y][y][x] = std::roundf( m_dctCoeffs[i][Y][y][x] / M_QT_MAT_LUMA[y][x] );
                m_qtCoeffs[i][Cb][y][x] = std::roundf( m_dctCoeffs[i][Cb][y][x] / M_QT_MAT_LUMA[y][x] );
                m_qtCoeffs[i][Cr][y][x] = std::roundf( m_dctCoeffs[i][Cr][y][x] / M_QT_MAT_LUMA[y][x] );
            }
        }
    }
    
    /*for ( int y = 0; y < 8; ++y )
    {
        for ( int x = 0; x < 8; ++x )
            std::cout << m_qtCoeffs[0][Y][y][x] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;*/
}

////////////////////////////////////////////////////////////////////////////////
    
Encoder::Status Encoder::encodeMCUs()
{
    // Zig-zag traversal and run length coding
    UInt16 mcuCount = ( m_image.width() * m_image.height() ) / 64;
    std::vector<std::array<std::array<Int16, 64>, 3>> zz( mcuCount );
    
    for ( UInt16 i = 0; i < mcuCount; ++i )
    {
        //zz.push_back( {} );
        
        for ( int y = 0; y < 8; ++y )
        {
            for ( int x = 0; x < 8; ++x )
            {
                auto index = coordsToZigzag( y, x );
                
                zz[i][Y][index]  = m_qtCoeffs[i][Y][y][x];
                zz[i][Cb][index] = m_qtCoeffs[i][Cb][y][x];
                zz[i][Cr][index] = m_qtCoeffs[i][Cr][y][x];
            }
        }
    }
    
    /*for ( int i = 0; i < 64; ++i )
    {
        std::cout << zz[0][Y][i] << " ";
    }
    std::cout << std::endl;*/
    
    std::vector<std::array<std::vector<Int16>, 3>> rle( mcuCount );
    
    ///////////////// WORKING
    
    for ( UInt16 i = 0; i < mcuCount; ++i )
    {
        // DC coefficient isn't zero run  length coded
        
        rle[i][Y].push_back( zz[i][Y][0] );
        //rle[i][Y].push_back( 339 );
        rle[i][Cb].push_back( zz[i][Cb][0] );
        rle[i][Cr].push_back( zz[i][Cr][0] );
        //rle[i][Cr].push_back( 0 );
        
        // Check if the channel is empty, i.e., DC it self is 0
        if ( rle[i][Y][0] == 0 )
            rle[i][Y].push_back( 0 );
        
        if ( rle[i][Cb][0] == 0 )
            rle[i][Cb].push_back( 0 );
            
        if ( rle[i][Cr][0] == 0 )
            rle[i][Cr].push_back( 0 );
            
        // run-length encode the remaining AC coefficients
        
        for ( int channel = Y; channel <= Cr; ++channel )
        {
            int zeros = 0;
            
            for( UInt16 j = 1; j < 64; ++j )
            {
                if ( j == 63)
                {
                    if ( zz[i][channel][j] == 0 )
                    {
                        rle[i][channel].push_back( zeros );
                        rle[i][channel].push_back( zz[i][channel][j] );
                    }
                }
                
                if ( zz[i][channel][j] == 0 && zeros < 16 )
                {
                    zeros++;
                    //std::cout << j << ", zeros: " << zeros << std::endl;
                    continue;
                }
                
                if ( zeros == 16 )
                {
                    rle[i][channel].push_back( 15 );
                    rle[i][channel].push_back( 0 );
                    
                    zeros = 1;
                }
                else
                {
                    rle[i][channel].push_back( zeros );
                    rle[i][channel].push_back( zz[i][channel][j] );
                    
                    zeros = 0;
                }
                
                //std::cout << j << ", zeros: " << zeros << std::endl;
            }
            
            while ( rle[i][channel].size() > 1 && rle[i][channel].back() == 0 )
            {
                rle[i][channel].pop_back();
                rle[i][channel].pop_back();
            }
            
            if ( rle[i][channel].size() < 64 )
            {
                rle[i][channel].push_back( 0 );
                rle[i][channel].push_back( 0 );
                
                if ( rle[i][channel].size() == 2 )
                {
                    rle[i][channel].push_back( 0 );
                    rle[i][channel].push_back( 0 );
                }
            }
        }
        
        std::cout << "MCU: " << i << std::endl;
        for ( int channel = Y; channel <= Cr; ++channel )
        {
        std::cout << "Channel: " << channel << ", ";
        for ( auto&& e : rle[i][channel] )
            std::cout << e << " ";
        std::cout << std::endl;
        }
    }
    
    ///////////////// TEST
    
    /*rle[0][Y].push_back( -26 );
    rle[0][Y].push_back( 0 );
    rle[0][Y].push_back( 0 );
    
    rle[0][Cb].push_back( -20 );
    rle[0][Cb].push_back( 0 );
    rle[0][Cb].push_back( 0 );
    
    rle[0][Cr].push_back( 60 );
    rle[0][Cr].push_back( 0 );
    rle[0][Cr].push_back( 0 );
    
    rle[1][Y].push_back( -75 );
    rle[1][Y].push_back( 0 );
    rle[1][Y].push_back( 0 );
    
    rle[1][Cb].push_back( -40 );
    rle[1][Cb].push_back( 0 );
    rle[1][Cb].push_back( 0 );
    
    rle[1][Cr].push_back( -50 );
    rle[1][Cr].push_back( 0 );
    rle[1][Cr].push_back( 0 );
    
    rle[2][Y].push_back( -99 );
    rle[2][Y].push_back( 0 );
    rle[2][Y].push_back( 0 );
    
    rle[2][Cb].push_back( 120 );
    rle[2][Cb].push_back( 0 );
    rle[2][Cb].push_back( 0 );
    
    rle[2][Cr].push_back( -20 );
    rle[2][Cr].push_back( 0 );
    rle[2][Cr].push_back( 0 );
    
    rle[3][Y].push_back( 61 );
    rle[3][Y].push_back( 0 );
    rle[3][Y].push_back( 0 );
    
    rle[3][Cb].push_back( -100 );
    rle[3][Cb].push_back( 0 );
    rle[3][Cb].push_back( 0 );
    
    rle[3][Cr].push_back( -40 );
    rle[3][Cr].push_back( 0 );
    rle[3][Cr].push_back( 0 );*/
    
    //////////////////
    
    
    //m_DCDiff[Y] = rle[0][Y][0];
    //m_DCDiff[Cb] = rle[0][Cb][0];
    //m_DCDiff[Cr] = rle[0][Cr][0];
    
    for ( UInt16 i = 0; i < mcuCount; ++i )
    {
        std::cout << "MCU: " << i << std::endl;
        for ( int channel = Y; channel <= Cr; ++channel )
        {
            std::cout << "Channel: " << channel << std::endl;
            // First encode the DC coefficients
            
            // DC_i+1 = DC_i + difference
            //m_DCDiff[channel] -= rle[i][channel][0];
            //Int16 DC = m_DCDiff[channel] - rle[i][channel][0];
            //Int16 DC = m_DCDiff[channel];
            
            //if ( i != 0 )
            //{
                //m_DCDiff[channel] = rle[i][channel][i] - m_DCDiff[channel];
            //}
            
            //Int16 DC = m_DCDiff[channel];
            
            //int* n;
            //*n = 456;
            
            int DC;
            
            if ( i == 0 )
                DC = rle[i][channel][0];
            else
            {
                DC = rle[i][channel][0] - m_DCDiff[channel];
            }
            
            m_DCDiff[channel] = rle[i][channel][0];
            
            auto category = getValueCategory( DC );
            auto bitString = valueToBitString( DC );
            
            m_scanData += ( channel == Y ? DC_LUMA_HUFF_CODES[category] : DC_CHROMA_HUFF_CODES[category] ) + bitString;
            //std::cout << "DC: " << ", " << ( channel == Y ? DC_LUMA_HUFF_CODES[category] : DC_CHROMA_HUFF_CODES[category] ) + " " + bitString << std::endl;
            //std::cout << "DC: " << rle[i][channel][0] << std::endl;
            
            //std::cout << "DC: " << rle[i][channel][0] << std::endl;
            
            // No DC coefficient for this channel, 
            //if ( DC == 0 && rle[i][channel][0] == 0 )
                //continue;
            
            // Then encode the AC coefficients
            
            UInt16 j = ( rle[i][channel][0] == 0 ? 2 : 1 );
            //UInt16 j = 1;
            
            //std::cout << "AC: ";
            //std::cout.flush();
            
            for( ; j < rle[i][channel].size() - 1; j += 2 )
            {
                Int16 AC_i = rle[i][channel][j + 1];
                auto zeroRun = rle[i][channel][j];
                auto category = getValueCategory( AC_i );
                auto bitString = valueToBitString( AC_i );
                
                //std::cout << "(" << rle[i][channel][j] << "," << rle[i][channel][j+1] << ") ";
                //std::cout.flush();
                
                //std::cout << "AC: " << ( channel == Y ? AC_LUMA_HUFF_CODES[zeroRun][category] : AC_CHROMA_HUFF_CODES[zeroRun][category] ) + " " + bitString << std::endl;
                //std::cout << "AC: " << zeroRun << ", " << category << ", " << bitString << std::endl;
                //std::cout << "AC: " << rle[i][channel][j] << ", " << rle[i][channel][j + 1] << std::endl;
                
                //std::cout << j << ", " << j + 1 << std::endl;
                
                //std::cout << "Crash: " << zeroRun << ", " << category << ", " << bitString << std::endl;
                m_scanData += ( channel == Y ? AC_LUMA_HUFF_CODES[zeroRun][category] : AC_CHROMA_HUFF_CODES[zeroRun][category] ) + bitString;
            }
            
            std::cout << std::endl;
        }
    }
    
    std::cout << "Final Scan Data: " << m_scanData << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
    
Encoder::Status Encoder::byteStuffScanData()
{
    if ( m_scanData.empty() )
    {
        std::cout << "Invalid scan data!" << std::endl;
        return Status::FAILED;
    }
    
    for ( UInt16 i = 0; i <= m_scanData.size() - 8; i += 8 )
    {
        if ( m_scanData.substr( i, 8 ) == "11111111" )
        {
            if ( i + 8 < m_scanData.size() - 8 )
            {
                m_scanData.insert( i + 8, "00000000" );
            }
            else
                continue;
        }
    }
    
    std::cout << "Byte stuffed scan data: " << m_scanData << std::endl;
}

}
