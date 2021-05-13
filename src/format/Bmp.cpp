#include "Bmp.h"
#include "utils/Tools.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>
//#include <memory/Tensor.cpp>

bool saveBMP(CTensor<uint8>& tensor, std::string filename)
{
    BitMapFileHeader head;
    head.type = ('B' & 0x00FF) + (('M' & 0x00FF) << 8);
//    head.size = ;
    head.reserved1 = 0;
    head.reserved2 = 0;
    head.offBits = sizeof(BitMapFileHeader) + sizeof(BitMapFileInformation);

    BitMapFileInformation info;
    info.size = sizeof(BitMapFileInformation);
    info.width = tensor.dim(dim_w);
    info.height = -1 * tensor.dim(dim_h);
    info.planes = 1;
    info.bitCount = (tensor.dim() == 3) ? (8 * tensor.dim(dim_c)) : 8;
    info.compression = 0;
//    info.sizeImage = std::abs(writeHeight * writeWidth);
    info.xPelsPerMeter = 0;
    info.yPelsPerMeter = 0;
    info.clrUsed = 0;
    info.clrImportant = 0;

    int writeHeight = std::abs(info.height);
    int lineWidth = info.width * info.bitCount / 8;
    int writeWidth = (info.width % 4 ==0) ? lineWidth : (lineWidth / 4 * 4 + 4);
    int widthFix = writeWidth - lineWidth;
    info.sizeImage = std::abs(writeHeight * writeWidth);
    head.size = head.offBits + info.sizeImage;
    head.offBits += (info.bitCount == 8 ? (sizeof(BitMapFileColorPlane) * 256) : 0);
//    infof("head.size:%d, writeHeight:%d, lineWidth:%d, writeWidth:%d, widthFix:%d\n",
//          head.size, writeHeight, lineWidth, writeWidth, widthFix);

    BitMapFileColorPlane colorPlane[256];
    for (int i=0; i<256; ++i)
    {
        colorPlane[i].blue = i;
        colorPlane[i].green = i;
        colorPlane[i].red = i;
        colorPlane[i].alpha = 0;
    }

    if (!SaveDataToFile(filename, sizeof(BitMapFileHeader), (uint8*)&head))
    {
        errorf("save BMP head filed\n");
        return false;
    }

    if (!SaveDataToFile(filename, sizeof(BitMapFileInformation), (uint8*)&info))
    {
        errorf("save BMP info filed\n");
        return false;
    }

    if (info.bitCount == 8)
    {
        if (!SaveDataToFile(filename, 256 * sizeof(BitMapFileColorPlane), (uint8*)&colorPlane))
        {
            errorf("save BMP color plane filed\n");
            return false;
        }
    }

    std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    if (out.fail())
    {
        errorf("Open BMP file:[%s] failed.\n", filename.c_str());
        out.close();
        return false;
    }

    uint8 fix[widthFix];
    for (int i=0; i<writeHeight; ++i)
    {
        out.write((char*)(tensor.begin() + lineWidth*i), lineWidth);
        if (0 != widthFix)
        {
            out.write((char*)fix, widthFix);
        }
    }
    out.close();

    return true;
}

CBmp::CBmp(std::string filename) :
    m_tensor(NULL)
{
    if (!GetDataFromFile(filename, 0, (int)sizeof(bmpHeader), (uint8*)&bmpHeader))
    {
        errorf("get bmp file %s head failed\n", filename.c_str());
    }

    if (!GetDataFromFile(filename, (int)sizeof(bmpHeader), (int)sizeof(bmpInformation), (uint8*)&bmpInformation))
    {
        errorf("get bmp file %s info failed\n", filename.c_str());
    }

#if 0
    infof("bmpHeader.type = %c%c\n", bmpHeader.type&0xFF, (bmpHeader.type>>8)&0xFF);
    infof("bmpHeader.size = %d\n", bmpHeader.size);
    infof("bmpHeader.offBits = %d\n", bmpHeader.offBits);

    infof("bmpInformation.size = %d\n", bmpInformation.size);
    infof("bmpInformation.width = %d\n", bmpInformation.width);
    infof("bmpInformation.height = %d\n", bmpInformation.height);
    infof("bmpInformation.planes = %d\n", bmpInformation.planes);
    infof("bmpInformation.bitCount = %d\n", bmpInformation.bitCount);
    infof("bmpInformation.compression = %d\n", bmpInformation.compression);
    infof("bmpInformation.sizeImage = %d\n", bmpInformation.sizeImage);
    infof("bmpInformation.xPelsPerMeter = %d\n", bmpInformation.xPelsPerMeter);
    infof("bmpInformation.yPelsPerMeter = %d\n", bmpInformation.yPelsPerMeter);
    infof("bmpInformation.clrUsed = %d\n", bmpInformation.clrUsed);
    infof("bmpInformation.clrImportant = %d\n", bmpInformation.clrImportant);
#endif

    loadImageData(filename);
}

CBmp::~CBmp()
{
    infof("going to free tenser\n");
    delete m_tensor;
}

void CBmp::show(std::string showName)
{
    m_tensor->show(showName);
}

bool CBmp::loadImageData(std::string filename)
{
//    int imageDataSize = bmpInformation.height * bmpInformation.width * bmpInformation.bitCount/8;

    m_tensor = new CTensor<uint8>({(int)bmpInformation.width,
                                   (int)bmpInformation.height,
                                   (int)bmpInformation.bitCount/8});

#if 0
    if (!GetDataFromFile(filename, bmpHeader.offBits, imageDataSize, m_tensor->atBegin()))
    {
        errorf("get bmp file %s data failed\n", filename.c_str());
        return false;
    }
#else // 位图需要处理四字节对齐问题，应当舍弃补齐的字节
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!in.is_open())
    {
        errorf("Open file:[%s] failed.\n", filename.c_str());
        in.close();
        return false;
    }

    in.seekg(bmpHeader.offBits, std::ios::beg);
    int readHight = bmpInformation.height;
    int lineWidth = bmpInformation.width * bmpInformation.bitCount / 8;
    int readWidth = (lineWidth % 4 == 0) ? lineWidth : (lineWidth / 4 * 4 + 4);
    int widthFix = readWidth - lineWidth;
    infof("readHight=%d, lineWidth=%d, readWidth=%d, widthFix=%d\n", readHight, lineWidth, readWidth, widthFix);

    uint8 fix[widthFix];
    for (int i=0; i<readHight; ++i)
    {
        in.read((char*)(m_tensor->data() + lineWidth*i), lineWidth);
        if (0 != widthFix)
        {
            in.read((char*)(fix), widthFix);
        }
    }

    in.close();
#endif

    // 大于0，说明bmp是倒置的
    if (bmpInformation.height > 0)
    {
        for (int i=0; i<bmpInformation.height/2; ++i)
        {
            swapMemory(m_tensor->begin() + bmpInformation.width*3*i,
                       m_tensor->begin() + bmpInformation.width*3*(bmpInformation.height-1-i),
                       bmpInformation.width*3);
        }
    }

    return true;
}

bool CBmp::swapMemory(uint8 *p1, uint8 *p2, int size)
{
    uint8 tmp[size];
    memcpy(tmp, p1, size);
    memcpy(p1, p2, size);
    memcpy(p2, tmp, size);

    return true;
}


