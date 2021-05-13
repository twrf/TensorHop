#ifndef LAYERRELU_H
#define LAYERRELU_H

#include "layer/LayerBase.hpp"

template<typename DType>
class CLayerRelu : public CLayerBase<DType>
{
public:
    CLayerRelu() = delete;
    CLayerRelu(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerRelu() {}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    bool forward();
    bool backword();

private:

};

REGISTER_LAYER_ALLTYPE(Relu)

#endif // LAYERRELU_H
