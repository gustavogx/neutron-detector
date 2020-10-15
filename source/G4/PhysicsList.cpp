#include "Core.h"
#include "PhysicsList.h"

#include <iomanip>   

#include "globals.hh"
#include "G4ios.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"

#include "G4OpticalPhysics.hh"
#include "G4FastSimulationPhysics.hh"

namespace ARAPUCA{

PhysicsList::PhysicsList(u_int8_t verbose)
{

    defaultCutValue = 0.7*CLHEP::mm;  
    SetVerboseLevel(verbose);

    // Optical Physics
    auto opticalPhysics = new G4OpticalPhysics();
    
    opticalPhysics->SetWLSVerbosity(verbose);
    opticalPhysics->SetWLSTimeProfile("exponential");
    
    opticalPhysics->SetTrackSecondariesFirst(kScintillation,true);
    opticalPhysics->SetTrackSecondariesFirst(kCerenkov,true);
    RegisterPhysics( opticalPhysics );

    auto fastSimulationPhysics = new G4FastSimulationPhysics();
    fastSimulationPhysics->ActivateFastSimulation("opticalphoton"); // this is for the dichroic filter paremetrization
    RegisterPhysics ( fastSimulationPhysics );

    // EM Physics
    RegisterPhysics( new G4EmStandardPhysics(verbose));

    // Decays 
    RegisterPhysics( new G4DecayPhysics(verbose) );

/*
    // Synchroton Radiation & GN Physics
    RegisterPhysics( new G4EmExtraPhysics(verbose) );


    // Hadron Elastic scattering
    RegisterPhysics( new G4HadronElasticPhysics(verbose) );

    // Hadron Physics
    RegisterPhysics(  new G4HadronPhysicsFTFP_BERT(verbose));

    // Stopping Physics
    RegisterPhysics( new G4StoppingPhysics(verbose) );

    // Ion Physics
    RegisterPhysics( new G4IonPhysics(verbose));

    // Neutron tracking cut
    RegisterPhysics( new G4NeutronTrackingCut(verbose));
    */
}

}
