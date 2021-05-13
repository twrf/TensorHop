#ifndef TENSOR_H
#define TENSOR_H

#include "common/Defs.h"
#include <vector>
#include <cstring>

enum DIM_TYPE
{
    dim_w = 0,
    dim_h,
    dim_c,
    dim_n,
    dim_max
};

// template CTensor
template<typename DType>
class CTensor
{

public:
    // rgb
    // rgba
    // binary
    // grey
    // 多通道
    // 多个tensor组合成一个大的tensor

    CTensor()
    {
//        tracef("create one empty CTensor, addr:%p\n", this);
    }

    CTensor(const std::vector<int> sharp, std::string name = "")
    {
        m_dataDType = nullptr;
        init(sharp);
        m_name = name;
        tracef("construct tensor by sharp, addr:%p, name:\"%s\"\n", this, m_name.c_str());
    }

    CTensor(int size, std::string name = "")
    {
        ASSERT(size);
        m_name = name;
        std::vector<int> sharp;
        sharp.push_back(size);
        sharp.push_back(size);
        init(sharp);
        tracef("construct tensor by size, addr:%p\n", this);
    }

    CTensor(const CTensor<DType>& src)
    {
        m_name = src.m_name;
        m_sharp = src.m_sharp;
        m_dataDType = src.m_dataDType;
        m_dataBegin = src.m_dataBegin;
        m_dataEnd = src.m_dataEnd;
        m_lengthDType = src.m_lengthDType;
        m_lengthByte = src.m_lengthByte;
        m_lengthMultiple = src.m_lengthMultiple;
        tracef("construct tensor by copy, addr:%p, use_count:%ld, name:\"%s\"\n", this, m_dataDType.use_count(), m_name.c_str());
    }

    CTensor& operator = (const CTensor<DType> &src)
    {
        m_name = src.m_name;
        m_sharp = src.m_sharp;
        m_dataDType = src.m_dataDType;
        m_dataBegin = src.m_dataBegin;
        m_dataEnd = src.m_dataEnd;
        m_lengthDType = src.m_lengthDType;
        m_lengthByte = src.m_lengthByte;
        m_lengthMultiple = src.m_lengthMultiple;
        tracef("construct tensor by =, addr:%p, use_count:%ld, name:\"%s\"\n", this, m_dataDType.use_count(), m_name.c_str());
        return *this;
    }

    void reset(const std::vector<int> sharp, const std::string name = "")
    {
        m_dataDType = nullptr;
        m_name = name;
        init(sharp);
        tracef("construct tensor by reset, addr:%p, name:\"%s\"\n", this, m_name.c_str());
    }

    ~CTensor()
    {
        if (m_dataDType)
        {
            tracef("destruct tensor, addr:%p, name:%s\n", this, m_name.c_str());
        }
    }

    inline int dim() const
    {
        return m_sharp.size();
    }

    inline int dim(DIM_TYPE type) const
    {
        ASSERT(type < m_sharp.size());
        return m_sharp[type];
    }

    const std::vector<int> sharp() const {return m_sharp;}

#ifdef USE_OPENCV
    cv::Mat& conv2CvMat()
    {
        const static int map[] = {CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4};
        ASSERT(2 == m_sharp.size() || 3 == m_sharp.size());
        cv::Mat *tmp = new cv::Mat(m_sharp[0], m_sharp[1],
                3 == m_sharp.size() ? map[m_sharp[2] - 1] : CV_8UC1,
                atBegin()); // TODO: need more safe check
        return *tmp;
    }
#endif

    void show(std::string showName = "")
    {
#ifdef USE_OPENCV
        cv::Mat& tmp = this->conv2CvMat();
        cv::imshow(showName, tmp);
#endif
    }

    void save(std::string showName = "")
    {
        saveBMP(*this, showName + ".bmp");
    }

