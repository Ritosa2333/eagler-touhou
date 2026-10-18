#include "sj2utf8.h"

#include <string>

#include "convtable.h"
#include "inttypes.hpp"

// stolen from https://stackoverflow.com/questions/33165171/c-shiftjis-to-utf8-conversion/33170901
std::string sj2utf8(const std::string &input)
{
    std::string output(3 * input.length(), ' '); //ShiftJis won't give 4byte UTF8, so max. 3 byte per input char are needed
    size_t indexInput = 0, indexOutput = 0;

    while(indexInput < input.length())
    {
        char arraySection = ((u8)input[indexInput]) >> 4;

        size_t arrayOffset;
        if(arraySection == 0x8) arrayOffset = 0x100; //these are two-byte shiftjis
        else if(arraySection == 0x9) arrayOffset = 0x1100;
        else if(arraySection == 0xE) arrayOffset = 0x2100;
        else arrayOffset = 0; //this is one byte shiftjis

        //determining real array offset
        if(arrayOffset)
        {
            arrayOffset += (((u8)input[indexInput]) & 0xf) << 8;
            indexInput++;
            if(indexInput >= input.length()) break;
        }
        arrayOffset += (u8)input[indexInput++];
        arrayOffset <<= 1;

        //unicode number is...
        uint16_t unicodeValue = (shiftJIS_convTable[arrayOffset] << 8) | shiftJIS_convTable[arrayOffset + 1];

        //converting to UTF8
        if(unicodeValue < 0x80)
        {
            output[indexOutput++] = unicodeValue;
        }
        else if(unicodeValue < 0x800)
        {
            output[indexOutput++] = 0xC0 | (unicodeValue >> 6);
            output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
        }
        else
        {
            output[indexOutput++] = 0xE0 | (unicodeValue >> 12);
            output[indexOutput++] = 0x80 | ((unicodeValue & 0xfff) >> 6);
            output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
        }
    }

    output.resize(indexOutput); //remove the unnecessary bytes
    return output;
}
