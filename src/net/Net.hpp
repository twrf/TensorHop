#ifndef NET_H
#define NET_H

#include "common/Defs.h"
#include "layer/LayerBase.hpp"
#include <list>
#include <map>

class CNet
{
public:
    CNet() = delete;
    CNet(const std::string file);

private:
    bool praseNetJson(const std::string& file, CConfigTable& root);
    bool createLayers(const CConfigTable& layers);

    std::shared_ptr<CLayerInterface> createLayer(const CConfigTable& cfg);

    template<typename DType>
    std::shared_ptr<CLayerBase<DType>> createLayer(const CConfigTable& cfg)
    {
        return std::dynamic_pointer_cast<CLayerBase<DType>>(createLayer(cfg));
    }

private:
    CConfigTable m_root = Json::Value::null;

//    CMutex m_mutexLayers; // both for map and list
    std::map<std::string, std::shared_ptr<CLayerInterface>> m_layersMap;
    std::list<std::shared_ptr<CLayerInterface>> m_layersList;
};

class CNetManager
{
public:
    CNetManager();
};

#endif // NET_H
