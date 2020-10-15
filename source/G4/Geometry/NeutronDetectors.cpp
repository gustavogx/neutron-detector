#include "NeutronDetectors.h"

#include "../Core.h"
#include "../Globals.h"

// Materials
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"

// Geometry
#include "G4Box.hh"
#include "G4Tet.hh"
#include "G4GenericTrap.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

// Volumes
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// Optics
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

// Visualization
#include "G4VisAttributes.hh"

namespace ARAPUCA{

G4VPhysicalVolume* ConstructNeutronDetectors(){
    //auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");

    // Energia dos nêutrons
    const int N_DATA=2;
    double neutronEnergies[N_DATA] = {661.7*keV, 661.7*MeV};

    // Properties of materials

    // Air
    auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto airPropertiesTable = new G4MaterialPropertiesTable();
    air -> SetMaterialPropertiesTable( airPropertiesTable);
    double airRIndex[N_DATA] = {1.0, 1.0};
    airPropertiesTable->AddProperty("RINDEX",neutronEnergies, airRIndex, N_DATA);

    //Xylene
    auto Xylol = G4NistManager::Instance()->FindOrBuildMaterial("G4_XYLENE");
    auto XylolPropertiesTable = new G4MaterialPropertiesTable();
    Xylol -> SetMaterialPropertiesTable( XylolPropertiesTable);
    double XylolRIndex[N_DATA] = {1.58, 1.58};
    XylolPropertiesTable->AddProperty("RINDEX",neutronEnergies, XylolRIndex, N_DATA);

    double XylolAbsLength[N_DATA] = {29.12*cm, 29.12*cm};
    XylolPropertiesTable->AddProperty("ABSLENGTH",neutronEnergies, XylolAbsLength, N_DATA);

    // SiPM
    auto silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
    auto siliconPropertiesTable = new G4MaterialPropertiesTable();
    silicon -> SetMaterialPropertiesTable( siliconPropertiesTable );

    double siliconRIndex[N_DATA] = {1, 1};
    siliconPropertiesTable->AddProperty("RINDEX", neutronEnergies, siliconRIndex, N_DATA);

    double siliconAbslength[N_DATA] = {1*nm, 1*nm};
    siliconPropertiesTable->AddProperty("ABSLENGTH", neutronEnergies, siliconAbslength, N_DATA );

    // Stainless-Steel
    auto ssteel = G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    auto ssteelPropertiesTable = new G4MaterialPropertiesTable();
    ssteel -> SetMaterialPropertiesTable( ssteelPropertiesTable);
    //double ssteelRIndex[N_DATA] = {2.7, 3.7};
    // ssteelPropertiesTable->AddProperty("RINDEX",neutronEnergies, ssteelRIndex, N_DATA);


    // World Volume
    auto worldBox = new G4Box("worldBox", 1*m, 1*m, 1*m);
    auto logicalWorld = new G4LogicalVolume(worldBox, air, "LogicalWorld");
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "World", 0, false, 0);
    logicalWorld->SetVisAttributes(new G4VisAttributes( false ) );

    // Detector Volume
    double raio = 65*mm, length = 120*mm, skin=3.0*mm;
    auto detectorExterior = new G4Tubs("detectorBoxE", 0,raio, length/2, 0, twopi); 
    auto detectorInterior = new G4Tubs("detectorBoxI", 0,raio-skin, length/2-skin,0, twopi); 

    auto logDetectorWall =  new G4LogicalVolume(detectorExterior, ssteel, "logDetectorWall");
    logDetectorWall->SetVisAttributes(new G4VisAttributes( G4Color::Gray() ) );

    // Liquid Volume
    auto logDetectorVolume = new G4LogicalVolume(detectorInterior, Xylol, "logDetectorVolume");
    logDetectorVolume->SetVisAttributes(new G4VisAttributes( G4Color::Magenta() ) );
    auto physDetectorVolume = new G4PVPlacement(0, {0,0,0}, logDetectorVolume, "DetectorVolume", logDetectorWall, false, 0);


    // Sensor Volume
    double sensorThickness = 0.1*mm;
    auto sensorTube = new G4Tubs("sensorTube",0,raio-skin, sensorThickness/2, 0, twopi);
    auto logSensor = new G4LogicalVolume(sensorTube, silicon, "logSensor");
    auto physSensor = new G4PVPlacement(0, {0,0,length/2-skin-sensorThickness/2}, logSensor, "Sensor", logDetectorVolume, false, 0);
    logSensor->SetVisAttributes( new G4VisAttributes( G4Color::Green ));



    G4ThreeVector positions[3] = {{100*mm,-100*mm,0}, {100*mm,100*mm,0}, {-100*mm,-100*mm,0}};
    G4PVPlacement *physDetectorWall[3];

    for(int i=0; i<3; i++) physDetectorWall[i] = new G4PVPlacement(0, positions[i], logDetectorWall, "DetectorWall", logicalWorld, true, 3);







    // Steel Xyleno Optical Properties

    auto surfacessteelXylol = new G4OpticalSurface("ssteel/xylol surface");
    surfacessteelXylol->SetModel(unified);
    surfacessteelXylol->SetType(dielectric_metal);
    surfacessteelXylol->SetFinish(ground);
    surfacessteelXylol->SetSigmaAlpha(1.0*degree);

    auto surfacessteelXylolProp = new G4MaterialPropertiesTable();
    surfacessteelXylol->SetMaterialPropertiesTable(surfacessteelXylolProp);


    for(int i=0; i<3; i++) {
        new G4LogicalBorderSurface("ssteel->Xylol Interface",physDetectorWall[i],physDetectorVolume,surfacessteelXylol);
        new G4LogicalBorderSurface("Xylol->ssteel  Interface",physDetectorVolume,physDetectorWall[i],surfacessteelXylol);
    }


    double reflectssteelXylol[N_DATA] = {0.7, 0.7};
    surfacessteelXylolProp->AddProperty("REFLECTIVY", neutronEnergies, reflectssteelXylol, N_DATA);

        // Xyleno PMT Optical Properties

    auto surfaceXylolSiPM = new G4OpticalSurface("xylol/SiPM surface");
    surfaceXylolSiPM->SetModel(unified);
    surfaceXylolSiPM->SetType(dielectric_dielectric);
    surfaceXylolSiPM->SetFinish(polished);

    auto surfaceXylolSiPMProp = new G4MaterialPropertiesTable();
    surfaceXylolSiPM->SetMaterialPropertiesTable(surfaceXylolSiPMProp);

    auto logsurfaceXylolSiPM = new G4LogicalBorderSurface("Xylol->SiPM Interface",physDetectorVolume,physSensor,surfaceXylolSiPM);
    auto logSurfaceXylolSiPM = new G4LogicalBorderSurface("SiPM -> Xylol Interface", physSensor, physDetectorVolume,surfaceXylolSiPM);


    double reflectXylolSiPM[N_DATA] = {0.7, 0.7};
    surfaceXylolSiPMProp->AddProperty("REFLECTIVY", neutronEnergies, reflectXylolSiPM, N_DATA);
    
    return physicalWorld;
}
// preocuparar do xyleno para o aço dielétrico metal round.
// do xyleno para pmt polido Dieletrico-DIELÉTRICO.

}