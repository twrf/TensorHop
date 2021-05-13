#ifndef LAYERBASE_H
#define LAYERBASE_H

#include "memory/Tensor.h"
#include "utils/Tools.h"
#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <typeinfo>
#include <map>

enum ELayerPhase
{
    phase_train = 0,
    phase_test
};

class CLayerInterface
{
public:
    CLayerInterface() {}
    virtual ~CLayerInterface() {}
    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map) = 0;
    virtual bool forward() = 0;
    virtual bool backword() = 0;
//    virtual std::string getLayerName() const = 0;
};

template<typename DType>
class CLayerBase : public CLayerInterface
{
public:
    CLayerBase() = delete;
    CLayerBase(const CConfigTable& cfg);
    virtual ~CLayerBase() {}

//    void setLayerName(std::string layerName) {this->m_name = layerName;}
//    virtual std::string getLayerName() const {return m_name;}
    virtual bool linkTensor(const std::map<std::string, std::shared_ptr<CLayerInterface>> &map);
    virtual std::string getLayerType() const {return m_cfg["type"].asString();}

    virtual bool config(const CConfigTable& cfg) {return true;}
    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg)  {return true;}
    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map)
                            {return layerConfig(map, m_cfg);}
    virtual bool forward() {return true;}
    virtual bool backword() {return true;}

private:
    CMultiTensor<DType>& string2TensorName(const std::string& name);

public:
    const CConfigTable m_cfg;
    std::string m_name;
    std::string m_type; // layer type
    ELayerPhase m_phase;

    CMultiTensor<DType> m_input;    // input
    CMultiTensor<DType> m_filter;   // filter and weight
    CMultiTensor<DType> m_bias;     // weight data for bais
    CMultiTensor<DType> m_output;   // output
    CMultiTensor<DType> m_label;    // label
};

// Instantiate a class with float and double specifications.
#define INSTANTIATE_CLASS(classname) \
    char gInstantiationGuard##classname; \
    template class classname<int>; \
    template class classname<float>; \
    template class classname<double>;


enum DATA_TYPE_LENGTH
{
    type_8bit = 0,
    type_16bit,
    type_32bit,
    type_64bit
};

class CLayerFactory
{
public:
    template<typename T>
    struct SLayerRegister
    {
        SLayerRegister(const std::string& key)
        {
            CLayerFactory::instance().m_map.emplace(
                        key, [](const CConfigTable& cfg) { return new T(cfg); });
            infof("registering layer key: \"%s\"\n", key.c_str());
        }
    };

    static CLayerFactory& instance()
    {
        static CLayerFactory s_instance;
        return s_instance;
    }

    CLayerInterface* produce(const CConfigTable& cfg);

    std::shared_ptr<CLayerInterface> produce_shared(const CConfigTable& cfg);

private:
    CLayerFactory() {}
    CLayerFactory(const CLayerFactory&) = delete;
    CLayerFactory(CLayerFactory&&) = delete;
    ~CLayerFactory() {}

    std::map<std::string, std::function<CLayerInterface*(const CConfigTable& cfg)>> m_map;
};

#define REGISTER_LAYER_VNAME(key) _regLayer_##key##_
#define REGISTER_LAYER(T, key, ...) \
    static CLayerFactory::SLayerRegister<T> REGISTER_LAYER_VNAME(key)(#key, ##__VA_ARGS__);
#define REGISTER_LAYER_ONETYPE(key, DType, ...) \
    REGISTER_LAYER(CLayer##key<DType>, _##key##_##DType##_, ##__VA_ARGS__)
#define REGISTER_LAYER_ALLTYPE(key, ...) \
    REGISTER_LAYER_ONETYPE(key, int, ##__VA_ARGS__) \
    REGISTER_LAYER_ONETYPE(key, float, ##__VA_ARGS__) \
    REGISTER_LAYER_ONETYPE(key, double, ##__VA_ARGS__)

#define LAYER_CLASS_DEFINE_START(classname)                             \
template<typename DType>                                                \
class classname : public CLayerBase<DType>                              \
{                                                                       \
public:                                                                 \
    classname() = delete;                                               \
    classname(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}      \
    virtual ~classname() {}                                             \
    virtual bool forward();                                             \
    virtual bool backward();

#define LAYER_CLASS_DEFINE_END };

#endif // LAYERBASE_H
