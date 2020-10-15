#include "XARAPUCA.h"

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
    G4VPhysicalVolume *ConstructDUNEXARAPUCA()
    {

        const int nSiPM = 18; // on each side
        auto pitchSiPM = CONFIG->Value("WLS_DIM_L")/nSiPM*mm;
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

        barPropertiesTable->AddConstProperty("WLSTIMECONSTANT", 0*ns);

        bar->SetMaterialPropertiesTable(barPropertiesTable);

        // Liquid Argon
        auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
        auto lArPropertiesTable = new G4MaterialPropertiesTable();
        lAr->SetMaterialPropertiesTable(lArPropertiesTable);
        CONFIG->FillProperty("LAR_RINDEX", lArPropertiesTable); 
        CONFIG->FillProperty("LAR_ABSLENGTH", lArPropertiesTable); 

        // Silicon
        auto silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
        auto siPropertiesTable = new G4MaterialPropertiesTable();
        silicon->SetMaterialPropertiesTable(siPropertiesTable);
        CONFIG->FillProperty("EJ200_RINDEX", siPropertiesTable);    // avoid surface reflection 
        CONFIG->FillProperty("SI_ABSLENGTH", siPropertiesTable);    // high AbsLength to kill the photon
        
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

        // Triangular-shaped cut
        double cutW = CONFIG->Value("PAR_9")*100*um+1*nm; LOG_TERM_ERROR("Cut {0} mm", (cutW-1*nm)/mm);
        double cutB = CONFIG->Value("SIPM_SIZE")*mm;
        double cutH = dimHeightWLS;
        auto solidTriCut = new G4GenericTrap("prismCut", cutH/2, {
                {0, cutW}, {cutB/2, 0}, {-cutB/2, 0}, {0, cutW},
                {0, cutW}, {cutB/2, 0}, {-cutB/2, 0}, {0, cutW}
        } ); UNUSED(solidTriCut);

        // Circular-shaped cut
        auto solidCircCut = new G4Tubs("circCut",0,cutB/2,cutH,0,pi); UNUSED(solidCircCut);

        // Wedge
        G4ThreeVector p[9];
        p[1] = {-cutB/2,-cutH/2,0};
        p[2] = { cutB/2,-cutH/2,0};
        p[3] = {0,-cutH/2,cutW};
        p[4] = {-cutB/2,cutH/2,0};
        p[5] = { cutB/2,cutH/2,0};
        p[6] = {0,cutH/2,cutW};
        p[7] = {0,-cutH/2,0};
        p[8] = {0,cutH/2,0};
        G4Tet *tetra[6] = {
            new G4Tet("tet0",p[8],p[5],p[6],p[3]),
            new G4Tet("tet1",p[8],p[5],p[2],p[3]),
            new G4Tet("tet2",p[8],p[7],p[2],p[3]),
            new G4Tet("tet3",p[8],p[4],p[6],p[3]),
            new G4Tet("tet4",p[8],p[4],p[1],p[3]),
            new G4Tet("tet5",p[8],p[7],p[1],p[3])
        };

        G4VSolid *solidPtr = tetra[0], *unionPtr;
        for(int i=1; i<6; i++) {
            unionPtr = new G4UnionSolid("Wedge",solidPtr,tetra[i]);
            solidPtr = unionPtr;
        }

        auto cutRotNeg = new G4RotationMatrix();
        cutRotNeg->rotateX(-90*deg);
        auto cutRotPos = new G4RotationMatrix();
        cutRotPos->rotateX(90*deg);

        G4VSolid *solidWLS, *solidCut = solidCircCut;
        G4VSolid *cutSubtractions[2*nSiPM], *cutPointer;
        cutPointer = boxWLS;
        if(CONFIG->Value("PAR_9")>=1*nm){
            for(int i=0; i<2*nSiPM; i++){

                if(i<nSiPM) cutSubtractions[i] = new G4SubtractionSolid("barWithCut", cutPointer, solidCut, cutRotPos, {xSiPM[i], 0, dimWidthWLS/2+1*um});
                else        cutSubtractions[i] = new G4SubtractionSolid("barWithCut", cutPointer, solidCut, cutRotNeg, {xSiPM[i-18], 0, -dimWidthWLS/2-1*um});
                cutPointer = cutSubtractions[i];
            }
            solidWLS = cutPointer;
        } else solidWLS = boxWLS;


        auto logWLS = new G4LogicalVolume(solidWLS, bar, "Logical WLS");
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
              
        // make the cut visible      
        //auto logCut = new G4LogicalVolume(solidPtr, bar, "Logical Cut");
        //auto logCut = new G4LogicalVolume(solidCircCut, bar, "Logical Cut");
        //auto logCut = new G4LogicalVolume(solidCut, bar, "Logical Cut");
        //auto phyCut = new G4PVPlacement(0, {0,0,0}, logCut, "WLS/Cut", logWorld, false, 0); UNUSED(phyCut);
        //logCut->SetVisAttributes(new G4VisAttributes(G4Color::Yellow()));

        auto sipmSize = CONFIG->Value("SIPM_SIZE")*mm;
        auto sipmThickness = 0.1*mm;
        auto sipmDistance = CONFIG->Value("PAR_8")*100*um+1*nm; LOG_TERM_ERROR("Distance {0} mm",(sipmDistance-1*nm)/mm);
        auto boxSiPM = new G4Box("SiPM", sipmSize*mm / 2, sipmSize*mm / 2, sipmThickness/2);
        auto logSiPM = new G4LogicalVolume(boxSiPM, silicon, "logSiPM");
        logSiPM->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));

        G4PVPlacement *phySiPM[2*nSiPM];
        std::string volSiPMName;
        for(int i=0; i<2*nSiPM; i++){
            volSiPMName = "World/SiPM_" + std::to_string(i);
            if(i<nSiPM) phySiPM[i] = new G4PVPlacement(0, { xSiPM[i] , 0, (dimWidthWLS+sipmThickness)/2+sipmDistance }, logSiPM, volSiPMName, logWorld, true, i); 
            else        phySiPM[i] = new G4PVPlacement(0, { xSiPM[i-18] , 0,-(dimWidthWLS+sipmThickness)/2-sipmDistance }, logSiPM, volSiPMName, logWorld, true, i);
        }
        UNUSED(phySiPM);


        // === Reflectors (perfect) ==================================
        auto reflecColor = new G4Color(0.8,0.8,1.0);

        double dist = 1*nm;
        // Bottom
        double reflectThickness = 65*um;
        auto reflectorMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
        auto boxReflector = new G4Box("Box Reflector", dimLengthWLS/2+reflectThickness, reflectThickness/2, dimWidthWLS/2+reflectThickness);
        auto logReflector = new G4LogicalVolume(boxReflector, reflectorMaterial, "Logical Reflector");
        logReflector->SetVisAttributes(new G4VisAttributes(reflecColor));

        auto phyReflector = new G4PVPlacement(0, {0, -((dimHeightWLS+reflectThickness)/2+dist), 0}, logReflector, "World/Reflector", logWorld, false, 0);

        // Laterals
        auto boxLateralReflector = new G4Box("Box Lateral Reflector", reflectThickness/2, dimHeightWLS/2, dimWidthWLS/2);
        auto logLateralReflector = new G4LogicalVolume(boxLateralReflector, reflectorMaterial, "Logical Lateral Reflector");
        logLateralReflector->SetVisAttributes(new G4VisAttributes(reflecColor));

        auto phyLateralReflector1 = new G4PVPlacement(0, {-(dimLengthWLS+reflectThickness)/2-dist, 0, 0}, logLateralReflector, "World/LateralReflector1", logWorld, true, 0); 
        auto phyLateralReflector2 = new G4PVPlacement(0, { (dimLengthWLS+reflectThickness)/2+dist, 0, 0}, logLateralReflector, "World/LateralReflector2", logWorld, true, 1);

        // SiPM Masks
        auto boxSiPMReflector = new G4Box("Box SiPM Reflector", dimLengthWLS/2, dimHeightWLS/2, reflectThickness/2);
        auto boxMaskHole = new G4Box("MaskHole", sipmSize*mm / 2, sipmSize*mm / 2, reflectThickness);

        G4VSolid *arrayMaskHoles[18], *boxPointer;

        boxPointer = boxSiPMReflector;
        for(int i=0; i<18; i++){
            arrayMaskHoles[i] = new G4SubtractionSolid("SiPM Reflector Mask",boxPointer,boxMaskHole,0,{ xSiPM[i], 0, 0});
            boxPointer = arrayMaskHoles[i];
        }

        auto boxSiPMMaskReflector = arrayMaskHoles[17];
        auto logSiPMMaskReflector = new G4LogicalVolume(boxSiPMMaskReflector, reflectorMaterial, "Logical Mask Reflector");
        logSiPMMaskReflector->SetVisAttributes(new G4VisAttributes(reflecColor));

        auto phySiPMMaskReflector1 = new G4PVPlacement(0, {0, 0, (dimWidthWLS+reflectThickness)/2+dist}, logSiPMMaskReflector, "World/MaskReflector1", logWorld, true, 0); UNUSED(phySiPMMaskReflector1);
        auto phySiPMMaskReflector2 = new G4PVPlacement(0, {0, 0, -(dimWidthWLS+reflectThickness)/2-dist}, logSiPMMaskReflector, "World/MaskReflector2", logWorld, true, 1); UNUSED(phySiPMMaskReflector2);

        // Optical Surfaces for lAr/Reflector
        auto surfacelArReflector = new G4OpticalSurface("lAr/Reflector Surface");
        surfacelArReflector->SetModel(unified);
        surfacelArReflector->SetType(dielectric_metal);
        surfacelArReflector->SetFinish(polished);

        auto surfacelArReflectorProp = new G4MaterialPropertiesTable();
        surfacelArReflector->SetMaterialPropertiesTable(surfacelArReflectorProp);
        CONFIG->FillProperty("VIKUITI_REFLEC", surfacelArReflectorProp);

        // One-way interfaces
        auto logSurfacelArReflector = new G4LogicalBorderSurface("lAr->Reflector Interface", phyWorld, phyReflector, surfacelArReflector);  UNUSED(logSurfacelArReflector); 
        auto logSurfacelArLateralReflector1 = new G4LogicalBorderSurface("lAr->Lateral Reflector 1 Interface", phyWorld, phyLateralReflector1, surfacelArReflector); UNUSED(logSurfacelArLateralReflector1);
        auto logSurfacelArLateralReflector2 = new G4LogicalBorderSurface("lAr->Lateral Reflector 2 Interface", phyWorld, phyLateralReflector2, surfacelArReflector); UNUSED(logSurfacelArLateralReflector2);
        auto logSurfacelArMaskReflector1 = new G4LogicalBorderSurface("lAr->Mask Reflector 1 Interface", phyWorld, phySiPMMaskReflector1, surfacelArReflector); UNUSED(logSurfacelArMaskReflector1);
        auto logSurfacelArMaskReflector2 = new G4LogicalBorderSurface("lAr->Mask Reflector 2 Interface", phyWorld, phySiPMMaskReflector2, surfacelArReflector); UNUSED(logSurfacelArMaskReflector2);

        // === Dichroic Filters (fast simulation) ==================================        
        double filterThickness = CONFIG->Value("FILTER_THICKNESS")*mm;
        double filterDistance  = 1.5*mm;
        auto filterMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr"); 
        auto boxFilter = new G4Box("Box Filter", dimLengthWLS/2, filterThickness/2, dimWidthWLS/2);
        auto logFilter = new G4LogicalVolume(boxFilter, filterMaterial, "Logical Filter");

        auto filterEnvelope = new G4Region("DichroicFilterRegion");
        filterEnvelope->AddRootLogicalVolume(logFilter);

        auto visFilter = new G4VisAttributes();
        visFilter->SetColor(0.5,0,0.5,0.5);
        logFilter->SetVisAttributes(visFilter);
        auto phyFilter = new G4PVPlacement(0, {0, (dimHeightWLS+filterThickness)/2+filterDistance, 0}, logFilter, "World/Filter", logWorld, false, 0);
        UNUSED(phyFilter);

        // Optical Surfaces for lAr/Reflector
        //auto surfacelArFilter = new G4OpticalSurface("lAr/Reflector Surface");
        //surfacelArFilter->SetModel(unified);
        //surfacelArFilter->SetType(dielectric_metal);
        //surfacelArFilter->SetFinish(polished);

        //auto logSurfacelArFilter = new G4LogicalBorderSurface("lAr->Filter Interface", phyWorld, phyFilter, surfacelArReflector); UNUSED(logSurfacelArFilter);
        //auto logSurfaceFilterlAr = new G4LogicalBorderSurface("Filter->lAr Interface", phyFilter, phyWorld, surfacelArReflector); UNUSED(logSurfaceFilterlAr);

        return phyWorld;
    }    
}