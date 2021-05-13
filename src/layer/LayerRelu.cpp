#include "LayerRelu.hpp"

template<typename DType>
bool CLayerRelu<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    this->linkTensor(map);
    return true;
}

template<typename DType>
bool CLayerRelu<DType>::forward()
{
    return true;
}

template<typename DType>
bool CLayerRelu<DType>::backword()
{
    return true;
}
