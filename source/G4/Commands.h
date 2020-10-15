#pragma once

#include "G4UImanager.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

class G4UIcmdWithAString;

namespace ARAPUCA
{
    class IOMessenger : public G4UImessenger
    {
    public:
        IOMessenger(G4UImanager *ui = 0);
        ~IOMessenger();

        G4String GetCurrentValue(G4UIcommand * command) override;
        void SetNewValue(G4UIcommand * command, G4String newValue) override;

        inline G4UIdirectory* Directory() { return m_ArapucaDirectory; }
        inline G4UIcmdWithAString* FileName() { return m_InputFileName; }

        void Register(G4UImanager *ui);

    private: //commands
        G4UIdirectory *m_ArapucaDirectory;
        G4UIcmdWithAString *m_InputFileName;
    };

} // namespace ARAPUCA
