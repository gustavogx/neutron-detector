#pragma once
#include "Singletons/Logger.h"
#include "G4VHit.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"

namespace ARAPUCA
{

	class CounterHit : public G4VHit
	{
	public:
		// Data Members
		uint64_t ID;
		G4ThreeVector Position, Direction;
		double Energy;

		CounterHit(G4Step *aStep) : ID(aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetCopyNo()),
									Position(aStep->GetPreStepPoint()->GetPosition()), //-aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetObjectTranslation()),
									Direction(aStep->GetPreStepPoint()->GetMomentumDirection()),
									Energy(aStep->GetPreStepPoint()->GetTotalEnergy())
		{ LOG_TERM_ERROR("{0}\t{1}",ID,Energy/CLHEP::MeV);
		}
		CounterHit() = delete;
		~CounterHit() = default;

		inline void *operator new(size_t);
		inline void operator delete(void *);

		void Print() override;
		void PrintToFile(std::string outFileName);
		std::ostream &operator<<(std::ostream &os);
	};

	typedef G4THitsCollection<CounterHit> CounterHitsCollection;

	extern G4ThreadLocal G4Allocator<CounterHit> *CounterHitAllocator;

	inline void *CounterHit::operator new(size_t)
	{
		if (CounterHitAllocator == 0)
			CounterHitAllocator = new G4Allocator<CounterHit>;
		void *aHit;
		aHit = (void *)CounterHitAllocator->MallocSingle();
		return aHit;
	}

	inline void CounterHit::operator delete(void *aHit)
	{
		CounterHitAllocator->FreeSingle((CounterHit *)aHit);
	}
	
} // namespace ARAPUCA