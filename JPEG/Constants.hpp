#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <vector>

#include "Utility.hpp"

namespace jpeg
{

// Luminance Quantization matrix, Quality: 50%
const std::vector<std::vector<UInt8>> M_QT_MAT_LUMA
{
    {
        { 16, 11, 10, 16,  24,  40,  51, 61  },
        { 12, 12, 14, 19,  26,  58,  60, 55  },
        { 14, 13, 16, 24,  40,  57,  69, 56  },
        { 14, 17, 22, 29,  51,  87,  80, 62  },
        { 18, 22, 37, 56,  68, 109, 103, 77  },
        { 24, 35, 55, 64,  81, 104, 113, 92  },    
        { 49, 64, 78, 87, 103, 121, 120, 101 },
        { 72, 92, 95, 98, 112, 100, 103, 99  }    
    }
};

// Chrominance Quantization matrix, Quality: 50%
const std::vector<std::vector<UInt8>> M_QT_MAT_CHROMA
{
    {
        { 17, 18, 24, 47, 99, 99, 99, 99 },
        { 18, 21, 26, 66, 99, 99, 99, 99 },
        { 24, 26, 56, 99, 99, 99, 99, 99 },
        { 47, 66, 99, 99, 99, 99, 99, 99 },
        { 99, 99, 99, 99, 99, 99, 99, 99 },
        { 99, 99, 99, 99, 99, 99, 99, 99 },
        { 99, 99, 99, 99, 99, 99, 99, 99 },
        { 99, 99, 99, 99, 99, 99, 99, 99 },
    }
};

// Standard Huffman Tables
// See Annex-K of the JPEG standard

// Table: K-3: Luminance DC coefficient differences
const std::vector<std::string> DC_LUMA_HUFF_CODES = 
{
    "00"        , // 0 (EOB)
    "010"       , // 1
    "011"       , // 2
    "100"       , // 3
    "101"       , // 4
    "110"       , // 5
    "1110"      , // 6
    "11110"     , // 7
    "111110"    , // 8
    "1111110"   , // 9
    "11111110"  , // 10
    "111111110"   // 11
};

// Table: K-4: chrominance DC coefficient differences
const std::vector<std::string> DC_CHROMA_HUFF_CODES = 
{
    "00"          , // 0 (EOB)
    "01"          , // 1
    "10"          , // 2
    "110"         , // 3
    "1110"        , // 4
    "11110"       , // 5
    "111110"      , // 6
    "1111110"     , // 7
    "11111110"    , // 8
    "111111110"   , // 9
    "1111111110"  , // 10
    "11111111110"   // 11
};

// Table: K-5: Luminance AC coefficients
const std::vector<std::vector<std::string>> AC_LUMA_HUFF_CODES = 
{
    {
        "1010"             , // 0/0 (EOB)
        "00"               , // 0/1
        "01"               , // 0/2
        "100"              , // 0/3
        "1011"             , // 0/4
        "11010"            , // 0/5
        "1111000"          , // 0/6
        "11111000"         , // 0/7
        "1111110110"       , // 0/8
        "1111111110000010" , // 0/9
        "1111111110000011"   // 0/A            
    },
    
    {
        "NULL"             , // 1/0
        "1100"             , // 1/1
        "11011"            , // 1/2
        "1111001"          , // 1/3
        "111110110"        , // 1/4
        "11111110110"      , // 1/5
        "1111111110000100" , // 1/6
        "1111111110000101" , // 1/7
        "1111111110000110" , // 1/8
        "1111111110000111" , // 1/9
        "1111111110001000"   // 1/A
    },
    
    {
        "NULL"              , // 2/0
        "11100"             , // 2/1
        "11111001"          , // 2/2
        "1111110111"        , // 2/3
        "111111110100"      , // 2/4
        "1111111110001001"  , // 2/5
        "1111111110001010"  , // 2/6
        "1111111110001011"  , // 2/7
        "1111111110001100"  , // 2/8
        "1111111110001101"  , // 2/9
        "1111111110001110"    // 2/A
    },
    
    {
        "NULL"              , // 3/0
        "111010"            , // 3/1
        "111110111"         , // 3/2
        "111111110101"      , // 3/3
        "11111111110001111" , // 3/4
        "11111111110010000" , // 3/5
        "11111111110010001" , // 3/6
        "11111111110010010" , // 3/7
        "11111111110010011" , // 3/8
        "11111111110010100" , // 3/9
        "11111111110010101"   // 3/A
    },
    
    {
        "NULL"              , // 4/0
        "111011"            , // 4/1
        "1111111000"        , // 4/2
        "1111111110010110"  , // 4/3
        "1111111110010111"  , // 4/4
        "1111111110011000"  , // 4/5
        "1111111110011001"  , // 4/6
        "1111111110011010"  , // 4/7
        "1111111110011011"  , // 4/8
        "1111111110011100"  , // 4/9
        "1111111110011101"    // 4/A
    },
    
    {
        "NULL"              , // 5/0
        "1111010"           , // 5/1
        "11111110111"       , // 5/2
        "1111111110011110"  , // 5/3
        "1111111110011111"  , // 5/4
        "1111111110100000"  , // 5/5
        "1111111110100001"  , // 5/6
        "1111111110100010"  , // 5/7
        "1111111110100011"  , // 5/8
        "1111111110100100"  , // 5/9
        "1111111110100101"   // 5/A
    },
    
    {
        "NULL"              , // 6/0
        "1111011"           , // 6/1
        "111111110110"      , // 6/2
        "1111111110100110"  , // 6/3
        "1111111110100111"  , // 6/4
        "1111111110101000"  , // 6/5
        "1111111110101001"  , // 6/6
        "1111111110101010"  , // 6/7
        "1111111110101011"  , // 6/8
        "1111111110101100"  , // 6/9
        "1111111110101101"    // 6/A
    },
    
    {
        "NULL"              , // 7/0
        "11111010"          , // 7/1
        "111111110111"      , // 7/2
        "1111111110101110"  , // 7/3
        "1111111110101111"  , // 7/4
        "1111111110110000"  , // 7/5
        "1111111110110001"  , // 7/6
        "1111111110110010"  , // 7/7
        "1111111110110011"  , // 7/8
        "1111111110110100"  , // 7/9
        "1111111110110101"  // 7/A
    },
    
    {
        "NULL"              , // 8/0
        "111111000"         , // 8/1
        "111111111000000"   , // 8/2
        "1111111110110110"  , // 8/3
        "1111111110110111"  , // 8/4
        "1111111110111000"  , // 8/5
        "1111111110111001"  , // 8/6
        "1111111110111010"  , // 8/7
        "1111111110111011"  , // 8/8
        "1111111110111100"  , // 8/9
        "1111111110111101"    // 8/A
    },
    {"NULL", // 9/0
    "111111001", // 9/1
    "1111111110111110", // 9/2
    "1111111110111111", // 9/3
    "1111111111000000", // 9/4
    "1111111111000001", // 9/5
    "1111111111000010", // 9/6
    "1111111111000011", // 9/7
    "1111111111000100", // 9/8
    "1111111111000101", // 9/9
    "1111111111000110"}, // 9/A
    {"NULL", // A/0
    "111111010",  // A/1
    "1111111111000111", // A/2
    "1111111111001000", // A/3
    "1111111111001001", // A/4
    "1111111111001010", // A/5
    "1111111111001011", // A/6
    "1111111111001100", // A/7
    "1111111111001101", // A/8
    "1111111111001110", // A/9
    "1111111111001111"}, // A/A
    {"NULL", // B/0
    "1111111001", // B/1
    "1111111111010000", // B/2
    "1111111111010001", // B/3
    "1111111111010010", // B/4
    "1111111111010011", // B/5
    "1111111111010100", // B/6
    "1111111111010101", // B/7
    "1111111111010110", // B/8
    "1111111111010111", // B/9
    "1111111111011000"}, // B/A
    {"NULL", // C/0
    "1111111010", // C/1
    "1111111111011001", // C/2
    "1111111111011010", // C/3
    "1111111111011011", // C/4
    "1111111111011100", // C/5
    "1111111111011101", // C/6
    "1111111111011110", // C/7
    "1111111111011111", // C/8
    "1111111111100000", // C/9
    "1111111111100001"}, // C/A
    {"NULL", // D/0
    "11111111000", // D/1
    "1111111111100010", // D/2
    "1111111111100011", // D/3
    "1111111111100100", // D/4
    "1111111111100101", // D/5
    "1111111111100110", // D/6
    "1111111111100111", // D/7
    "1111111111101000", // D/8
    "1111111111101001", // D/9
    "1111111111101010"}, // D/A
    {"NULL", // E/0
    "1111111111101011", // E/1
    "1111111111101100", // E/2
    "1111111111101101", // E/3
    "1111111111101110", // E/4
    "1111111111101111", // E/5
    "1111111111110000", // E/6
    "1111111111110001", // E/7
    "1111111111110010", // E/8
    "1111111111110011", // E/9
    "1111111111110100"}, // E/A
    {"11111111001", // F/0 (Zero-run length)
    "1111111111110101", // F/1
    "1111111111110110", // F/2
    "1111111111110111", // F/3
    "1111111111111000", // F/4
    "1111111111111001", // F/5
    "1111111111111010", // F/6
    "1111111111111011", // F/7
    "1111111111111100", // F/8
    "1111111111111101", // F/9
    "1111111111111110"} // F/A
};

// Table: K-5: Chrominance AC coefficients
const std::vector<std::vector<std::string>> AC_CHROMA_HUFF_CODES = 
{
    {"00", // 0/0 (EOB)
    "01", // 0/1
    "100", // 0/2
    "1010", // 0/3
    "11000", // 0/4
    "11001", // 0/5
    "111000", // 0/6
    "1111000", // 0/7
    "111110100", // 0/8
    "1111110110", // 0/9
    "111111110100"},  // 0/A
    {"NULL", // 1/0
    "1011", // 1/1
    "111001", // 1/2
    "11110110", // 1/3
    "111110101", // 1/4
    "11111110110", // 1/5
    "111111110101", // 1/6
    "1111111110001000", // 1/7
    "1111111110001001", // 1/8
    "1111111110001010", // 1/9
    "1111111110001011"}, // 1/A
    {"NULL",  // 2/0
    "11010", // 2/1
    "11110111", // 2/2
    "1111110111", // 2/3
    "111111110110", // 2/4
    "111111111000010", // 2/5
    "1111111110001100", // 2/6
    "1111111110001101", // 2/7
    "1111111110001110", // 2/8
    "1111111110001111", // 2/9
    "1111111110010000"}, // 2/A
    {"NULL", // 3/0
    "11011", // 3/1
    "11111000", // 3/2
    "1111111000", // 3/3
    "111111110111", // 3/4
    "1111111110010001", // 3/5
    "1111111110010010", // 3/6
    "1111111110010011", // 3/7
    "1111111110010100", // 3/8
    "1111111110010101", // 3/9
    "1111111110010110"}, // 3/A
    {"NULL", // 4/0
    "111010", // 4/1
    "111110110", // 4/2
    "1111111110010111", // 4/3
    "1111111110011000", // 4/4
    "1111111110011001", // 4/5
    "1111111110011010", // 4/6
    "1111111110011011", // 4/7
    "1111111110011100", // 4/8
    "1111111110011101", // 4/9
    "1111111110011110"}, // 4/A
    {"NULL", // 5/0
    "111011", // 5/1
    "1111111001", // 5/2
    "1111111110011111", // 5/3
    "1111111110100000", // 5/4
    "1111111110100001", // 5/5
    "1111111110100010", // 5/6
    "1111111110100011", // 5/7
    "1111111110100100", // 5/8
    "1111111110100101", // 5/9
    "1111111110100110"}, // 5/A
    {"NULL", // 6/0
    "1111001", // 6/1
    "11111110111", // 6/2
    "1111111110100111", // 6/3
    "1111111110101000", // 6/4
    "1111111110101001", // 6/5
    "1111111110101010", // 6/6
    "1111111110101011", // 6/7
    "1111111110101100", // 6/8
    "1111111110101101", // 6/9
    "1111111110101110"}, // 6/A
    {"NULL", // 7/0
    "1111010", // 7/1
    "11111111000", // 7/2
    "1111111110101111", // 7/3
    "1111111110110000", // 7/4
    "1111111110110001", // 7/5
    "1111111110110010", // 7/6
    "1111111110110011", // 7/7
    "1111111110110100", // 7/8
    "1111111110110101", // 7/9
    "1111111110110110"}, // 7/A
    {"NULL", // 8/0
    "11111001", // 8/1
    "1111111110110111", // 8/2
    "1111111110111000", // 8/3
    "1111111110111001", // 8/4
    "1111111110111010", // 8/5
    "1111111110111011", // 8/6
    "1111111110111100", // 8/7
    "1111111110111101", // 8/8
    "1111111110111110", // 8/9
    "1111111110111111"}, // 8/A
    {"NULL", // 9/0
    "111110111", // 9/1
    "1111111111000000", // 9/2
    "1111111111000001", // 9/3
    "1111111111000010", // 9/4
    "1111111111000011", // 9/5
    "1111111111000100", // 9/6
    "1111111111000101", // 9/7
    "1111111111000110", // 9/8
    "1111111111000111", // 9/9
    "1111111111001000"}, // 9/A
    {"NULL", // A/0
    "111111000", // A/1
    "1111111111001001", // A/2
    "1111111111001010", // A/3
    "1111111111001011", // A/4
    "1111111111001100", // A/5
    "1111111111001101", // A/6
    "1111111111001110", // A/7
    "1111111111001111", // A/8
    "1111111111010000", // A/9
    "1111111111010001"}, // A/A
    {"NULL", // B/0
    "111111001", // B/1
    "1111111111010010", // B/2
    "1111111111010011", // B/3
    "1111111111010100", // B/4
    "1111111111010101", // B/5
    "1111111111010110", // B/6
    "1111111111010111", // B/7
    "1111111111011000", // B/8
    "1111111111011001", // B/9
    "1111111111011010"}, // B/A
    {"NULL", // C/0
    "111111010", // C/1
    "1111111111011011", // C/2
    "1111111111011100", // C/3
    "1111111111011101", // C/4
    "1111111111011110", // C/5
    "1111111111011111", // C/6
    "1111111111100000", // C/7
    "1111111111100001", // C/8
    "1111111111100010", // C/9
    "1111111111100011"}, // C/A
    {"NULL", // D/0
    "11111111001", // D/1
    "1111111111100100", // D/2
    "1111111111100101", // D/3
    "1111111111100110", // D/4
    "1111111111100111", // D/5
    "1111111111101000", // D/6
    "1111111111101001", // D/7
    "1111111111101010", // D/8
    "1111111111101011", // D/9
    "1111111111101100"}, // D/A
    {"NULL", // E/0
    "11111111100000", // E/1
    "1111111111101101", // E/2
    "1111111111101110", // E/3
    "1111111111101111", // E/4
    "1111111111110000", // E/5
    "1111111111110001", // E/6
    "1111111111110010", // E/7
    "1111111111110011", // E/8
    "1111111111110100", // E/9
    "1111111111110101"}, // E/A
    {"1111111010", // F/0 (Zero-run length)
    "111111111000011", // F/1
    "1111111111110110", // F/2
    "1111111111110111", // F/3
    "1111111111111000", // F/4
    "1111111111111001", // F/5
    "1111111111111010", // F/6
    "1111111111111011", // F/7
    "1111111111111100", // F/8
    "1111111111111101", // F/9
    "1111111111111110"} // F/A
};

}

#endif // CONSTANTS_HPP
