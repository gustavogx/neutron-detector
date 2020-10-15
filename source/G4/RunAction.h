#pragma once
#include "G4UserRunAction.hh"

namespace ARAPUCA{

    class RunAction : public G4UserRunAction{
    public:
        RunAction() : G4UserRunAction() {}
        ~RunAction() = default;

        void BeginOfRunAction(const G4Run* aRun) override;
        void EndOfRunAction(const G4Run* aRun) override;

    };

}