#ifndef TOOLS_H
#define TOOLS_H

#include "common/Defs.h"

#define MSB     0
#define LSB     1

#define MSBLSB_32b(tmp)  ( ((tmp & 0x000000FF) << 24) \
                         | ((tmp & 0x0000FF00) << 8) \
                         | ((tmp & 0x00FF0000) >> 8) \
                         | ((tmp & 0xFF000000) >> 24))

#define MSBLSB_16b(tmp)  ( ((tmp & 0x00FF) << 8) \
                         | ((tmp & 0xFF00) >> 8))

bool GetDataFromFile(std::string file_path, int offset, int size, uint8 *pbuffer);
bool GetDataFromFile(std::ifstream in, int offset, int size, uint8 *pbuffer);

bool SaveDataToFile(std::string filename, int size, uint8* pbuffer);

bool GetDataFromMemory(uint8* src, int src_length, int offset, int size, uint8 *pbuffer);

enum MsbLsbChangeType
{
    MsbLsbChangeType_16 = 0,
    MsbLsbChangeType_32,
    MsbLsbChangeType_Given
};
bool ChangeMsbLsb(uint8* src, int lengthOfByte);

std::vector<int> convJsonArray2Vector(const CConfigTable& array);

#endif // TOOLS_H
