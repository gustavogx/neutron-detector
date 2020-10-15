/* ========================================================================

    ARAPUCA Geant4 Simulation

    Gustavo Valdiviesso 2020

   ======================================================================== */

#include <iostream>
#include "G4/Core.h"
#include "G4/Globals.h"
#include "G4/Singletons/MaterialProperties.h"

// The Detector
#include "G4/DetectorConstruction.h"

// The Action Initialization (particle source)
#include "G4/ActionInitialization.h"

// The Run Action
#include "G4/RunAction.h"

// The Run Action
#include "G4/EventAction.h"

// The Physics List
#include "G4/PhysicsList.h"

// The Manager
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"

// This is the User Interface
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

// Random Numeber Generator
#include "Randomize.hh"

#ifndef G4VIS_USE_OPENGLX
    #define G4VIS_USE_OPENGLX
#endif

#ifndef G4VIS_USE_OPENGLQT
    #define G4VIS_USE_OPENGLQT
#endif

// This is the Visualization Engine
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

// Optical Physics
#include "G4OpticalPhysics.hh"

#include "G4/Commands.h"

// This is the Main code.
int main(int argc, char** argv){

    auto param = ARAPUCA::CommandLineParameters::Access();
    param->Parse(argc,argv);
    auto batchMode = param->Has("-b");
    auto interactiveMode = ! ( param->Has("-i") || batchMode );

    std::string inputFile;
    param->GetParameter("-i",inputFile);
    param->GetParameter("-b",inputFile);
    
    int64_t verboseLevel = ARAPUCA::kAll;
    param->GetParameter("-v",verboseLevel);
    ARAPUCA::Config::Access()->SetValue("VERBOSITY_LEVEL",verboseLevel); 
    ARAPUCA::Logger::Init(verboseLevel);

    G4String useGDML = "";
    if(param->Has("-g"))    useGDML = param->GetParameter("-g");
    if(param->Has("-gdml")) useGDML = param->GetParameter("-gdml");
    if(useGDML.size()) LOG_TERM_TRACE("GDML\t{0}",useGDML);

    std::string configFile("../data/arapuca.cfg");

    CONFIG->SetConfigFile(configFile);
    if(CONFIG->Has("VERBOSITY_LEVEL")) CONFIG->SetValue("VERBOSITY_LEVEL", std::min((int)verboseLevel, (int)CONFIG->Value("VERBOSITY_LEVEL")));
    ARAPUCA::Logger::SetVerbose(CONFIG->Value("VERBOSITY_LEVEL"));
    
    ARAPUCA::MaterialProperties::Access()->Fill();
    CONFIG->BuildCDF("pTP_EMISSION","pTP_SPECTRUM");

    int64_t globalSeed = 1<<10;
    if(CONFIG->Has("RND_SEED") ) globalSeed = CONFIG->Value("RND_SEED");
    param->GetParameter("-s",globalSeed);
    CONFIG->SetValue("RND_SEED",globalSeed);
    CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine() );
