#pragma once

// The Detector
#include "G4VUserDetectorConstruction.hh"
#include "G4TessellatedSolid.hh"

namespace ARAPUCA
{
    G4TessellatedSolid* LoadTessellatedSolid(const G4String &fileName);
    
    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    private:
        G4VPhysicalVolume *m_World;

    private:

        G4VPhysicalVolume *ConstructXARAPUCA();
        G4VPhysicalVolume *ConstructXARAPUCA2();
        G4VPhysicalVolume *ConstructTestBox();

    public:

        DetectorConstruction(const std::string &geometry="");

        virtual G4VPhysicalVolume *Construct() override;
        virtual void ConstructSDandField() override;
        
    };
    
} // namespace ARAPUCA
