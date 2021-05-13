#ifndef LAYERSOFTMAXWITHLOSS_H
#define LAYERSOFTMAXWITHLOSS_H

#include "layer/LayerBase.hpp"

template<typename DType>
class CLayerSoftmaxWithLoss : public CLayerBase<DType>
{
public:
    CLayerSoftmaxWithLoss() = delete;
    CLayerSoftmaxWithLoss(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerSoftmaxWithLoss() {}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    bool forward();
    bool backword();

private:

};

REGISTER_LAYER_ALLTYPE(SoftmaxWithLoss)

#endif // LAYERSOFTMAXWITHLOSS_H
