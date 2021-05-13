#include "Tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if (0)

//CTensor::CTensor(int b, int c, int w, int h, int d, std::string name) :
//    data(NULL), dataLength(0), batch(b), channel(c), width(w), height(h), depth(d)
//{
//    this->name = name;

//    int sizeOfByte = batchSize * channel * width * height * depth;
//    if (sizeOfByte > 0) // 至少要有一个点
//    {
//        this->data = this->allocMemory(sizeOfByte);
//        this->dataLength = sizeOfByte;
//    }
//    else
//    {
//        errorf("init tensor[%s] sizeOfByte is [%d], which is not expected !!!\n",
//               this->name.c_str(), sizeOfByte);
//        ASSERT(0);
//    }

//    sharp.clear();
//}

//CTensor::CTensor(int dim, ...) :
//    m_data(NULL),
//    m_dataBegin(NULL),
//    m_dataEnd(NULL)
//{
//    ASSERT(0 != dim);

//    m_sharp.resize(dim);
//    m_sharp.clear();
//    m_dataLength = 1;

//    va_list list;
//    va_start(list, dim);
//    for (int i=0; i<dim; ++i)
//    {
//        m_sharp.push_back(va_arg(list, int));
//    }
//    va_end(list);

//    for (unsigned int i=0; i<m_sharp.size(); ++i)
//    {
//        tracef("m_sharp[%d] = %d\n", i, m_sharp[i]);
//        if (i !=  m_sharp.size() && m_sharp[i] <= 1)
//        {
//            ASSERT(1 != m_sharp[i]);
//        }
//        m_dataLength = m_dataLength * m_sharp[i];
//    }

//    infof("m_dataLength = %d\n", m_dataLength);
//    if (NULL == m_data)
//    {
//        m_data = callocMemory(m_dataLength);
//    }
//}

CTensor::CTensor(std::vector<int> sharp) :
    m_data(NULL),
    m_dataBegin(NULL),
    m_dataEnd(NULL)
{
    ASSERT(sharp.size());
    init(sharp);
}

CTensor::CTensor(int size)
{
    ASSERT(size);
    std::vector<int> sharp;
    sharp.push_back(size);
    sharp.push_back(size);
    init(sharp);
}

CTensor::CTensor(const CTensor &src)
{
    m_sharp = src.m_sharp;
    m_data = src.m_data;
    m_dataBegin = src.m_dataBegin;
    m_dataEnd = src.m_dataEnd;
    m_dataLength = src.m_dataLength;
    infof("m_data count = %ld\n", m_data.use_count());
}

CTensor &CTensor::operator =(const CTensor &src)
{
    CTensor dst(src.sharp());
    dst.m_sharp = src.m_sharp;
    dst.m_data = src.m_data;
    dst.m_dataBegin = src.m_dataBegin;
    dst.m_dataEnd = src.m_dataEnd;
    dst.m_dataLength = src.m_dataLength;
    infof("dst.m_data count = %ld\n", m_data.use_count());

    return *this;
}

CTensor::~CTensor()
{
    // share_ptr会自动调用free进行内存释放

//    if (m_data)
//    {
//        freeMemory(m_data);
//    }
}

unsigned char *CTensor::at(unsigned char *ptr)
{
    ASSERT(m_data);
    ASSERT(ptr >= m_dataBegin);
    ASSERT(ptr <= m_dataEnd);
    return ptr;
}

unsigned char *CTensor::at(int offset)
{
    ASSERT(m_data);
    return m_data.get() + offset;
}

unsigned char *CTensor::at(unsigned char *start, int offset)
{
    ASSERT(m_data);
    ASSERT(start);
    ASSERT(start + offset >= m_dataBegin);
    ASSERT(start + offset <= m_dataEnd);
    return start + offset;
}

unsigned char *CTensor::at(unsigned char *center, int kh, int kw)
{
    return (center + kh*m_sharp[2]*m_sharp[1] + m_sharp[2]*kw);
}

unsigned char *CTensor::atBegin()
{
    return m_dataBegin;
}

unsigned char *CTensor::atEnd()
{
    return m_dataEnd;
}

bool CTensor::setValue(unsigned char val)
{
    memset(m_data.get(), val, m_dataLength);
    return true;
}

bool CTensor::init(std::vector<int> &sharp)
{
    m_sharp.resize(sharp.size());
    m_sharp.clear();
    m_sharp = sharp;
    m_dataLength = 1;

    for (unsigned int i=0; i<m_sharp.size(); ++i)
    {
        tracef("m_sharp[%d] = %d\n", i, m_sharp[i]);
        if (i !=  m_sharp.size() && m_sharp[i] <= 1)
        {
            ASSERT(m_sharp[i]);
        }
        m_dataLength = m_dataLength * m_sharp[i];
    }

    infof("m_dataLength = %d\n", m_dataLength);
    if (NULL == m_data)
    {
        unsigned char* tmp = callocMemory(m_dataLength);
        ASSERT(tmp);
        tracef("malloc memory:%p, len:%d\n", tmp, m_dataLength);
        m_data.reset(tmp, [this](unsigned char* ptr){tracef("free memor:%p, len:%d\n", ptr, m_dataLength); free(ptr);});
    }

    m_dataBegin = m_data.get();
    m_dataEnd = m_data.get() + m_dataLength;

    return true;
}

//unsigned char* CTensor::at(int b, int c, int w, int h, int d)
//{
//    return (this->data + b*c*w*h*d);
//}

