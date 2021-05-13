#ifndef LAYERPOOLING_H
#define LAYERPOOLING_H

#include "layer/LayerBase.hpp"

template<typename DType>
class CLayerPooling : public CLayerBase<DType>
{
    enum Pool_Type
    {
        pool_type_max =0,
        pool_type_ave
    };

public:
    CLayerPooling() = delete;
    CLayerPooling(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerPooling() {}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    bool forward();
    bool backword();

private:
    int32 m_stride = 2;
    int32 m_kernel = 2;
    Pool_Type m_poolType = pool_type_max;
};

REGISTER_LAYER_ALLTYPE(Pooling)

#endif // LAYERPOOLING_H
