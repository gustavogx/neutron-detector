#pragma once
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Color.hh"

namespace ARAPUCA{

class GDML : public G4GDMLParser {
private:

    static GDML* s_Instance;
    G4VPhysicalVolume* m_World;
    std::map<std::string,G4LogicalVolume*> m_SDMap;
    static bool s_InUse;

    void ReadAuxInfo(G4VPhysicalVolume* physVol=nullptr);
    void SetMaterialProperties(G4VPhysicalVolume* physVol=nullptr);
    
public:
  
    GDML() = default;
    GDML(const GDML&) = delete;
    void operator=(const GDML&) = delete;

    static GDML* Parser();
    static inline bool InUse() { return s_InUse; } 
    bool File(const std::string &fileName, const bool &validate=true);
    void Print(G4VPhysicalVolume* physVol=nullptr);


public:

    static G4Color StrToColor(const std::string &key);

};

}