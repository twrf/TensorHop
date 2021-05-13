#ifndef BMP_H
#define BMP_H

#include "common/Defs.h"
#include "memory/Tensor.h"

/*
> bmp文件头(bmp file header)：提供文件的格式、大小等信息 > 14 byte
> 位图信息头(bitmap information)：提供图像数据的尺寸、位平面数、压缩方式、颜色索引等信息 > 40byte
> 调色板(color palette)：可选，如使用索引来表示图像，调色板就是索引与其对应的颜色的映射表 > 由颜色索引数决定
> 位图数据(bitmap data)：就是图像数据啦 > 由图像尺寸决定
*/

// 数据按照小端存储
#pragma pack(1)
struct BitMapFileHeader
{
        uint16 type;            // BM - Windows 3.1x, 95, NT, ...
                                // BA - OS/2 Bitmap Array
                                // CI - OS/2 Color Icon
                                // CP - OS/2 Color Poinyer
                                // IC - OS/2 Icon
                                // PT - OS/2 Pointer */
        uint32 size;            // 说明该位图文件的大小，用字节为单位
        uint16 reserved1;       // 保留，必须设置为0
        uint16 reserved2;       // 保留，必须设置为0
        uint32 offBits;         // 说明从文件头开始到实际的图像数据之间的字节偏移量
};
#pragma pack()

#pragma pack(1)
struct BitMapFileInformation
{
        uint32 size;            // BitMapFileInformation结构所需的字节数
        uint32 width;           // 说明图像的宽度，用像素为单位
        int32 height;           // 说明图像的高度，用像素为单位
                                // 以行为主序排列
                                // 正数：倒向位图，左下角到右上角
                                // 负数：正向位图，左上角到右下角
        uint16 planes;          // 为目标设备说明颜色平面数，其值将总被设为1
        uint16 bitCount;        // 说明比特数/像素，其值为1、4、8、16、24或32
        uint32 compression;     // 说明图像数据压缩的类型：
                                // 0 - BI_RGB 不压缩（最常用）
                                // 1 - BI_RLE8 8比特游程编码（RLE），只用于8位位图
                                // 2 - BI_RLE4 4比特游程编码（RLE），只用于4位位图
                                // 3 - BI_BITFIELEDS 比特域，用于16/32位位图
                                // 4 - BI_JPEG 位图含JPEG图像（仅用于打印机）
                                // 5 - BI_PNG 位图含PNG图像（仅用于打印机）
        uint32 sizeImage;       // 说明图像的大小，以字节为单位。当使用BI_RGB格式时，可设置为0
        uint32 xPelsPerMeter;	// 说明水平分辨率，用像素/米表示，有符号整数
        uint32 yPelsPerMeter;	// 说明垂直分辨率，用像素/米表示，有符号整数
        uint32 clrUsed;         // 说明位图实际使用的彩色表中的颜色索引数，设为0表示使用所有调色板项
        uint32 clrImportant;	// 说明对图像显示有多重要影响的颜色索引的数码，0表示都重要
};
#pragma pack()

#pragma pack(1)
struct BitMapFileColorPlane
{
        uint8 blue;
        uint8 green;
        uint8 red;
        uint8 alpha;
};
#pragma pack()


class CBmp
{
public:
    CBmp() {}
    CBmp(std::string filename);
    ~CBmp();

//    uint8* getImage() {return NULL == m_tensor ? NULL : m_tensor->atBegin();}
    int getImageWidth() {return bmpInformation.width;}
    int getImageHeight() {return (bmpInformation.height > 0) ? bmpInformation.height : -1 * bmpInformation.height;}
    int getImageDepth() {return bmpInformation.bitCount/8;} // CV_8UC3
    CTensor<uint8>& getTensor() {return *m_tensor;}

    void show(std::string showName);

private:
    bool loadImageData(std::string filename);
    bool swapMemory(uint8* p1, uint8* p2, int size);

public:

private:
    BitMapFileHeader bmpHeader;
    BitMapFileInformation bmpInformation;
//    uint8 *image;
    CTensor<uint8>* m_tensor;
};

bool saveBMP(CTensor<uint8>& tensor, std::string filename);

#endif // BMP_H
