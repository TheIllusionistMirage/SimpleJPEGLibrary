#include <cmath>

#include "Utility.hpp"

namespace jpeg
{

FlMatrix8x8 forwardDCT( const IMatrix8x8& block )
{
    FlMatrix8x8 coeffs;
    
    for ( unsigned v = 0; v < 8; ++v )
    {
        for ( unsigned u = 0; u < 8; ++u )
        {
            double Cu = u == 0 ? 1.0 / std::sqrt(2) : 1 ;
            double Cv = v == 0 ? 1.0 / std::sqrt(2) : 1 ;
            double coeffY = (0.25) * Cu * Cv;
            
            double s1 = 0.0;
            
            for ( unsigned y = 0; y < 8; ++y )
            {
                double s2 = 0.0;
                
                for ( unsigned x = 0; x < 8; ++x )
                {
                    s2 += block[y][x] *
                           std::cos( ( ( 2 * x + 1 ) * u * M_PI ) / 16.0 ) *
                            std::cos( ( ( 2 * y + 1 ) * v * M_PI ) / 16.0 );
                }
                
                s1 += s2;
            }
            
            coeffY *= s1;
            
            coeffs[v][u] = std::roundf( coeffY * 100 ) / 100;
        }
    }
    
    return coeffs;
}

////////////////////////////////////////////////////////////////////////////////

void rgbToYCbCr( Pixel& pixel )
{
    Int16 r = pixel.comp[R];
    Int16 g = pixel.comp[G];
    Int16 b = pixel.comp[B];
    
    pixel.comp[Y] = 0.299 * r + 0.587 * g + 0.114 * b;
    pixel.comp[Cb] = -0.1687 * r - 0.3313 * g + 0.5 * b + 128;
    pixel.comp[Cr] = 0.5 * r - 0.4187 * g - 0.0813 * b + 128;
}

////////////////////////////////////////////////////////////////////////////////

const Int16 getValueCategory( const Int16 value )
{
    if ( value == 0x0000 )
        return 0;
    return std::log2( std::abs( value ) ) + 1;
}

////////////////////////////////////////////////////////////////////////////////

const std::string valueToBitString( const Int16 value )
{
    if ( value == 0x0000 )
        //return "0"; 
        return "";
    
    Int16 val = value;
    int bits = ( std::log2( std::abs( value ) ) ) + 1;
    //std::cout << "Bits: " << bits << ", ";
    std::string bitStr( bits, '0' );
    
    if ( val < 0 )
    {
        UInt16 delta = 0xFFFF >> (16 - bits);
        val += delta;
        val = std::abs( val );
    }
    
    int i = bitStr.size() - 1;
    while ( val > 0 )
    {
        bitStr[i--] = '0' + val % 2;
        val /= 2;
    }
    
    return bitStr;
}

}
