#include "RunAction.h"
#include "Core.h"
#include "Globals.h"
#include "G4SDManager.hh"

namespace ARAPUCA{

    void RunAction::BeginOfRunAction(const G4Run* aRun) {

        UNUSED(aRun);
        //Hits::Access()->Clear();

    }

    void RunAction::EndOfRunAction(const G4Run* aRun) {

        UNUSED(aRun);

        //sdManager->FindSensitiveDetector("SiPM_" + std::to_string(i))->

        
        //LOG_TERM_ERROR("{0} {1}", Hits::Access()->GetSum(), Hits::Access()->GetAvgObs() );
        //LOG_TERM_ERROR("{0}", SDList()->GetScoresLine() );
/*
        for(auto aHit : HITS->GetCH()) aHit.Print();

        if(Hits::Access()->Tag()!="") {
            LOG_FILE(ARAPUCA::Config::Access()->String("OUTPUT_FILE"),"#{0}\t{1}\t{2}",  Hits::Access()->Tag(), Hits::Access()->GetSum(), Hits::Access()->GetAvgObs());            

            for(auto value : ARAPUCA::Hits::Access()->GetObs()){
                LOG_FILE(ARAPUCA::Config::Access()->String("OUTPUT_FILE"),"{0}",value);
            }
        }
        else {
            LOG_FILE(ARAPUCA::Config::Access()->String("OUTPUT_FILE"),"{0}\t{1}", Hits::Access()->GetSum(), Hits::Access()->GetAvgObs() );
        }
*/
    }

}