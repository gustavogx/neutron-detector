#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4VSolid.hh"

#include "Generators/GeneralParticleSource.h"

namespace ARAPUCA
{
	class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
	{
	private:
		G4ParticleGun *m_ParticleGun;

		G4VSolid *m_SourceSolid;

		static G4ThreeVector RandomWithinVolume(G4VSolid *solid);
		static void GeneratePhotonsFromWindow(G4ParticleGun * particleGun);
		static void GeneratePhotonsFromWLS(G4ParticleGun *particleGun, G4VSolid *solid);
		static void GeneratePhotonsFromFilter(G4ParticleGun *particleGun);
		static void GenerateLineBeam(G4ParticleGun * particleGun);

	public:
		PrimaryGenerator();
		~PrimaryGenerator();

		virtual void GeneratePrimaries(G4Event *anEvent) override;
	};

} // namespace ARAPUCA
