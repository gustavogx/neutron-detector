#include "EventAction.h"
#include "Core.h"
#include "Globals.h"
#include "G4Event.hh"
#include "Globals.h"

namespace ARAPUCA {

void EventAction::BeginOfEventAction(const G4Event* anEvent){

    UNUSED(anEvent);
}

void EventAction::EndOfEventAction(const G4Event* anEvent){

    auto HC = anEvent->GetHCofThisEvent();

    for(int i=0; i<HC->GetNumberOfCollections(); i++ ){
        HC->GetHC(i)->PrintAllHits();
        for(uint64_t j=0; j<HC->GetHC(i)->GetSize(); j++) {
            if (j==HC->GetHC(i)->GetSize() -1 ) {
                ((CounterHit*)HC->GetHC(i)->GetHit(j))->PrintToFile(CONFIG->String("OUTPUT_FILE"));
            }
        }
    }

}


}