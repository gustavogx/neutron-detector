#include "Core.h"
#include "Globals.h"

#include "Commands.h"
#include "G4UIdirectory.hh"

namespace ARAPUCA{

    IOMessenger::IOMessenger(G4UImanager *ui) :
        m_ArapucaDirectory(new G4UIdirectory("/arapuca/")),
        m_InputFileName(new G4UIcmdWithAString("/arapuca/inputDataFile",this))
    {
        m_ArapucaDirectory->SetGuidance("ARAPUCA Commands.");
        
        m_InputFileName->SetGuidance("Input File for Tables and Constants.");
        m_InputFileName->SetParameterName("fileName",true);
        m_InputFileName->SetDefaultValue("../data/arapuca.cfg");

        if(ui) Register(ui);
    }

    IOMessenger::~IOMessenger(){
        delete m_ArapucaDirectory;
        delete m_InputFileName;
    }
 
    G4String IOMessenger::GetCurrentValue(G4UIcommand * command) {

        G4String value;

        if( command== m_InputFileName ){
            value = ARAPUCA::Config::Access()->GetConfigFile();
        }

        return value;
    }

    void IOMessenger::SetNewValue(G4UIcommand * command, G4String newValue) {
        
        if( command == m_InputFileName ) ARAPUCA::Config::Access()->SetConfigFile(newValue);

    }

    void IOMessenger::Register(G4UImanager *ui){
        ui->AddNewCommand(m_ArapucaDirectory);
        ui->AddNewCommand(m_InputFileName);
    }
}