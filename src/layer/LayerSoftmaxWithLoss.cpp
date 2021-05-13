#include "LayerSoftmaxWithLoss.hpp"

template<typename DType>
bool CLayerSoftmaxWithLoss<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    this->linkTensor(map);
    return true;
}

template<typename DType>
bool CLayerSoftmaxWithLoss<DType>::forward()
{
    return true;
}

template<typename DType>
bool CLayerSoftmaxWithLoss<DType>::backword()
{
    return true;
}
