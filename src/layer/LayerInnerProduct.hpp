#ifndef LAYERINNERPRODUCT_H
#define LAYERINNERPRODUCT_H

#include "layer/LayerBase.hpp"

template<typename DType>
class CLayerInnerProduct : public CLayerBase<DType>
{
public:
    CLayerInnerProduct() = delete;
    CLayerInnerProduct(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerInnerProduct(){}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    bool forward();
    bool backward();
};

REGISTER_LAYER_ALLTYPE(InnerProduct)

#endif // LAYERINNERPRODUCT_H
