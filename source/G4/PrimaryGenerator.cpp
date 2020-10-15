#include "PrimaryGenerator.h"
#include "Core.h"
#include "Globals.h"

#include "G4Geantino.hh"
#include "G4Neutron.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4VSolid.hh"

namespace ARAPUCA
{
	PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction(),
										   m_ParticleGun(new G4ParticleGun(G4Neutron::Definition())),
										   m_SourceSolid(nullptr)
	{

		m_ParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
		m_ParticleGun->SetParticleEnergy(1*eV);    
		m_ParticleGun->SetParticleMomentumDirection(G4RandomDirection());


	}

	PrimaryGenerator::~PrimaryGenerator()
	{

		delete m_ParticleGun;
	}

	G4ThreeVector PrimaryGenerator::RandomWithinVolume(G4VSolid *solid){

		G4ThreeVector min, max, pos;
		solid->BoundingLimits(min,max);
		do{
			pos.setX(min.getX() + G4UniformRand() * ( max.getX() - min.getX() ) );
			pos.setY(min.getY() + G4UniformRand() * ( max.getY() - min.getY() ) );
			pos.setZ(min.getZ() + G4UniformRand() * ( max.getZ() - min.getZ() ) );
			
		}while( solid->Inside(pos)==kOutside);

		return pos;
	}

	void PrimaryGenerator::GeneratePhotonsFromWindow(G4ParticleGun * particleGun){

		auto config = ARAPUCA::Config::Access();

  		G4ParticleDefinition* particle = particleGun->GetParticleDefinition();
  		if (particle == G4OpticalPhoton::Definition()) {

			double d = config->Value("BOX_DIM_D")*mm;
			double dx0 = config->Value("BOX_DIM_L")*mm - d;
			double dy0 = config->Value("BOX_DIM_H")*mm - d;
			double dz0 = config->Value("BOX_DIM_W")*mm - d;

			double x0 = dx0*(G4UniformRand()-0.5);
			double y0 = dy0*(0.5);
			double z0 = dz0*(G4UniformRand()-0.5);
			particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

			x0 = G4UniformRand()-0.5;
			y0 = -G4UniformRand(); 
			z0 = G4UniformRand()-0.5;

			particleGun->SetParticleMomentumDirection(G4ThreeVector(x0,y0,z0));
			particleGun->SetParticlePolarization(G4ThreeVector(x0,y0,z0));
			
		} 

	}

	void PrimaryGenerator::GeneratePhotonsFromWLS(G4ParticleGun *particleGun, G4VSolid *solid){		
		
  		G4ParticleDefinition* particle = particleGun->GetParticleDefinition();

  		if (particle == G4OpticalPhoton::Definition()) {

			particleGun->SetParticlePosition( RandomWithinVolume(solid) );

			particleGun->SetParticleMomentumDirection( G4RandomDirection() );
			particleGun->SetParticlePolarization( G4RandomDirection() );

			particleGun->SetParticleEnergy( hc/(490*nm) );
		}

	}

	void PrimaryGenerator::GeneratePhotonsFromFilter(G4ParticleGun *particleGun){		
		
  		G4ParticleDefinition* particle = particleGun->GetParticleDefinition();

  		if (particle == G4OpticalPhoton::Definition()) {

				double dx0 = CONFIG->Value("WLS_DIM_L")*mm;
				double dz0 = CONFIG->Value("WLS_DIM_W")*mm;
				double x0 = dx0*(G4UniformRand()-0.5);
				double y0 = CONFIG->Value("WLS_DIM_H")*mm/2 + 1.5*mm;
				double z0 = dz0*(G4UniformRand()-0.5);
				particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

				auto direction = G4RandomDirection();
				if(direction.y()>0) direction.setY( -direction.y() );
				particleGun->SetParticleMomentumDirection( direction );
				particleGun->SetParticlePolarization( G4RandomDirection() );

				double energy;
				if(CONFIG->Value("PAR_0")==0.0) {
					energy = hc/(CONFIG->RandomFromSample("pTP_SPECTRUM")*nm)*MeV;
				} else {
					energy = hc/(CONFIG->Value("PAR_0")*nm)*MeV;
					LOG_TERM_ERROR("Photon Energy set to {0} eV", energy/eV );
				}
				particleGun->SetParticleEnergy( energy*MeV );
				
		}

	}

	void PrimaryGenerator::GenerateLineBeam(G4ParticleGun * particleGun){
		particleGun->SetParticleMomentumDirection(G4ThreeVector(1,0,0));
		particleGun->SetParticlePolarization(G4ThreeVector(1,1,1));
		particleGun->SetParticlePosition(G4ThreeVector(-2*cm,0,0));
		particleGun->SetParticleEnergy( 1*MeV );
	}

	void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
	{
		m_ParticleGun->SetParticleMomentumDirection(G4RandomDirection());
		m_ParticleGun->GeneratePrimaryVertex(anEvent);
	}

} // namespace ARAPUCA

