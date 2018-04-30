#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <array>
#include <utility>

namespace jpeg
{

using UInt8  = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;

using Int8   = std::int8_t;
using Int16  = std::int16_t;
using Int32  = std::int32_t;
using Int64  = std::int64_t;

////////////////////////////////////////////////////////////////////////////////

const int R = 0;
const int G = 1;
const int B = 2;

const int Y = 0;
const int Cb = 1;
const int Cr = 2;

struct Pixel
{
    
    Pixel()
    {
        comp[0] = comp[1] = comp[2] = 0;
    }
    
    Pixel( Int16 c1, Int16 c2, Int16 c3 )
    {
        comp[0] = c1;
        comp[1] = c2;
        comp[2] = c3;
    }
    
    Int16& operator[] ( std::size_t c )
    {
        return comp[c];
    }
    
    Pixel& operator= ( const Pixel& p )
    {
        comp[0] = p.comp[0];
        comp[1] = p.comp[1];
        comp[2] = p.comp[2];
    }
    
    Int16 comp[3];
    
};

using PixelArray = std::vector<std::vector<Pixel>>;
using PixelArrayPtr = PixelArray*;

////////////////////////////////////////////////////////////////////////////////

using HuffmanTable = std::array<std::pair<Int16, std::vector<UInt8>>, 16 >;

const int HT_DC = 0;
const int HT_AC = 1;
const int HT_Y = 0;
const int HT_CbCr = 1;

////////////////////////////////////////////////////////////////////////////////

using Block8x8 = std::array<std::array<Pixel, 8>, 8>;
using FlMatrix8x8 = std::array<std::array<float, 8>, 8>;
using IMatrix8x8 = std::array<std::array<Int16, 8>, 8>;

////////////////////////////////////////////////////////////////////////////////

FlMatrix8x8 forwardDCT( const IMatrix8x8& block );

////////////////////////////////////////////////////////////////////////////////

void rgbToYCbCr( Pixel& rgbPixel );

////////////////////////////////////////////////////////////////////////////////

inline const int coordsToZigzag( const int row, const int column )
{
    static const int matOrder[8][8] = 
    {
        {  0,  1,  5,  6, 14, 15, 27, 28 },
        {  2,  4,  7, 13, 16, 26, 29, 42 },
        {  3,  8, 12, 17, 25, 30, 41, 43 },
        {  9, 11, 18, 24, 31, 40, 44, 53 },
        { 10, 19, 23, 32, 39, 45, 52, 54 },
        { 20, 22, 33, 38, 46, 51, 55, 60 },
        { 21, 34, 37, 47, 50, 56, 59, 61 },
        { 35, 36, 48, 49, 57, 58, 62, 63 }
    };
    
    return matOrder[row][column];
}

////////////////////////////////////////////////////////////////////////////////

inline const std::pair<const int, const int> zigzagToCoords( const int index )
{
    static const std::pair<const int, const int> zzorder[64] =
    {
        {0,0},
        {0,1}, {1,0},         
        {2,0}, {1,1}, {0,2},
        {0,3}, {1,2}, {2,1}, {3,0},
        {4,0}, {3,1}, {2,2}, {1,3}, {0,4},
        {0,5}, {1,4}, {2,3}, {3,2}, {4,1}, {5,0},
        {6,0}, {5,1}, {4,2}, {3,3}, {2,4}, {1,5}, {0,6},
        {0,7}, {1,6}, {2,5}, {3,4}, {4,3}, {5,2}, {6,1}, {7,0},
        {7,1}, {6,2}, {5,3}, {4,4}, {3,5}, {2,6}, {1,7},
        {2,7}, {3,6}, {4,5}, {5,4}, {6,3}, {7,2},
        {7,3}, {6,4}, {5,5}, {4,6}, {3,7},
        {4,7}, {5,6}, {6,5}, {7,4},
        {7,5}, {6,6}, {5,7},
        {6,7}, {7,6},
        {7,7}
    };

    return zzorder[index];
}

////////////////////////////////////////////////////////////////////////////////

const Int16 getValueCategory( const Int16 value );

const std::string valueToBitString( const Int16 value );

}

#endif // UTILITY_HPP