//unsigned char *CTensor::at(int num)
//{
//    va_list vl;     //va_list指针，用于va_start取可变参数，为char*
//    va_start(vl,num);       //取得可变参数列表中的第一个值
//    tracef("num:%d, vl:%d\n", num, *vl);
//}

unsigned char* CTensor::callocMemory(int sizeOfByte)
{
    unsigned char* data = (unsigned char*)malloc(sizeOfByte);
    memset(data, 0x00, sizeOfByte);

    return data;
}

bool CTensor::freeMemory(void *ptr)
{
    infof("free ptr\n");
    free(ptr);
    return true;
}

#else

template<typename DType>
CTensor<DType>::CTensor()
{

}

template<typename DType>
CTensor<DType>::CTensor(std::vector<int> sharp, std::string name)
{
    m_dataDType(NULL);
    init(sharp);
    m_name = name;
}

template<typename DType>
CTensor<DType>::CTensor(int size, std::string name)
{
    ASSERT(size);
    m_name = name;
    std::vector<int> sharp;
    sharp.push_back(size);
    sharp.push_back(size);
    init(sharp);
}

template<typename DType>
CTensor<DType>::CTensor(const CTensor<DType> &src)
{
    m_name = src.m_name;
    m_sharp = src.m_sharp;
    m_dataDType = src.m_dataDType;
    m_dataBegin = src.m_dataBegin;
    m_dataEnd = src.m_dataEnd;
    m_lengthDType = src.m_lengthDType;
    infof("m_dataDType count = %d\n", m_dataDType.use_count());
}

template<typename DType>
CTensor<DType> &CTensor<DType>::operator =(const CTensor<DType> &src)
{
#if 0
    CTensor dst;
    dst.m_name = src.m_name;
    dst.m_sharp = src.m_sharp;
    dst.m_dataDType = src.m_dataDType;
    dst.m_lengthDType = src.m_lengthDType;
    dst.m_lengthByte = src.m_lengthByte;
    dst.m_lengthMultiple = src.m_lengthMultiple;
    dst.m_dataBegin = src.m_dataBegin;
    dst.m_dataEnd = src.m_dataEnd;
#else
    m_name = src.m_name;
    m_sharp = src.m_sharp;
    m_dataDType = src.m_dataDType;
    m_lengthDType = src.m_lengthDType;
    m_lengthByte = src.m_lengthByte;
    m_lengthMultiple = src.m_lengthMultiple;
    m_dataBegin = src.m_dataBegin;
    m_dataEnd = src.m_dataEnd;
#endif
    infof("dst.m_dataDType count = %d\n", m_dataDType.use_count());

    return *this;
}

template<typename DType>
CTensor<DType>::~CTensor()
{

}

template<typename DType>
unsigned char *CTensor<DType>::at(DType *ptr)
{
    ASSERT(m_dataDType);
    ASSERT(ptr >= m_dataBegin);
    ASSERT(ptr <= m_dataEnd);
    return ptr;
}

template<typename DType>
unsigned char *CTensor<DType>::at(int offset)
{
    ASSERT(m_dataDType);
    ASSERT(m_dataBegin + offset <= m_dataEnd);
    return m_dataBegin + offset;
}

template<typename DType>
unsigned char *CTensor<DType>::at(DType *start, int offset)
{
    ASSERT(m_dataDType);
    ASSERT(start);
    ASSERT(start + offset >= m_dataBegin);
    ASSERT(start + offset <= m_dataEnd);
    return start + offset;
}

template<typename DType>
unsigned char *CTensor<DType>::at(DType *center, int kh, int kw)
{
    return (center + kh*m_sharp[2]*m_sharp[1] + m_sharp[2]*kw);
}

template<typename DType>
bool CTensor<DType>::setValue(DType val)
{
    memset(m_dataDType.get(), val, m_lengthDType);
    return true;
}

template<typename DType>
bool CTensor<DType>::init(std::vector<int> &sharp)
{
    m_sharp.resize(sharp.size());
    m_sharp.clear();
    m_sharp = sharp;
    m_lengthDType = 1;
    m_lengthMultiple = sizeof(DType) / sizeof(unsigned char);
    m_lengthByte = m_lengthDType * m_lengthMultiple;

    for (unsigned int i=0; i<m_sharp.size(); ++i)
    {
        tracef("m_sharp[%d] = %d\n", i, m_sharp[i]);
        if (i !=  m_sharp.size() && m_sharp[i] <= 1)
        {
            ASSERT(m_sharp[i]);
        }
        m_lengthDType = m_lengthDType * m_sharp[i];
    }

    m_lengthByte = m_lengthDType * sizeof(DType) / sizeof(unsigned char);
    infof("m_lengthDType=%d, m_lengthByte=%d\n", m_lengthDType, m_lengthByte);
    if (NULL == m_dataDType)
    {
        DType* tmp = callocMemory(m_lengthByte);
        ASSERT(tmp);
        tracef("malloc memory:%p, len:%d\n", tmp, m_lengthByte);
        m_dataDType.reset(tmp, [this](DType* ptr){tracef("free memor:%p, UClen:%d\n", ptr, m_lengthByte); free(ptr);});
    }

    m_dataBegin = m_dataDType.get();
    m_dataEnd = m_dataDType.get() + m_lengthDType;

    return true;
}

template<typename DType>
DType *CTensor<DType>::callocMemory(int sizeOfByte)
{
    DType* data = (DType*)malloc(sizeOfByte);
    memset(data, 0x00, sizeOfByte);

    return data;
}

template<typename DType>
bool CTensor<DType>::freeMemory(void *ptr)
{
    infof("free ptr\n");
    free(ptr);
    return true;
}


#endif
