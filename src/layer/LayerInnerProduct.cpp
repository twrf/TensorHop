#include "LayerInnerProduct.hpp"
#include "utils/math_function.hpp"

template<typename DType>
bool CLayerInnerProduct<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    this->linkTensor(map);
    return true;
}

template<typename DType>
bool CLayerInnerProduct<DType>::forward()
{
//    output = input * weight;
    CTensor<DType>& input = this->m_input.m_data.m_cpu;
    CTensor<DType>& filter = this->m_filter.m_data.m_cpu;
    CTensor<DType>& output = this->m_output.m_data.m_cpu;
    th_cpu_gemm(CblasNoTrans, CblasNoTrans,
                input.dim(dim_h), filter.dim(dim_w), input.dim(dim_h),
                1.0, input.data(), filter.data(),
                1.0, output.begin());

    return true;
}

template<typename DType>
bool CLayerInnerProduct<DType>::backward()
{
    // step-1: Output Gradient will be calcuated and updated by top layer(s)

    // step-2: update Weight Gradient

    // step-3: update Bias Gradient

    // step-1: update Input Gradient

    return true;
}
