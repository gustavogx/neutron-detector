#pragma once
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace CLHEP;

namespace MyAnthracene{

G4Material* ConstructEJ309(const G4String &lightYeildFileName=""){
    
    double density = 0.959*g/cm3;
    double rindex  = 1.57;

    auto scintillator = G4NistManager::Instance()->BuildMaterialWithNewDensity("EJ309","G4_ANTHRACENE",density);

    auto properties = new G4MaterialPropertiesTable();
    scintillator->SetMaterialPropertiesTable(properties);

    // light emission
    std::ifstream lightFile(lightYeildFileName);
    if(lightFile.is_open()){
        std::vector<double> lightYTableEnergy, lightYTableValue;
        double x, y;
        while (!lightFile.eof())
        {
            lightFile >> x >> y;
            lightYTableEnergy.push_back(h_Planck*c_light/(x*nm));
            lightYTableValue.push_back(y);
        }
        properties->AddProperty("FASTCOMPONENT",&(lightYTableEnergy.front()),&(lightYTableValue.front()),lightYTableEnergy.size());
        std::cout << "EJ309 has " << lightYTableEnergy.size() << " values on its light yeild table." << std::endl;
    }else { G4cerr << "Light yeild (EJ309) file not found: " << lightYeildFileName << G4endl; }

    properties->AddConstProperty("SCINTILLATIONYIELD",1/MeV);
    properties->AddConstProperty("FASTTIMECONSTANT",3.5*ns);
    properties->AddConstProperty("RESOLUTIONSCALE",1.0);
    properties->AddConstProperty("YIELDRATIO",1.0);

    // refractive index
    const int rIndexData = 2;
    double rIndexTable[2][rIndexData] = { { 1*eV, 5*eV }, {rindex, 2*rindex } };
    properties->AddProperty("RINDEX", rIndexTable[0], rIndexTable[1], rIndexData);

    return scintillator;
}

}