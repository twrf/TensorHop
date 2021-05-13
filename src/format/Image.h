#ifndef IMAGE_H
#define IMAGE_H

#include "common/Defs.h"
#include "memory/Tensor.h"

class CImageBase
{
public:
    CImageBase() {}

    virtual void show(std::string name = "")
    {
        if (NULL == mp_tensor)
        {
            errorf("mp_tensor is null\n");
            return;
        }

        mp_tensor->show(m_name);
    }

    virtual void imwrite(std::string name = "")
    {
        if (NULL == mp_tensor)
        {
            errorf("mp_tensor is null\n");
            return;
        }

        mp_tensor->save(m_name);
    }

    std::vector<int> sharp() const
    {
        return mp_tensor->sharp();
    }

    CTensor<uint8>& getTensor() const
    {
        return *mp_tensor;
    }

//    virtual bool operate();

protected:
    std::shared_ptr<CTensor<uint8>> mp_tensor;
    std::string m_name;
};


class CColorImage : public CImageBase
{
public:
    CColorImage() {}
    CColorImage(const CTensor<uint8>& tensor, std::string name = "RGB")
    {
        ASSERT(3 == tensor.dim());
        ASSERT(3 == tensor.dim(dim_c));

        mp_tensor.reset(new CTensor<uint8>(tensor));
        m_name = name;
    }

};


class CGrayImage : public CImageBase
{
public:
    CGrayImage() {}

    CGrayImage(const CColorImage& color, std::string name = "GRAY")
    {
        m_name = name;
        const std::vector<int> colorSharp = color.sharp();
        ASSERT(3 == colorSharp.size());
        ASSERT(3 == colorSharp[2]);

        mp_tensor.reset(new CTensor<uint8>({colorSharp[dim_w], colorSharp[dim_h]}));

        int count = colorSharp[dim_w] * colorSharp[dim_h];
        CTensor<uint8>& colorTensor = color.getTensor();
        for (int i=0; i<count; ++i)
        {
             int tmp = (uint8)(colorTensor.at(3*i + 0) * 0.11)
                    + (uint8)(colorTensor.at(3*i + 1) * 0.49)
                    + (uint8)(colorTensor.at(3*i + 2) * 0.3);
            mp_tensor->at(i) = tmp > 255 ? 255 : tmp;
        }
    }

    CGrayImage(const CTensor<uint8>& color, std::string name = "GRAY")
    {
        m_name = name;
        ASSERT(3 == color.dim());
        ASSERT(3 == color.dim(dim_c));

        mp_tensor.reset(new CTensor<uint8>({color.dim(dim_w), color.dim(dim_h)}));
        int count = color.dim(dim_w) * color.dim(dim_h);
        CTensor<uint8>& t = const_cast<CTensor<uint8>&>(color);
        for (int i=0; i<count; ++i)
        {
             int tmp = (uint8)(t.at(3*i + 0) * 0.11)
                    + (uint8)(t.at(3*i + 1) * 0.49)
                    + (uint8)(t.at(3*i + 2) * 0.3);
            mp_tensor->at(i) = tmp > 255 ? 255 : tmp;
        }
    }
};


class CBinaryImage : public CImageBase
{
public:
    CBinaryImage() {}
    CBinaryImage(CTensor<uint8>& gray, std::string name = "BIN")
    {
        m_name = name;
        ASSERT(2 == gray.dim());

        mp_tensor.reset(new CTensor<uint8>(gray.sharp()));
        uint32 count = gray.dim(dim_w) *gray.dim(dim_h);
        uint32 total = 0;
        for (uint32 i=0; i<count; ++i)
        {
            total += gray.at(i);
        }

        uint32 avg = (uint32)(total / count);
        ASSERT(avg <= 255);
        for (uint32 i=0; i<count; ++i)
        {
            mp_tensor->at(i) = (uint8)(gray.at(i) >= avg + 70 ? 255 : 0);
        }
    }
};

#endif // IMAGE_H
