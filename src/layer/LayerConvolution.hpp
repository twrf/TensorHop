#ifndef LAYERCONVOLUTION_H
#define LAYERCONVOLUTION_H

#include "layer/LayerBase.hpp"

template<typename DType>
class CLayerConvolution : public CLayerBase<DType>
{
public:
    CLayerConvolution() = delete;
    CLayerConvolution(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerConvolution() {}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    bool forward();
    bool backword();

private:
    int32 m_stride_h = 1;
    int32 m_stride_w = 1;
    int32 m_padding_h = 0;
    int32 m_padding_w = 0;
    CMultiTensor<DType> m_colBuffer;
};

REGISTER_LAYER_ALLTYPE(Convolution)

#endif // LAYERCONVOLUTION_H
