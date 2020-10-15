#include "MyDetector.h"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "G4VisAttributes.hh"

#include "MySensorDetector.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4MaterialPropertiesTable.hh"

#include "MyAnthracene.h"

using namespace CLHEP;

#define inches 25.4*mm

G4VPhysicalVolume* MyDetector::Construct(){

    auto EJ309 = MyAnthracene::ConstructEJ309("data/EJ309-Scintillation_Emission.dat");

    const int N_DATA = 2;
    double photonEneries[N_DATA] = { 1.65*eV, 3.10*eV };

    auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto airPropertiesTable = new G4MaterialPropertiesTable();
    air->SetMaterialPropertiesTable( airPropertiesTable );

    double airRIndex[N_DATA] = { 1.0, 1.0 };
    airPropertiesTable->AddProperty("RINDEX", photonEneries, airRIndex, N_DATA);

    auto water = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    auto waterPropertiesTable = new G4MaterialPropertiesTable();
    water->SetMaterialPropertiesTable( waterPropertiesTable );

    double waterRIndex[N_DATA] = { 1.33, 1.33 };
    waterPropertiesTable->AddProperty("RINDEX", photonEneries, waterRIndex, N_DATA);

    auto glass = G4NistManager::Instance()->FindOrBuildMaterial("G4_GLASS_PLATE");
    auto glassPropertiesTable = new G4MaterialPropertiesTable();
    glass->SetMaterialPropertiesTable( glassPropertiesTable );

    double glassRIndex[N_DATA] = { 2.50, 2.50 };
    glassPropertiesTable->AddProperty("RINDEX", photonEneries, glassRIndex, N_DATA);

//    auto aluminium = G4NistManager::Instance()->FindOrBuildMaterial("G4_ALUMINIUM");
//    auto aluminiumPropertiesTable = new G4MaterialPropertiesTable();
//    aluminium->SetMaterialPropertiesTable( aluminiumPropertiesTable );


    // World volume
    auto worldBox = new G4Box("worldBox", 0.5*m, 0.5*m, 0.5*m); 
    auto logicalWorld = new G4LogicalVolume(worldBox, air, "LogicalWorld");
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "World", 0, false, 0);
    logicalWorld->SetVisAttributes( new G4VisAttributes( false ) );

    double cavityInnerDiameter = 5.0*inches;
    double cavityInnerLength = cavityInnerDiameter;
    double cavityWallThickness = 0.06*inches;
    auto cavityInnerTube = new G4Tubs("cavityInnerVolume",0.0,cavityInnerDiameter/2.0,cavityInnerLength/2.0,0,2*pi);

    auto scintillatorVolume_log = new G4LogicalVolume(cavityInnerTube, EJ309, "Logical Scintillator Volume");
    auto scintillatorVolume_phy = new G4PVPlacement(0, {0,0,0}, scintillatorVolume_log, "World/Scincillator", logicalWorld, false, 0);
    scintillatorVolume_log->SetVisAttributes( new G4VisAttributes( G4Colour::Yellow() ) );

    return physicalWorld;
}