#include "../Core.h"
#include "../Globals.h"

#include "G4VisAttributes.hh"
#include "MaterialProperties.h"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

namespace ARAPUCA{

G4Color GDML::StrToColor(const std::string &key){

    G4Color color;

    if(G4Color::GetColour(key,color)) return color;
    return G4Color::White();

}

GDML* GDML::s_Instance = nullptr;
bool GDML::s_InUse = false;

GDML* GDML::Parser(){

    if(s_Instance==nullptr) s_Instance = new GDML();
    return s_Instance;
    
}

bool GDML::File(const std::string &fileName, const bool &validate) {

    if( !SourceExists(fileName) )  { 
        LOG_TERM_FATAL("Geometry file <{0}> was not found.", fileName);
        return false;
    } else {
        SetOverlapCheck(true);
        Read(fileName, validate);   
        ReadAuxInfo();
        SetMaterialProperties();
        s_InUse = true;
    }
    return true;

}

void GDML::SetMaterialProperties(G4VPhysicalVolume* physVol){

    if(!physVol) physVol = GetWorldVolume();
    auto prop = MaterialProperties::Access();
    auto vol = physVol->GetLogicalVolume(); 
    
    if(vol->GetMaterial()->GetMaterialPropertiesTable()==nullptr){ 
        auto table = new G4MaterialPropertiesTable();
        auto materialName = vol->GetMaterial()->GetName();
        LOG_TERM_ERROR("{0} {1}",materialName, prop->Map().count(materialName)) ;
        if(prop->Map().count(materialName)) {
            for(auto &property : MaterialProperties::Access()->Map()[materialName] ){
            table->AddProperty( property.first.c_str(), property.second );
            LOG_TERM_ERROR("GDML :: Property {1} added to material {0} with {2} data points.", materialName, property.first, table->GetProperty(property.first.c_str())->GetVectorLength());
            }
            vol->GetMaterial()->SetMaterialPropertiesTable(table);
        } else LOG_TERM_ERROR("No property found for material {0}.",materialName);
    }
    for(uint64_t i=0; i<vol->GetNoDaughters(); i++) SetMaterialProperties(vol->GetDaughter(i));
}

void GDML::ReadAuxInfo(G4VPhysicalVolume* physVol){

    if(!physVol) physVol = GetWorldVolume();
    auto vol = physVol->GetLogicalVolume();
    for(auto &aux : GetVolumeAuxiliaryInformation(vol)) {
        if( KeyIsColor(aux.type) ) vol->SetVisAttributes( new G4VisAttributes( StrToColor(aux.value) ));
        if( KeyIsSensDet(aux.type) ) SDList()->Add( "SD"+vol->GetName(), vol );
    }
    for(uint64_t i=0; i<vol->GetNoDaughters(); i++) ReadAuxInfo(vol->GetDaughter(i));
}

void GDML::Print(G4VPhysicalVolume* physVol){

    if(!physVol) physVol = GetWorldVolume();
    auto vol = physVol->GetLogicalVolume();
    LOG_TERM_ERROR("GDML :: Volume <{0}> , ID {1}, Daughters {2}, Aux {3}",vol->GetName(), vol->GetInstanceID(), vol->GetNoDaughters(), GetVolumeAuxiliaryInformation(vol).size() );    

    for(auto &aux : GetVolumeAuxiliaryInformation(vol)) {
        LOG_TERM_ERROR("\t\tAuxiliary Info {0} value {1} unit {2}", aux.type, aux.value, aux.unit);
    }
    for(uint64_t i=0; i<vol->GetNoDaughters(); i++) Print(vol->GetDaughter(i));

}

}