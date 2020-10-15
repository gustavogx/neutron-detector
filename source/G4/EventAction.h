#pragma once
#include "G4UserEventAction.hh"

namespace ARAPUCA{

class EventAction : public G4UserEventAction {
  public:
      EventAction() = default;
      ~EventAction() = default;

  public: // with description
      void BeginOfEventAction(const G4Event* anEvent);
      void EndOfEventAction(const G4Event* anEvent);     

};

}
