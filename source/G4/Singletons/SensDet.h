#pragma once
#include "../Core.h"
#include "G4LogicalVolume.hh"
#include <sstream>
#include <utility>

namespace ARAPUCA{

struct SDElement{
    G4String                Name;
    G4LogicalVolume*        Volume;
    uint64_t                Score;

    SDElement() :  Name(), Volume(nullptr), Score(0) {};
    SDElement(const G4String &name, G4LogicalVolume *vol) :  Name(name), Volume(vol), Score(0) {};

};

typedef std::map<G4String, ARAPUCA::SDElement> SDMap;

class SensDet {
private:
    static SensDet* m_Instance;
    SDMap m_SDMap;
    
public:
    static SensDet* Instance();
    SensDet() = default;
    SensDet(SensDet const &) = delete;
    void operator=(SensDet const &) = delete;

    void Add(const G4String &name, G4LogicalVolume *vol) { m_SDMap[name] = SDElement(name,vol); }
    
    SDElement& Get(const G4String &name) { return m_SDMap[name]; }

    void Reset(const G4String &name) { m_SDMap[name].Score=0; }
    void Reset() { for(auto aSD :  m_SDMap) aSD.second.Score = 0; }

    void Hit(const G4String &name) { m_SDMap[name].Score++; }

    u_int64_t GetHits(const G4String &name) { return m_SDMap[name].Score; }

    SDMap &Map() { return m_SDMap; }
    SDMap  Map() const { return m_SDMap; }

    std::string GetScoresLine(){
        std::stringstream s;
        for(auto aSD :  m_SDMap) s << aSD.second.Score << "\t";
        return s.str();
    }

};

#define SDList SensDet::Instance

}