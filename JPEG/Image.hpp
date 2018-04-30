#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "Utility.hpp"

namespace jpeg
{

class Image
{

public:

    Image();
    
    ~Image();
    
    void resize( const UInt16 width, const UInt16 height );
    
    void setPixels( const PixelArray& pixels );
    
    Pixel& operator() ( const UInt16 row, const UInt16 column );
    
    PixelArray& getPixelPtr();
    
    inline const UInt16 width() const
    {
        return m_width;
    }

    inline const UInt16 height() const
    {
        return m_height;
    }
    
    IMatrix8x8 getMCU( const UInt16 mcu, const int channel );

private:

    PixelArray m_pixels;
    UInt16 m_width;
    UInt16 m_height;

};

}

#endif // IMAGE_HPP
