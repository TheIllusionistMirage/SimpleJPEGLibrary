#include <iostream>

#include "Image.hpp"

namespace jpeg
{

Image::Image() :
 m_pixels{},
 m_width(0),
 m_height(0)
{
}

////////////////////////////////////////////////////////////////////////////////
    
Image::~Image()
{
}

////////////////////////////////////////////////////////////////////////////////

void Image::resize( const UInt16 width, const UInt16 height )
{
    m_pixels.resize( height, std::vector<Pixel>( width, Pixel( 0, 0, 0 ) ) );
    m_width = width;
    m_height = height;
}

////////////////////////////////////////////////////////////////////////////////

void Image::setPixels( const PixelArray& pixels )
{
    for ( int y = 0; y < m_height; ++y )
    {
        for ( int x = 0; x < m_width; ++x )
        {
            m_pixels[y][x] = pixels[y][x];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Pixel& Image::operator() ( const UInt16 row, const UInt16 column )
{
    return m_pixels[row][column];
}

////////////////////////////////////////////////////////////////////////////////

PixelArray& Image::getPixelPtr()
{
    return m_pixels;
}

////////////////////////////////////////////////////////////////////////////////

IMatrix8x8 Image::getMCU( const UInt16 mcu, const int channel )
{
    //int y = i / ( m_width / 8 );
    //int x = i / ( ( m_height + 8 ) / 8 );
    
    UInt16 y = ( mcu / ( m_width / 8 ) ) * 8;
    UInt16 x = ( mcu % ( m_width / 8 ) ) * 8;
    
    //std::cout << y * 8 << ", " << x * 8 << std::endl;
    
    IMatrix8x8 MCU;
    
    for ( UInt16 r = y; r < y + 8; ++r ) {
        for ( UInt16 c = x; c < x + 8; ++c )
        {
        //std::cout << "Y: " << std::abs( y - r ) << ", X: " << std::abs(x - c) << std::endl;
        //std::cout << "" << r << "," << c << " ";// << std::endl;
        MCU[std::abs( y - r )][ std::abs(x - c)] = m_pixels[r][c][channel];
        }
        //std::cout << std::endl; 
    }
    
    return MCU;
}

}
