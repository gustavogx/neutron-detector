#include "DetectorConstruction.h"
#include "Core.h"
#include "Globals.h"

#include "G4Box.hh"
#include "G4GenericTrap.hh"
#include "G4Tubs.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4VFacet.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4VisAttributes.hh"

#include "GeneralCounterSD.h"

#include "G4MaterialPropertiesTable.hh"

#include "G4SDManager.hh"

#include "G4UserLimits.hh"

#include "G4RegionStore.hh"

#include <sstream>
#include <stdlib.h>

#include "Geometry/XARAPUCA.h"
#include "Geometry/AbsorptionTest.h"
#include "Geometry/NeutronDetectors.h"
#include "FastModels/DichroicFilterModel.h"

using namespace CLHEP;

namespace ARAPUCA
{

    DetectorConstruction::DetectorConstruction(const std::string &geometry) {
        
        UNUSED(geometry);
        //if(CONFIG->String("ARAPUCA_MODEL") == "ABSTEST") m_World = ARAPUCA::ConstructAbsTest();
        //else m_World = ARAPUCA::ConstructDUNEXARAPUCA();
        m_World = ARAPUCA::ConstructNeutronDetectors();
        
    }

    G4VPhysicalVolume *DetectorConstruction::Construct()
    {
        return m_World;
    }



    void DetectorConstruction::ConstructSDandField() {


        auto sdManager = G4SDManager::GetSDMpointer();
        auto counter = new GeneralCounterSD("PMT","PMTHC");
        sdManager->AddNewDetector(counter);
        SetSensitiveDetector("logSensor",counter,false);

    }

} // namespace ARAPUCA