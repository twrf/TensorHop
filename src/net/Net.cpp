#include "Net.hpp"
#include <fstream>

CNet::CNet(const std::string file)
{
    m_root = Json::Value::null;
    praseNetJson(file, m_root);

    // check net json

    // finetune net json

    createLayers(m_root["Layer"]);
    infof("finish create layers\n");

    for (auto itor : m_layersList) {itor->layerConfig(this->m_layersMap);}
    infof("finish config layers\n");
}

bool CNet::praseNetJson(const std::string &file, CConfigTable &root)
{
    std::string filename = file;
    Json::Reader reader;
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        errorf("Open %s failed\n", filename.c_str());
        ASSERT(0);
        return false;
    }

    infof("Open %s succed\n", filename.c_str());
    if (reader.parse(in, root, false))
    {
        infof("Prase %s succeed\n", filename.c_str());
//        infof("root: %s\nm", root.toStyledString().c_str());
    }
    else
    {
        errorf("Prase %s failed\n", filename.c_str());
        ASSERT(0);
        return false;
    }

    return true;
}

bool CNet::createLayers(const CConfigTable &layers)
{
    for (uint32 index=0; index<layers.size(); ++index)
    {
        const CConfigTable& cfg = layers[index];
        createLayer(cfg);
    }
    return true;
}

std::shared_ptr<CLayerInterface> CNet::createLayer(const CConfigTable &cfg)
{
    if (!cfg.isMember("type") || !cfg.isMember("name"))
    {
        errorf("this layer config do NOT have type or name: %s\n",
               cfg.toStyledString().c_str());
        ASSERT(0);
    }

    std::string name = cfg["name"].asString();
    if (m_layersMap.find(name) != m_layersMap.end())
    {
        errorf("find same layer name, name:\"%s\"\n", name.c_str());
        ASSERT(0);
    }

    m_layersMap[name] = CLayerFactory::instance().produce_shared(cfg);
    m_layersList.push_back(m_layersMap[name]);
    return m_layersMap[name];
}

CNetManager::CNetManager()
{

}

