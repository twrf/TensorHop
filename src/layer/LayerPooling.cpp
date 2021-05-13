#include "LayerPooling.hpp"
#include <algorithm>

template<typename DType>
bool CLayerPooling<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    this->linkTensor(map);

    // load config data
    const CConfigTable& param = this->m_cfg["param"];
    m_stride = param.isMember("stride") ? param["stride"].asInt() : 2;
    m_kernel = param.isMember("kernel") ? param["kernel"].asInt() : 2;
    m_poolType = (param["type"].asString() == "max") ? pool_type_max : pool_type_ave;

    return true;
}

template<typename DType>
bool CLayerPooling<DType>::forward()
{
    CTensor<DType>& input = this->m_input.m_data.m_cpu;
    CTensor<DType>& output = this->m_output.m_data.m_cpu;
    ASSERT(input.dim(dim_c) == output.dim(dim_c));

    for (int32 c=0; c<output.dim(dim_c); ++c)
    {
        for (int32 h=0; h<output.dim(dim_h); ++h)
        {
            for (int32 w=0; w<output.dim(dim_w); ++w)
            {
                double tmp = 0.0;
                int32 offsetOutput = c*output.dim(dim_h)*output.dim(dim_w)
                        + h*output.dim(dim_w) + w;
                int32 offsetInput = c*input.dim(dim_h)*input.dim(dim_w)
                        + h*input.dim(dim_w)*m_stride + w*m_stride;
                for (int32 kh=0; kh<m_kernel; ++kh)
                {
                    for (int32 kw=0; kw<m_kernel; ++kw)
                    {
                        if (m_poolType == pool_type_ave)
                        {
                            tmp += input.at(offsetInput, kh, kw);
                        }
                        else if (m_poolType == pool_type_max)
                        {
                            tmp = std::max(input.at(offsetInput, kh, kw), (DType)tmp);
                        }
                    }
                }

                if (m_poolType == pool_type_ave)
                {
                    output.at(offsetOutput) = tmp / (m_kernel * m_kernel);
                }
                else if (m_poolType == pool_type_max)
                {
                    output.at(offsetOutput) = tmp;
                }
            }
        }
    }

    return true;
}

template<typename DType>
bool CLayerPooling<DType>::backword()
{
    return true;
}