//    CLHEP::HepRandom::getTheEngine()->setSeed(globalSeed,INT32_MAX);

    std::string outputFile("output.dat");       //  default
    if(CONFIG->Has("OUTPUT_FILE") ) outputFile = CONFIG->String("OUTPUT_FILE"); //  look for value in config file
    param->GetParameter("-o", outputFile);      //  overwrite it IF -o param is present
    CONFIG->SetString("OUTPUT_FILE",outputFile);
    ARAPUCA::Logger::NewLog( outputFile, !param->Has("-a") );

    G4String genericFileName = "OUTPUT", genericFileTag = "-o", genericOvewriteTag = "-a";
    const int gFileN = 10;
    std::string gFileName;
    for(int i=0; i<gFileN; i++){
        gFileName="";
        param->GetParameter(genericFileTag+std::to_string(i), gFileName);
        CONFIG->SetString(genericFileName+std::to_string(i), gFileName);
        if(param->Has(genericFileTag+std::to_string(i))) {
            ARAPUCA::Logger::NewLog( gFileName, !param->Has(genericOvewriteTag+std::to_string(i)) );
            LOG_TERM_ERROR("Output {0} set with name {1} and overwrite flag {2}",
            i, CONFIG->String(genericFileName+std::to_string(i)), (bool) param->Has(genericOvewriteTag+std::to_string(i)));
        }
    }

    std::string meshFile = "";
    param->GetParameter("-mesh",meshFile);
    if( meshFile!="" ) {
        CONFIG->SetString("MESH_FILE",meshFile);
        CONFIG->SetString("ARAPUCA_MODEL","box");
    }

    float cutSize = 0;
    if(CONFIG->Has("SIPM_CUT") ) cutSize = CONFIG->Value("SIPM_CUT");
    param->GetParameter("-cut", cutSize);
    CONFIG->SetValue("SIPM_CUT",cutSize);
    
    G4String genericParName = "PAR_", genericParTag = "-p";
    const int gParN = 10;
    float gParValue;
    for(int i=0; i<gParN; i++){
        gParValue = 0;
        param->GetParameter(genericParTag+std::to_string(i), gParValue);
        CONFIG->SetValue(genericParName+std::to_string(i),gParValue);
        if(param->Has(genericParTag+std::to_string(i))) LOG_TERM_ERROR("Input -p{0} with name {1} has value {2}",i, genericParName+std::to_string(i), CONFIG->Value(genericParName+std::to_string(i)));
    }


    float wlsMolarity = 1.0-4;
    if(CONFIG->Has("BARWLS_MOLARITY") ) wlsMolarity = CONFIG->Value("BARWLS_MOLARITY") ;
    if(CONFIG->Value("PAR_1")!=0.0){
        wlsMolarity = CONFIG->Value("PAR_1") * 1.0e-6;
    }
    CONFIG->SetValue("BARWLS_MOLARITY",wlsMolarity);
    LOG_TERM_ERROR("Using {0} as molarity for WLS.", wlsMolarity );

    CONFIG->BuildWLSAbs("BARWLS_MOLAR_EXTINCTION","BARWLS_ABSORPTION", wlsMolarity );
    param->GetParameter("-tag",ARAPUCA::Hits::Access()->Tag());

    LOG_TERM_INFO("Arapuca Geant4 Simulation version {0}.{1}.{2}", ARAPUCA_VER, ARAPUCA_SUB, ARAPUCA_ITE);

    //  Run Manager =============================================
    G4RunManager *runManager;
    uint8_t numberOfThreads = G4Threading::G4GetNumberOfCores();
    if(CONFIG->Has("NUMBER_OF_THREADS")) numberOfThreads = std::min( numberOfThreads, (uint8_t) CONFIG->Value("NUMBER_OF_THREADS"));
    param->GetParameter("-j",numberOfThreads);

    if(numberOfThreads<=1){
        runManager = new G4RunManager(); 
    } else {
        runManager = new G4MTRunManager();
        dynamic_cast<G4MTRunManager*>(runManager)->SetNumberOfThreads(numberOfThreads);
    } 

    runManager->SetVerboseLevel(verboseLevel);

    // User Initializations    
    runManager->SetUserInitialization( new ARAPUCA::DetectorConstruction(useGDML) );
    runManager->SetUserInitialization( new ARAPUCA::PhysicsList(verboseLevel) );
    runManager->SetUserInitialization( new ARAPUCA::ActionInitialization() );
    runManager->SetUserAction( new ARAPUCA::RunAction() );
//    runManager->SetUserAction( new ARAPUCA::EventAction() );
    runManager->Initialize();


    if(interactiveMode){

        auto uiExecutive = new G4UIExecutive(argc,argv,"csh");
        auto uiManager = G4UImanager::GetUIpointer();
        auto arapucaIO = new ARAPUCA::IOMessenger(uiManager);  
    	uiManager->ApplyCommand("/control/execute " + inputFile );
        uiExecutive->SessionStart();
	    
        

        delete uiExecutive;
        delete arapucaIO;

    }
    else if(!batchMode){
        
        auto visManager = new G4VisExecutive("quiet");
        auto uiExecutive = new G4UIExecutive(argc,argv,"Qt");
        auto uiManager = G4UImanager::GetUIpointer();
        auto arapucaIO = new ARAPUCA::IOMessenger(uiManager);
        visManager->Initialise();
    	uiManager->ApplyCommand("/control/execute " + inputFile ); 
        uiExecutive->SessionStart();
	    
        delete uiExecutive;
        delete visManager;
        delete arapucaIO;

    } else {

        //auto uiExecutive = new G4UIExecutive(argc,argv,"csh");
        auto uiManager = G4UImanager::GetUIpointer();
        auto arapucaIO = new ARAPUCA::IOMessenger(uiManager);
    	uiManager->ApplyCommand("/control/execute " + inputFile );	    
        delete arapucaIO;
        //delete uiExecutive;
    }

    delete runManager; // The runManager will delete all other pointers owned by it.

    return 0;
}