    void print() const
    {
        ASSERT(this->dim() <= 4 || this->dim() >= 1);
        uint32 width = dim(dim_w);
        uint32 height = (dim()>dim_h) ? dim(dim_h) :1;
        uint32 channel = (dim()>dim_c) ? dim(dim_c) : 1;
        uint32 batchsize = (dim()>dim_n) ? dim(dim_n) : 1;
        infof("name:\"%s\", data use_count:%ld, WHCN:[%d, %d, %d, %d]\n", m_name.c_str(),
              m_dataDType.use_count(), width, height, channel, batchsize);
        if (m_lengthDType > 500)
        {
            warnf("Tensor:\"%s\", data length:%d(>500), too larger to print\n", m_name.c_str(), m_lengthDType);
            return;
        }

        for (uint32 n=0; n<batchsize; ++n)
        {
            for (uint32 h=0; h<height; ++h)
            {
                for (uint32 c=0; c<channel; ++c)
                {
                    for (uint32 w=0; w<width; ++w)
                    {
                        DType* p = m_dataBegin + n*height*channel*width + c*height*width + h*width + w;
                        if (typeid(DType) == typeid(int))
                        {
                            printf("%d ", (int)(*p));
                        }
                        else if (typeid(DType) == typeid(float) || typeid(DType) == typeid(double))
                        {
                            printf("%f ", (double)(*p));
                        }
                        else
                        {
                            printf("- ");
                        }
                    }
                    printf("  ");
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    CTensor operator *(const CTensor<DType>& in) const
    {
        ASSERT(this->dim() == 1 && in.dim() == 1);
        CTensor out({in.dim(dim_w), in.dim(dim_w)}, "tmp");

        int width = in.dim(dim_w);
        for (int h=0; h<width; ++h)
        {
            for (int w=0; w<width; ++w)
            {
                out.setAt(h*width+w) = this->at(h) * in.at(w);
            }
        }

        return out;
    }


    // 获取一个R/B/G/A的值
    // 获取一个RGB或RGBA的值
    // 获取一行的值
    // 获取一列的值
    // 获取矩形框的值

    void setName(const std::string name)
    {
        m_name = name;
    }

    DType& at(int offset) const
    {
        ASSERT(m_dataDType);
        ASSERT(m_dataBegin + offset <= m_dataEnd);
        return *(m_dataBegin + offset);
    }

    DType& setAt(int offset)
    {
        ASSERT(m_dataDType);
        ASSERT(m_dataBegin + offset <= m_dataEnd);
        return *(m_dataBegin + offset);
    }

    DType& at(int offset, int kh, int kw) const
    {
        return *(&(at(offset)) + (kh*dim(dim_h) + kw));
    }

    const DType* const data() const {return m_dataBegin;}

    DType* const begin() const {return m_dataBegin;}

    int getDTypeLength() const {return m_lengthDType;}

    bool fill(DType val)
    {
        std::fill(m_dataDType.get(), m_dataDType.get()+m_lengthDType, val);
        return true;
    }

    bool fill(std::vector<DType> value)
    {
        ASSERT(m_lengthDType == value.size());
        memcpy(m_dataBegin, value.data(), m_lengthByte);

        return true;
    }

    template<typename DType2>
    bool fill(DType2* ptr, uint32 length, double scale = 1.0)
    {
        if (!ptr || length != m_lengthDType)
        {
            ASSERT(0);
            return false;
        }

        if (typeid(DType) == typeid(DType2))
        {
            memcpy(m_dataBegin, ptr, m_lengthByte);
        }
        else
        {
            for (uint32 i=0; i<length; ++i)
            {
                *(m_dataBegin+i) = (*(ptr+i) * scale);
            }
        }
        return true;
    }

    long getDataUseCount() const
    {
        return m_dataDType.use_count();
    }

private:
    bool init(const std::vector<int> &sharp)
    {
//        m_sharp.resize(sharp.size());
//        m_sharp.clear();
        m_sharp = sharp;
        m_lengthDType = 1;
        m_lengthMultiple = sizeof(DType) / sizeof(uint8);
        m_lengthByte = m_lengthDType * m_lengthMultiple;

        for (unsigned int i=0; i<m_sharp.size(); ++i)
        {
            ASSERT(m_sharp[i]);
            m_lengthDType = m_lengthDType * m_sharp[i];
        }

        m_lengthByte = m_lengthDType * sizeof(DType) / sizeof(uint8);
        tracef("m_lengthDType=%d, m_lengthByte=%d\n", m_lengthDType, m_lengthByte);
        if (/*nullptr == m_dataDType && */m_lengthByte > 0)
        {
            DType* tmp = callocMemory(m_lengthByte);
            ASSERT(tmp);
            tracef("malloc memory:%p, byteLength:%d\n", tmp, m_lengthByte);
            m_dataDType.reset(tmp, [](DType* ptr)
                        {tracef("free memory:%p\n", ptr); free(ptr);});
        }

        m_dataBegin = m_dataDType.get();
        m_dataEnd = m_dataDType.get() + m_lengthDType;

        return true;
    }

    DType* callocMemory(int sizeOfByte) // 暂时使用操作系统自带的malloc或alloc，后面再使用MemoryManager管理起来
    {
        DType* data = (DType*)malloc(sizeOfByte);
        memset(data, 0x00, sizeOfByte);

        return data;
    }

    bool freeMemory(void *ptr)
    {
        infof("free ptr\n");
        free(ptr);
        return true;
    }

public:
    enum DimType
    {
        DIMTYPE_BINARY = 0,
        DIMTYPE_GREY,
        DIMTYPE_RGB,
        DIMTYPE_RGBA,
        DIMTYPE_UNKNOW
    };

private:
    std::string m_name = "";
    std::vector<int> m_sharp; // WHCN, [0]->width [1]->height [2]->channel [3]->batchSize

    uint32 m_lengthDType = 0;
    uint32 m_lengthMultiple = 0;
    uint32 m_lengthByte = 0; // lenght of bytes (uint8)
    std::shared_ptr<DType> m_dataDType = nullptr;
    DType *m_dataBegin = nullptr;
    DType *m_dataEnd = nullptr;
};

template<typename DType>
class CSyncTensor
{
    enum SyncState
    {
        head_null = 0,
        head_cpu,
        head_gpu
    };

public:
    CSyncTensor() {}

    CSyncTensor& operator = (const CSyncTensor<DType> &src)
    {
        m_name = src.m_name;
        m_state = src.m_state;
        m_cpu = src.m_cpu;
//        m_gpu = src.m_gpu;
        return *this;
    }

    void reset(const std::vector<int> sharp, const std::string name, bool isUseGpu = false)
    {
        m_name = name;
        m_cpu.reset(sharp, name + "_cpu");
    }

//    std::shared_ptr<CTensor<DType>>& getCpuData() {return m_cpu;}
//    std::shared_ptr<CTensor<DType>>& getGpuData() {return m_gpu;}

    inline int dim() const
    {
        return m_cpu.dim();
    }

    inline int dim(DIM_TYPE type) const
    {
        return m_cpu.dim(type);
    }


private:
    bool syncFromCpu2Gpu() {return true;}
    bool syncFromGpu2Cpu() {return true;}
    void move2Gpu() {}
    void move2Cpu() {}

public:
//    std::shared_ptr<CTensor<DType>> m_cpu;
    CTensor<DType> m_cpu;

//    std::shared_ptr<CTensor<DType>> m_gpu;

private:
    std::string m_name = "";
    SyncState m_state = head_null;
};

template<typename DType>
class CMultiTensor
{
public:
    CMultiTensor() {}
    CMultiTensor(const std::vector<int> sharp, const std::string name, bool isNeedDiff = true)
    {
        reset(sharp, name, isNeedDiff);
    }

    CMultiTensor& operator = (const CMultiTensor<DType> &src)
    {
        m_name = src.m_name;
        m_data = src.m_data;
        m_diff = src.m_diff;
//        m_sharp = src.m_sharp;
        return *this;
    }

    bool reset(const std::vector<int> sharp, const std::string name, bool isNeedDiff = true)
    {
        // if necessary, clean or free old data

//        m_sharp = sharp;
        m_name = name;
        m_isNeedDiff = isNeedDiff;

        m_data.reset(sharp, name + "_data");
        if (isNeedDiff)
        {
            m_diff.reset(sharp, name + "_diff");
        }
        return true;
    }

    inline int dim() const
    {
        return m_data.dim();
    }

    inline int dim(DIM_TYPE type) const
    {
        return m_data.dim(type);
    }

public:
    CSyncTensor<DType> m_data;
    CSyncTensor<DType> m_diff;

private:
//    std::vector<int> m_sharp;
    std::string m_name = "";
    bool m_isNeedDiff = true;
};


extern bool saveBMP(CTensor<uint8>& tensor, std::string filename);


#endif // TENSOR_H
