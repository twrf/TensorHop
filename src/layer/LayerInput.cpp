#include "LayerInput.hpp"

template<typename DType>
bool CLayerInput<DType>::layerConfig(
        const std::map<std::string, std::shared_ptr<CLayerInterface> > &map,
        const CConfigTable &cfg)
{
    if (cfg.isMember("sharp") && cfg["sharp"] != Json::Value::null)
    {
        const CConfigTable& sharp = cfg["sharp"];
        if (sharp.isMember("label"))
        {
            std::vector<int> vecLabel = convJsonArray2Vector(sharp["label"]);
            this->m_label.reset(vecLabel, this->m_name + "_label", false);
        }
    }
    else
    {
        errorf("input layer not contain label\n");
    }

    m_scale = this->m_cfg["param"]["scale"].asDouble();
    m_sourceData = this->m_cfg["param"]["source_data"].asString();
    m_sourceLabel = this->m_cfg["param"]["source_label"].asString();
    m_mmapData.reset(new SMmapFile(m_sourceData));
    m_mmapLabel.reset(new SMmapFile(m_sourceLabel));

    this->linkTensor(map);

    return true;
}

template<typename DType>
bool CLayerInput<DType>::forward()
{
    static uint32 dataStep = this->m_cfg["sharp"]["output"][0u].asUInt()
            * this->m_cfg["sharp"]["output"][1].asUInt();
    if (m_index >= m_datasetSize || m_datasetSize != m_labelsetSize)
    {
        warnf("m_index:%d, m_datasetSize:%d\n", m_index, m_datasetSize);
        return false;
    }

    this->m_output.m_data.m_cpu.template fill<uint8>(m_mmapData->m_p + 16 + dataStep*m_index, dataStep, m_scale);
    this->m_label.m_data.m_cpu.template fill<uint8>(m_mmapLabel->m_p + 8 + 1*m_index, 1);
    m_index++;

    return true;
}

template<typename DType>
bool CLayerInput<DType>::backward()
{
    return true;
}

template<typename DType>
void CLayerInput<DType>::loadDatasetSize()
{
    int tmp;
    GetDataFromFile(this->cfg["param"]["source_data"], 4, 4, (char*)&tmp);
    this->m_datasetSize = MSBLSB_32b(tmp);
    infof("get data set size:%d\n", m_datasetSize);
}

template<typename DType>
void CLayerInput<DType>::loadLaeblsetSize()
{
    int tmp;
    GetDataFromFile(this->cfg["param"]["source_label"], 4, 4, (char*)&tmp);
    this->m_labelsetSize = MSBLSB_32b(tmp);
    infof("get label set size:%d\n", m_labelsetSize);
}

template<typename DType>
CLayerInput<DType>::SMmapFile::SMmapFile(const std::string &file)
{
    int fd = open(file.c_str(), O_RDONLY);
    long length = lseek(fd, 0, SEEK_END);
    infof("file \"%s\" length:%ld\n", file.c_str(), length);

    uint8* p = (uint8*)mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED)
    {
       errorf("mmap \"%s\" failed\n", file.c_str());
       ASSERT(0);
    }

    this->m_p = p;
    this->m_length = length;
    close(fd);
    infof("mmap \"%s\" succeed, p:%p\n", file.c_str(), p);
}

template<typename DType>
CLayerInput<DType>::SMmapFile::~SMmapFile()
{
    infof("munmap %p\n", m_p);
    munmap(m_p, m_length);
}
