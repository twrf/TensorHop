#ifndef SMOOTH_H
#define SMOOTH_H

#include "memory/Tensor.h"
#ifdef USE_OPENCV
#include "opencv2/core.hpp"
#endif
#include <math.h>
#include <float.h>

template<typename FilterType, typename TensorType>
class CFilterBase
{
public:
    // Various border types, image boundaries are denoted with '|'
    enum PaddingType
    {
        PADDING_DEFAULT,         // 000000|abcdefgh|0000000
        PADDING_REPLICATE,       // aaaaaa|abcdefgh|hhhhhhh
        PADDING_REFLECT,         // fedcba|abcdefgh|hgfedcb
        PADDING_REFLECT_101,     // gfedcb|abcdefgh|gfedcba
        PADDING_WRAP,            // cdefgh|abcdefgh|abcdefg
        PADDING_CONSTANT,        // iiiiii|abcdefgh|iiiiiii  with some specified 'i'
        PADDING_TRANSPARENT,     // uvwxyz|abcdefgh|ijklmno
//        PADDING_REFLECT101,      // same as PADDING_REFLECT_101
//        PADDING_DEFAULT,         // same as PADDING_REFLECT_101
//        PADDING_ISOLATED         // do not look outside of ROI
    };

public:
    CFilterBase() {}
    CFilterBase(std::vector<int> sharp)
    {
        mp_kernel = nullptr;//new CTensor<FilterType>(sharp);
    }
    ~CFilterBase()
    {
//        delete mp_kernel;
    }

//    virtual CTensor& createKernel() = 0;
    virtual CTensor<TensorType>& apply(CTensor<TensorType>& src, PaddingType type = PADDING_DEFAULT)
    {
        int srcDim = src.dim();
        ASSERT(PADDING_DEFAULT == type);
        ASSERT(2 == srcDim || 3 == srcDim);
        ASSERT(mp_kernel);

        CTensor<TensorType>* dst = new CTensor<TensorType>(src.sharp());
        int height = src.dim(dim_h);
        int width = src.dim(dim_w);
        int channel = (srcDim == 3) ? src.dim(dim_c) : 1;
        int kwr = mp_kernel->dim(dim_w) / 2;
        int khr = mp_kernel->dim(dim_h) / 2;
        int kernel_width = mp_kernel->dim(dim_w);

        for (int h=0; h<height; ++h)
        {
            for (int w=0; w<width; ++w)
            {
                for (int cn=0; cn<channel; ++cn)
                {
                    int offset = h*channel*width +  channel*w + cn;
                    TensorType& curDstPtr = dst->setAt(offset);
                    int tmp = 0;
                    double tmpD = 0.0f;
                    int cnt = 0;
                    for (int kh=-1*khr; kh<=khr; ++kh)
                    {
                        if (kh+h<0 || kh+h>=height)
                        {
                            continue;
                        }

                        for (int kw=-1*kwr; kw<=kwr; ++kw)
                        {
                            if (kw+w<0 || kw+w>=width)// || (0==kh && 0==kw))
                            {
                                continue;
                            }
                            cnt++;
                            TensorType& curSrcPtr = src.at(offset, kh, kw);
                            if (isGaussian)
                            {
                                tmpD += (double)curSrcPtr * mp_kernel->at(kernel_width*(kh+khr) + kw+kwr);
                            }
                            else if (isSobel)
                            {
                                tmp += (int)curSrcPtr * (int)(mp_kernel->at(kernel_width*(kh+khr) + kw+kwr));
                            }
                            else
                            {
                                tmp += ((int)(curSrcPtr));
                            }
                        }
                    }

                    if (isGaussian)
                    {
                        curDstPtr = (TensorType)((tmpD >= 255.0f) ? 255.0f : tmpD);
                    }
                    else if (isSobel)
                    {
                        curDstPtr = std::abs(tmp);
                    }
                    else
                    {
                        curDstPtr = (TensorType)(tmp / cnt);
                    }
                }
            }
        }

        return *dst;
    }

//    virtual bool apply(const CTensor& src, CTensor& dst);

private:


public:

protected:
    std::shared_ptr<CTensor<FilterType>> mp_kernel;
    bool isGaussian = false;
    bool isSobel = false;

private:
};


template<typename FilterType, typename TensorType>
class CBoxFilter : public CFilterBase <FilterType, TensorType>
{
public:
    CBoxFilter(int size)
    {
        this->mp_kernel = NULL;
        this->mp_kernel.reset(new CTensor<FilterType>(size));
        this->mp_kernel->setValue(1);
        this->isGaussian = false;
    }

private:

};


template<typename FilterType, typename TensorType>
class CGaussianFilter : public CFilterBase <FilterType, TensorType>
{
public:
//    CGaussianFilter() {}
    CGaussianFilter(int width, int height=0, FilterType sigma1=0, FilterType sigma2=0, int ktype=CV_64F)
    {
        CTensor<FilterType> kx = getGaussianKernel(width, sigma1);
        CTensor<FilterType> ky;
        if ((height == width || 0 == height) && std::abs(sigma1 - sigma2) < DBL_EPSILON)
        {
            ky = kx;
        }
        else
        {
            ky = getGaussianKernel(height, sigma2);
        }

        this->mp_kernel = NULL;
        this->mp_kernel.reset(&(kx.multiply(ky)));
        this->isGaussian = true;
    }


private:
    CTensor<FilterType> getGaussianKernel(int n, FilterType sigma, int ktype = CV_64F)
    {
        const int SMALL_GAUSSIAN_SIZE = 7;
        static const double small_gaussian_tab[][SMALL_GAUSSIAN_SIZE] =
        {
            {1.f},
            {0.238994266f, 0.522011469f, 0.238994266f},
            {0.070766371f, 0.244460399f, 0.369546460f, 0.244460399f, 0.070766371f},
            {0.028995265f, 0.103818351f, 0.223173361f, 0.288026046f, 0.223173361f, 0.103818351f, 0.028995265f}
        };

        const FilterType* fixed_kernel = n % 2 == 1 && n <= SMALL_GAUSSIAN_SIZE && sigma <= 0 ?
            (const FilterType*)small_gaussian_tab[n>>1] : 0;
        CTensor<FilterType> kernel(std::vector<int>{n});

        FilterType sigmaX = sigma > 0 ? sigma : ((n-1)*0.5 - 1)*0.3 + 0.8;
        FilterType scale2X = -0.5/(sigmaX*sigmaX);
        FilterType sum = 0;
        int offset = 0;
        for (int i=0; i<n; ++i)
        {
            FilterType x = i - (n - 1) * 0.5;
            FilterType t = fixed_kernel ? (FilterType)fixed_kernel[i] : exp(scale2X*x*x);
            kernel.at(offset) = t;
            sum += t;
            offset++;
        }

        sum = 1./sum;
        offset = 0;
        for (int i = 0; i < n; i++)
        {
            kernel.at(offset) /= sum;
            offset++;
        }

        return kernel;
    }


public:

private:
//    CTensor<FilterType> m_gaussianKernel;
};


template<typename FilterType, typename TensorType>
class CSobelFilter : public CFilterBase <FilterType, TensorType>
{
public:
    CSobelFilter(std::string name = "")
    {
        this->isSobel = true;
        this->mp_kernel = NULL;
        this->mp_kernel.reset(new CTensor<FilterType>({3, 3}));
        this->mp_kernel->fillValue({ -1, 0, 1,
                                     -2, 0, 2,
                                     -1, 0, 1 }); // Gx方向
    }

};

#endif // SMOOTH_H
