#include "morph.h"


bool dilate(const CTensor<uint8> &src, CTensor<uint8> &dst, const CTensor<uint8> &kernel)
{
    ASSERT(2 == src.dim() && 2 == dst.dim());

    int srcWidth = src.dim(dim_w);
    int srcHeight = src.dim(dim_h);
    int kernelWidth = kernel.dim(dim_w);
    int kernelHeight = kernel.dim(dim_h);

    ASSERT(srcWidth >= kernelWidth && srcHeight >= kernelHeight);
    ASSERT(kernelWidth % 2 == 1 && kernelHeight % 2 == 1);

    int kwr = kernelWidth / 2;
    int khr = kernelHeight / 2;
    for (int h=0; h<srcHeight; ++h)
    {
        for (int w=0; w<srcWidth; ++w)
        {
            int offset = h*srcWidth + w;
            uint8& curDst = dst.setAt(offset);
            bool isMatchOne = false;
            for (int kh=-1*khr; kh<=khr; ++kh)
            {
                if (kh+h<0 || kh+h>=srcHeight)
                {
                    continue;
                }

                for (int kw=-1*kwr; kw<=kwr; ++kw)
                {
                    if (kw+w<0 || kw+w>=srcWidth)
                    {
                        continue;
                    }

                    const uint8& curSrc = src.at(offset, kh, kw);
                    const uint8& curKernel = kernel.at(kernelWidth*(kh+khr) + kw+kwr);
                    if (curSrc && curKernel)
                    {
                        isMatchOne = true;
                        break;
                    }
                }

                if (isMatchOne)
                {
                    break;
                }
            }

            curDst = isMatchOne ? 255 : 0;
        }
    }

    return true;
}

bool erode(const CTensor<uint8> &src, CTensor<uint8> &dst, const CTensor<uint8> &kernel)
{
    ASSERT(2 == src.dim() && 2 == dst.dim());

    int srcWidth = src.dim(dim_w);
    int srcHeight = src.dim(dim_h);
    int kernelWidth = kernel.dim(dim_w);
    int kernelHeight = kernel.dim(dim_h);

    ASSERT(srcWidth >= kernelWidth && srcHeight >= kernelHeight);
    ASSERT(kernelWidth % 2 == 1 && kernelHeight % 2 == 1);

    int kwr = kernelWidth / 2;
    int khr = kernelHeight / 2;
    for (int h=0; h<srcHeight; ++h)
    {
        for (int w=0; w<srcWidth; ++w)
        {
            int offset = h*srcWidth + w;
            uint8& curDst = dst.setAt(offset);
            bool isMatchOne = true;
            for (int kh=-1*khr; kh<=khr; ++kh)
            {
                if (kh+h<0 || kh+h>=srcHeight)
                {
                    continue;
                }

                for (int kw=-1*kwr; kw<=kwr; ++kw)
                {
                    if (kw+w<0 || kw+w>=srcWidth)
                    {
                        continue;
                    }

                    const uint8& curSrc = src.at(offset, kh, kw);
                    const uint8& curKernel = kernel.at(kernelWidth*(kh+khr) + kw+kwr);
                    if (curSrc == 0 || curKernel == 0)
                    {
                        isMatchOne = false;
                        break;
                    }
                }

                if (!isMatchOne)
                {
                    break;
                }
            }

            curDst = isMatchOne ? 255 : 0;
        }
    }

    return true;
}

bool findContours(const CTensor<uint8> &src, std::vector<Contour> &contours)
{


    return true;
}
