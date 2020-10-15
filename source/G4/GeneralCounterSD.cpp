#include "GeneralCounterSD.h"
#include "Core.h"
#include "Globals.h"
#include "G4SDManager.hh"

#include <iostream>
#include <iomanip>


namespace ARAPUCA
{
    GeneralCounterSD::GeneralCounterSD(const G4String &sdName,const G4String &hcName) : G4VSensitiveDetector(sdName), m_HitsCollection(nullptr), m_HCID(-1) {
        collectionName.insert(hcName);

        if(CONFIG->Has("SIPM_EFFICIENCY")){
            m_Energy = CONFIG->X("SIPM_EFFICIENCY");
            m_Efficiency = CONFIG->Y("SIPM_EFFICIENCY");
        }

    }

    GeneralCounterSD::~GeneralCounterSD() {}

    void GeneralCounterSD::Initialize(G4HCofThisEvent* hcE){

        UNUSED(hcE);

        m_HitsCollection = new CounterHitsCollection(SensitiveDetectorName, collectionName[0]);
        if(m_HCID<0) m_HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        hcE->AddHitsCollection(m_HCID, m_HitsCollection);

    }
    
    void GeneralCounterSD::EndOfEvent(G4HCofThisEvent* hcE){

        UNUSED(hcE); 
    }

    G4bool GeneralCounterSD::ProcessHits(G4Step *aStep, G4TouchableHistory *tHist)
    {
        UNUSED(tHist);

        //HITS->GetCH().emplace_back(aStep);

        //Hits::Access()->Add();

        m_HitsCollection->insert( new CounterHit(aStep) );
        /*
        if(m_Energy.size()>1){
            auto energy = aStep->GetPreStepPoint()->GetTotalEnergy();
            auto isHit = G4UniformRand() <= Interpolate( energy/eV, &m_Energy[0], &m_Efficiency[0], m_Energy.size());

            if(isHit) m_HitsCollection->insert( new CounterHit(aStep) );
        } else {
            m_HitsCollection->insert( new CounterHit(aStep) );
        }
        */
        //auto direction = aStep->GetPreStepPoint()->GetMomentumDirection();
        //auto distance  = aStep->GetTrack()->GetTrackLength();
        //G4ThreeVector normal = {0,0,1};
        //double cosAngle = (direction * normal)/direction.mag();
        //Hits::Access()->AddObservable(cosAngle);
        //std::string sdName = this->GetName();
        //int pos =( std::find( sdName.begin(), sdName.end(), "_" ) - sdName.end() );
        //LOG_TERM_ERROR( aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetCopyNo() );
        //Hits::Access()->AddObservable(  ); //direction.x() ); 
        
        return true;
    }

} // namespace ARAPUCA