#include "AbsorptionTest.h"

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

    // =====================================================================
    //      X-ARAPUCA DUNE Geometry
    // =====================================================================
    G4VPhysicalVolume *ConstructAbsTest()
    {

        const int nSiPM = 100; // on each side
        auto pitchSiPM = (5.0/nSiPM)*mm;
        double xSiPM[nSiPM];
        for(int i=0; i<nSiPM; i++) xSiPM[i] = (0.5+i)*pitchSiPM - CONFIG->Value("WLS_DIM_L")*mm/2;

        // === Materials =======================================================

        // Polystyrene (C8H8)n
        auto elC = G4NistManager::Instance()->FindOrBuildElement("C");
        auto elH = G4NistManager::Instance()->FindOrBuildElement("H");

        auto bar = new G4Material("BARWLS", CONFIG->Value("BARWLS_DENSITY")*g/cm3, 2);
        bar->AddElement(elC,8);
        bar->AddElement(elH,8);
        
        auto barPropertiesTable = new G4MaterialPropertiesTable();
        
        CONFIG->FillProperty("BARWLS_RINDEX", barPropertiesTable, eV);         
        CONFIG->FillProperty("BARWLS_ABSLENGTH", barPropertiesTable, eV, m);
        CONFIG->FillProperty("BARWLS_EMISSION", barPropertiesTable, eV);
        CONFIG->FillProperty("BARWLS_ABSORPTION", barPropertiesTable, eV);        
        
        barPropertiesTable->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
        bar->SetMaterialPropertiesTable(barPropertiesTable);

        // Liquid Argon
        auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
        auto lArPropertiesTable = new G4MaterialPropertiesTable();
        lAr->SetMaterialPropertiesTable(lArPropertiesTable);
        CONFIG->FillProperty("LAR_RINDEX", lArPropertiesTable); 
        CONFIG->FillProperty("LAR_ABSLENGTH", lArPropertiesTable); 

        // End of Materials =======================================================

        // Geometry =======================================================
        // World volume
        auto worldBox = new G4Box("worldBox", 1 * m, 1 * m, 1 * m);
        auto logWorld = new G4LogicalVolume(worldBox, lAr, "LogicalWorld");
        auto phyWorld = new G4PVPlacement(0, {0, 0, 0}, logWorld, "World", 0, false, 0);
        logWorld->SetVisAttributes(new G4VisAttributes(false));

        // WLS Bar
        double dimLengthWLS = CONFIG->Value("WLS_DIM_L") * mm;
        double dimWidthWLS = CONFIG->Value("WLS_DIM_W") * mm;
        double dimHeightWLS = CONFIG->Value("WLS_DIM_H") * mm;
        auto boxWLS = new G4Box("Box WLS", dimLengthWLS/2, dimHeightWLS/2, dimWidthWLS/2);

        auto logWLS = new G4LogicalVolume(boxWLS, bar, "Logical WLS");
        auto phyWLS = new G4PVPlacement(0, {0, 0, 0}, logWLS, "World/WLS", logWorld, false, 0);

        auto visWLS = new G4VisAttributes();
        visWLS->SetColor(0.6,1.0,1.0);
        logWLS->SetVisAttributes(visWLS);

        // Optical Surfaces for lAr/WLS
        auto surfacelArWLS = new G4OpticalSurface("lAr/WLS Surface");
        surfacelArWLS->SetModel(unified);
        surfacelArWLS->SetType(dielectric_dielectric);
        surfacelArWLS->SetFinish(polished);

        auto surfacelArWLSProp = new G4MaterialPropertiesTable();
        surfacelArWLS->SetMaterialPropertiesTable(surfacelArWLSProp);
        CONFIG->FillProperty("LAR_WLS_REFLEC", surfacelArWLSProp);

        // Two-way interface
        auto logSurfacelArWLS = new G4LogicalBorderSurface("lAr->WLS Interface", phyWorld, phyWLS, surfacelArWLS); UNUSED(logSurfacelArWLS);
        auto logSurfaceWLSlAr = new G4LogicalBorderSurface("WLS->lAr Interface", phyWLS, phyWorld, surfacelArWLS); UNUSED(logSurfaceWLSlAr);

        auto sipmSize = 4*mm*hc;
        auto sipmThickness = 1*nm;
        auto boxSiPM = new G4Box("SiPM", sipmSize*mm / 2, sipmSize*mm / 2, sipmThickness/2);
        auto logSiPM = new G4LogicalVolume(boxSiPM, bar, "logSiPM");
        logSiPM->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));

        auto sipmRotPos = new G4RotationMatrix();
        sipmRotPos->rotateY(90*deg);

        G4PVPlacement *phySiPM[nSiPM];
        std::string volSiPMName;
        for(int i=0; i<nSiPM; i++){
            volSiPMName = "World/SiPM_" + std::to_string(i);
            phySiPM[i] = new G4PVPlacement(sipmRotPos, { xSiPM[i] , 0, 0 }, logSiPM, volSiPMName, logWLS, true, i); 
        }
        UNUSED(phySiPM);

        return phyWorld;
    }    
}