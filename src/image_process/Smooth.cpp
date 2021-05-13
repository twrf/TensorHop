#include "Smooth.h"

template<typename FilterType, typename TensorType>
CFilterBase<FilterType, TensorType>::CFilterBase(std::vector<int> sharp)
{
//    kernel = new CTensor(sharp);
}

template<typename FilterType, typename TensorType>
CFilterBase<FilterType, TensorType>::~CFilterBase()
{
    delete kernel;
}

template<typename FilterType, typename TensorType>
CTensor<TensorType> &CFilterBase<FilterType, TensorType>::apply(const CTensor<TensorType> &src, CFilterBase::PaddingType type)
{

}

//CTensor &CFilterBase::apply(CTensor &src, CFilterBase::PaddingType type)
//{
//    ASSERT(PADDING_DEFAULT == type);
//    ASSERT(3 == src.dim());

//    CTensor* dst = new CTensor(src.sharp());
//    int height = src.sharp()[0];
//    int width = src.sharp()[1];
//    int channel = src.sharp()[2]; // channel is default to be the last DIM
//    int kwr = kernel->sharp()[0] / 2;
//    int khr = kernel->sharp()[1] / 2;

//    for (int h=0; h<height; ++h)
//    {
//        for (int w=0; w<width; ++w)
//        {
//            for (int cn=0; cn<channel; ++cn)
//            {
//                int offset = h*channel*width +  channel*w + cn;
//                unsigned char* curDstPtr = dst->at(offset);
//                int tmp = 0;
//                int cnt = 0;
//                for (int kh=-1*khr; kh<=khr; ++kh)
//                {
//                    if (kh+h<0 || kh+h>=height)
//                    {
//                        continue;
//                    }

//                    for (int kw=-1*kwr; kw<=kwr; ++kw)
//                    {
//                        if (kw+w<0 || kw+w>=width)// || (0==kh && 0==kw))
//                        {
//                            continue;
//                        }
//                        cnt++;
//                        unsigned char* curSrcPtr = src.at(src.at(offset), kh, kw);
//                        tmp += ((int)(*curSrcPtr));
//                    }
//                }
//#if 0
//                *curDstPtr = (unsigned char)((tmp>=255) ? 255 : tmp);
//#else
//                *curDstPtr = (unsigned char)(tmp/cnt);
//#endif
//            }
//        }
//    }

//    return *dst;
//}

//CBoxFilter::CBoxFilter(int size)
//{
//    kernel = new CTensor<unsigned char>(size);
//    kernel->setValue(1);
//}

//CGaussianFilter::CGaussianFilter(int width, int height, int ktype)
//{

//}

//CGaussianFilter::CGaussianFilter(double sigma, int ktype)
//{

//}
