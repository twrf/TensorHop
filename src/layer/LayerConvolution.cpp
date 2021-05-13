#include "LayerConvolution.hpp"
#include "utils/math_function.hpp"

template<typename DType>
bool CLayerConvolution<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    this->linkTensor(map);

    // load config data
    const CConfigTable& param = this->m_cfg["param"];
    if (param.isMember("stride"))
    {
        m_stride_h = param["stride"].asInt();
        m_stride_w = m_stride_h;
    }
    else
    {
        m_stride_h = param.isMember("stride_h") ? param["stride_h"].asInt() : 1;
        m_stride_w = param.isMember("stride_w") ? param["stride_w"].asInt() : 1;
    }
    if (param.isMember("padding"))
    {
        m_padding_h = param["padding"].asInt();
        m_padding_w = m_padding_h;
    }
    else
    {
        m_padding_h = param.isMember("padding_h") ? param["padding_h"].asInt() : 0;
        m_padding_w = param.isMember("padding_w") ? param["padding_w"].asInt() : 0;
    }

    // resharp colBuffer
    // h_output = (h_input + 2 * padding_h - h_filter) / stride_h + 1
    // w_output = (w_input + 2 * padding_w - w_filter) / stride_w + 1
    // h_col = channel_input * h_filter * w_filter
    // w_col = h_output * w_output // or (h_output * w_output * batchsize)
    int32 h_output = (this->m_input.dim(dim_h) + 2 * m_padding_h - this->m_filter.dim(dim_h)) / m_stride_h + 1;
    int32 w_output = (this->m_input.dim(dim_w) + 2 * m_padding_w - this->m_filter.dim(dim_w)) / m_stride_w + 1;
    int32 h_col = this->m_input.dim(dim_c) * this->m_filter.dim(dim_h) * this->m_filter.dim(dim_w);
    int32 w_col = h_output * w_output;
    m_colBuffer.reset({w_col, h_col, this->m_filter.dim(dim_c), 1}, this->m_name + "_colbuf", false); // maybe use group, like caffe

    return true;
}

template<typename DType>
bool CLayerConvolution<DType>::forward()
{
    im2col_cpu(this->m_input.m_data.m_cpu.data(),
               this->m_input.dim(dim_c), this->m_input.dim(dim_w), this->m_input.dim(dim_w),
               this->m_filter.dim(dim_h), this->m_filter.dim(dim_w),
               m_padding_h, m_padding_w, m_stride_h, m_stride_w, 1.0, 1.0,
               m_colBuffer.m_data.m_cpu.begin());

    th_cpu_gemm(CblasNoTrans, CblasNoTrans,
                this->m_filter.dim(dim_h), m_colBuffer.dim(dim_w), this->m_filter.dim(dim_w),
                1.0, this->m_filter.m_data.m_cpu.data(), this->m_colBuffer.m_data.m_cpu.data(),
                1.0, this->m_output.m_data.m_cpu.begin());

    return true;
}

template<typename DType>
bool CLayerConvolution<DType>::backword()
{
    return true;
}
