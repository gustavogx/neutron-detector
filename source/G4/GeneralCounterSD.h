#pragma once
#include "G4VSensitiveDetector.hh"
#include "CounterHit.h"

namespace ARAPUCA
{
class GeneralCounterSD : public G4VSensitiveDetector
{
private:
    CounterHitsCollection* m_HitsCollection;
    G4int m_HCID;
    std::vector<double> m_Energy, m_Efficiency;

public:
    GeneralCounterSD(const G4String &sdName, const G4String &hcName);
    ~GeneralCounterSD();

    virtual void Initialize(G4HCofThisEvent* hcE) override;
    virtual void EndOfEvent(G4HCofThisEvent* hcE) override;
    
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *tHist) override;

    inline G4int GetHCID() { return m_HCID; }
    inline CounterHitsCollection* GetHitsCollection() { return m_HitsCollection; }
};
} // namespace ARAPUCA