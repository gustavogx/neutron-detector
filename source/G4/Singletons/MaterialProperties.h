#pragma once
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4MaterialPropertiesIndex.hh"

namespace ARAPUCA{
class MaterialProperties{

private:
    static MaterialProperties* m_Instance;
    std::map<std::string, std::map<std::string, G4MaterialPropertyVector*>> m_Map; // m_Map[material][property]

public:

    MaterialProperties() = default;
    MaterialProperties(const MaterialProperties &) = delete;
    void operator=(const MaterialProperties &) = delete;

    static inline MaterialProperties* Access() { if(m_Instance==nullptr) m_Instance = new MaterialProperties(); return m_Instance; }

    static G4MaterialPropertyVector* Load(const std::string &fileName, const double &unit=1, const bool &isLambda=false);
    
    inline std::map<std::string, std::map<std::string, G4MaterialPropertyVector*>>& Map() { return m_Map; }
    inline std::map<std::string, std::map<std::string, G4MaterialPropertyVector*>> Map() const { return m_Map; }

    static G4MaterialPropertyIndex GetPropertyIndex(const std::string &tag);
    static std::string GetPropertyName(const std::string &tag);
    static std::string GetMaterialName(const std::string &tag);

    void Fill();

};

}