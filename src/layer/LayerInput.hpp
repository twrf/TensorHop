#ifndef LAYERINPUT_H
#define LAYERINPUT_H

#include "layer/LayerBase.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>


template<typename DType>
class CLayerInput : public CLayerBase<DType>
{
    struct SMmapFile
    {
        SMmapFile() = delete;
        SMmapFile(const std::string& file);
        ~SMmapFile();

    public:
        uint8* m_p;
        long m_length;
    };

public:
    CLayerInput() = delete;
    CLayerInput(const CConfigTable& cfg) : CLayerBase<DType>(cfg) {}
    virtual ~CLayerInput() {}

    virtual bool layerConfig(const std::map<std::string, std::shared_ptr<CLayerInterface>>& map,
                             const CConfigTable& cfg);
    virtual bool forward();
    virtual bool backward();

    uint32 getDatasetSize() const {return m_datasetSize;}
    uint32 getLabelsetSize() const {return m_labelsetSize;}

private:
    void loadDatasetSize();
    void loadLaeblsetSize();
//    void loadDataset();
//    void loadLabelset();

private:
    std::shared_ptr<SMmapFile> m_mmapData;
    std::shared_ptr<SMmapFile> m_mmapLabel;
    std::string m_sourceData;
    std::string m_sourceLabel;
    double m_scale = 1.0;
    uint32 m_datasetSize = 0;
    uint32 m_labelsetSize = 0;
    uint32 m_index = 0;
};

REGISTER_LAYER_ALLTYPE(Input)

#endif // LAYERINPUT_H
