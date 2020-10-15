#pragma once

#include "G4VFastSimulationModel.hh"
#include <vector>

namespace ARAPUCA {

enum class DichroicFilterModelType { LowPass, HighPass, LUT };

class DichroicFilterModel : public G4VFastSimulationModel {
private:
	DichroicFilterModelType m_Type;
	G4Envelope* m_Envelope;
	double m_Cut;
	bool m_KillOnTransmission;
	std::vector<double> m_LUTEnergy;
	std::vector<double> m_LUTReflect;

	void ReflectPhoton(const G4FastTrack &aTrack, G4FastStep &aStep);
	void TransmitPhoton(const G4FastTrack &aTrack, G4FastStep &aStep);

public:
    
    DichroicFilterModel(const G4String &aName = "DichroicFilter");
    DichroicFilterModel(const G4String &aName, G4Envelope* anEnvelope,const G4bool &isUnique=true);

	G4bool IsApplicable(const G4ParticleDefinition &aParticleDefinition) override;
	G4bool ModelTrigger(const G4FastTrack &aTrack) override;
	void DoIt(const G4FastTrack &aTrack, G4FastStep &aStep) override;

	inline void SetLowPass(const double &cut=0) { m_Type = DichroicFilterModelType::LowPass; m_Cut = cut!=0.0 ? cut : m_Cut; }
	inline void SetHighPass(const double &cut=0) { m_Type = DichroicFilterModelType::HighPass; m_Cut = cut!=0.0 ? cut : m_Cut; }
	inline void SetKillOnTransmission(const double &kill=true) { m_KillOnTransmission = kill; }

};

}
