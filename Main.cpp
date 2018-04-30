#include <iostream>
#include "JPEG/Utility.hpp"
#include "Encoder/Encoder.hpp"

int main()
{
    jpeg::Pixel p( 255, 156, 89 );
    jpeg::rgbToYCbCr( p );
    
    //std::cout << p[0] << "," << p[1] << ", " << p[2] << std::endl;
    
    int width = 317;
    int height = 457;
    
    width = ( width / 8 ) * 8 + 8;
    height = ( height / 8 ) * 8 + 8;
    
    //std::cout << width << "X" << height << std::endl;
    
    /*jpeg::IMatrix8x8 mat =
    {
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76,
        76, 76, 76, 76, 76, 76, 76, 76
    };
    
    for (int i=0;i<8;++i)
        for (int j=0;j<8;++j)
            mat[i][j] -= 128;
    
    auto coeff = jpeg::forwardDCT( mat );
    
    for ( auto&& row: coeff )
    {
        for (auto&& v : row)
            std::cout << v << " ";
        std::cout << std::endl;
    }*/
    
    jpeg::Encoder encoder;
    encoder.loadPPMFile( "foo2.ppm" );
    encoder.encodePPMFile();
    encoder.saveToJFIFFile( "foo2.jpg" );

    return 0;
}
