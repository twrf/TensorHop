#include "LayerBase.hpp"

template<typename DType>
CLayerBase<DType>::CLayerBase(const CConfigTable &cfg) :
    m_cfg(cfg)
{
    m_name = m_cfg["name"].asString();
    m_type = m_cfg["type"].asString();
    m_phase = (m_cfg["phase"].asString() == "test") ? phase_test : phase_train;

    if (m_cfg.isMember("sharp") && m_cfg["sharp"] != Json::Value::null)
    {
        const CConfigTable& sharp = m_cfg["sharp"];
        if (sharp.isMember("filter"))
        {
            std::vector<int> vecFilter = convJsonArray2Vector(sharp["filter"]);
            m_filter.reset(vecFilter, m_name + "_filter");
        }
        if (sharp.isMember("bias"))
        {
            std::vector<int> vecBias = convJsonArray2Vector(sharp["bias"]);
            m_bias.reset(vecBias, m_name + "_bias");
        }
        if (sharp.isMember("output"))
        {
            std::vector<int> vecOutput = convJsonArray2Vector(sharp["output"]);
            bool isNeedDiff = (m_type == "Input") ? false : true;
            m_output.reset(vecOutput, m_name + "_output", isNeedDiff);
        }
    }
}

template<typename DType>
bool CLayerBase<DType>::linkTensor(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map)
{
    const static std::vector<std::string> s_tensorTypes = {"input", "output", "filter", "label"};
    for (uint32 typeIndex=0; typeIndex<s_tensorTypes.size(); ++typeIndex)
    {
        std::string distTensorName = s_tensorTypes[typeIndex];
        if (!m_cfg["sharp"].isMember(distTensorName) || m_cfg["sharp"][distTensorName] == Json::Value::null)
        {
            continue;
        }

        if (!(m_cfg["sharp"][distTensorName][0u].isString() || m_cfg["sharp"][distTensorName][0u].isObject()))
        {
            continue;
        }

        const CConfigTable& distTensor = m_cfg["sharp"][distTensorName];
        ASSERT(distTensor.size() == 1); // only support one for now
        const CConfigTable& one = distTensor[0u];
        std::string srcLayerName;
        std::string srcTensorName;
        if (one.isString())
        {
            srcLayerName = one.asString();
            srcTensorName = "output";
        }
        else if (one.isObject())
        {
            ASSERT(one.size() == 1);
            srcLayerName = one.getMemberNames()[0u];
            srcTensorName = one[srcLayerName].asString();
        }
        infof("%s(%s) <-- %s(%s)\n", m_name.c_str(), distTensorName.c_str(),
              srcLayerName.c_str(), srcTensorName.c_str());
#if 1
        if (map.find(srcLayerName) != map.end())
        {
            std::shared_ptr<CLayerBase<DType>> srcLayer = std::dynamic_pointer_cast<CLayerBase<DType>>(map.at(srcLayerName));
            this->string2TensorName(distTensorName) = srcLayer->string2TensorName(srcTensorName);
        }
        else
        {
            errorf("layer:%s, NOT find layer %s to link\n", m_name.c_str(), srcLayerName.c_str());
            ASSERT(1);
        }
#endif
    }

    return true;
}

template<typename DType>
CMultiTensor<DType> &CLayerBase<DType>::string2TensorName(const std::string &name)
{
    if (name == "input")
    {
        return m_input;
    }
    else if (name == "output")
    {
        return m_output;
    }
    else if (name == "bias")
    {
        return m_bias;
    }
    else if (name == "filetr")
    {
        return m_filter;
    }
    else if (name == "label")
    {
        return m_label;
    }

    return m_output; // not expected run here
}

CLayerInterface *CLayerFactory::produce(const CConfigTable &cfg)
{
    std::string key = cfg["type"].asString();
    std::string name = cfg["name"].asString();
    std::string dataType = cfg.isMember("dataType") ? cfg["dataType"].asString() : "double";
    infof("producing layer: %s, %s, %s\n", key.c_str(), name.c_str(), dataType.c_str());

    std::string final_key = "_"+key+"_"+dataType+"_";
    if (m_map.find(final_key) == m_map.end())
    {
        errorf("the layer key \"%s\" is not supported!\n", final_key.c_str());
        ASSERT(0);
    }

    return dynamic_cast<CLayerInterface*>(m_map[final_key](cfg));
}

std::shared_ptr<CLayerInterface> CLayerFactory::produce_shared(const CConfigTable &cfg)
{
    return  std::shared_ptr<CLayerInterface>(produce(cfg));
}

INSTANTIATE_CLASS(CLayerBase)
