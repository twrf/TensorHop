#include "Tools.h"
#include <string.h>
#include <iostream>
#include <ios>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

bool GetDataFromFile(std::string file_path, int offset, int size, uint8 *pbuffer)
{
    std::ifstream in(file_path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!in.is_open())
    {
        errorf("Open file:[%s] failed.\n", file_path.c_str());
        in.close();
        return false;
    }
//    infof("Open file:[%s] succeed.\n", file_path.c_str());

    long file_length = in.tellg();
//    tracef("file_length = %ld\n", file_length);
    if (size + offset > file_length)
    {
        errorf("Read lenght:%d+%d=%d is larger than file_length=%ld\n", offset, size, offset+size, file_length);
        in.close();
        return false;
    }

    in.seekg(offset, std::ios::beg);
    in.read((char*)pbuffer, size);
    in.close();
    return true;
}

bool GetDataFromFile(std::ifstream in, int offset, int size, uint8 *pbuffer)
{
    long file_length = in.tellg();
    if (size + offset > file_length)
    {
        errorf("Read lenght:%d+%d=%d is larger than file_length=%ld\n", offset, size, offset+size, file_length);
        return false;
    }

    in.seekg(offset, std::ios::beg);
    in.read((char*)pbuffer, size);
    return true;
}

bool SaveDataToFile(std::string filename, int size, uint8* pbuffer)
{
    std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    if (out.fail())
    {
        errorf("Open file:[%s] failed.\n", filename.c_str());
        out.close();
        return false;
    }

    out.write((char*)pbuffer, size);
    out.close();
    return true;
}

bool GetDataFromMemory(uint8 *src, int src_length, int offset, int size, uint8 *pbuffer)
{
    if (offset + size > src_length)
    {
        errorf("Read lenght:%d+%d=%d is larger than src_length=%d\n", offset, size, offset+size, src_length);
        return false;
    }

    memcpy((char*)pbuffer, src + offset, size);
    return true;
}

bool ChangeMsbLsb(uint8 *src, int lengthOfByte)
{
    if (NULL == src)
    {
        errorf("src if null\n");
        return false;
    }

    if (2 == lengthOfByte)
    {
        MSBLSB_16b(*(unsigned short *)src);
    }
    else if (4 == lengthOfByte)
    {
        MSBLSB_32b(*(unsigned int *)src);
    }
    else
    {
        warnf("not supprt yet\n");
        ASSERT(0);
    }

    return true;
}

std::vector<int> convJsonArray2Vector(const CConfigTable &array)
{
    std::vector<int> ret;
    if (array.isArray() && array.size() > 0)
    {
        for (uint32 index=0; index<array.size(); ++index)
        {
            ret.push_back(array[index].asInt());
        }
    }
    return ret;
}
