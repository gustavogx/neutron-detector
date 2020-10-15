#include "DichroicFilterModel.h"
#include "G4OpticalPhoton.hh"
#include "../Core.h"
#include "../Globals.h"

namespace ARAPUCA{

DichroicFilterModel::DichroicFilterModel(const G4String &aName)
    : G4VFastSimulationModel(aName), m_Type(DichroicFilterModelType::HighPass), m_Envelope(nullptr), m_Cut(0.0), m_KillOnTransmission(true) {
    if(CONFIG->Has("FILTER_REFLEC")) m_Type = DichroicFilterModelType::LUT;
}

DichroicFilterModel::DichroicFilterModel(const G4String &aName, G4Envelope* anEnvelope,const G4bool &isUnique)
    : G4VFastSimulationModel(aName, anEnvelope, isUnique), m_Type(DichroicFilterModelType::HighPass), m_Envelope(anEnvelope), m_Cut(0.0), m_KillOnTransmission(true) {
    if(CONFIG->Has("FILTER_REFLEC")) m_Type = DichroicFilterModelType::LUT;
}

G4bool DichroicFilterModel::IsApplicable(const G4ParticleDefinition &aParticleDefinition) {
    return &aParticleDefinition == G4OpticalPhoton::OpticalPhotonDefinition();

}

G4bool DichroicFilterModel::ModelTrigger(const G4FastTrack &) {
    return true;

}

void DichroicFilterModel::ReflectPhoton(const G4FastTrack &aTrack, G4FastStep &aStep){

        auto position = aTrack.GetPrimaryTrack()->GetStep()->GetPreStepPoint()->GetPosition();
        auto normal = aTrack.GetEnvelopeLogicalVolume()->GetSolid()->SurfaceNormal(position);
        normal = (*aTrack.GetEnvelopePhysicalVolume()->GetObjectRotation())*normal;

        u_int8_t axisOrientation =3;
        for(u_int8_t i=0; i<3; i++) if(normal[i]==1.0) axisOrientation = i;

        if(axisOrientation<3){
            auto momentum = aTrack.GetPrimaryTrack()->GetStep()->GetPreStepPoint()->GetMomentum();
            
            switch(axisOrientation){
                case 0 : momentum.setX( -momentum.x() ); break;
                case 1 : momentum.setY( -momentum.y() ); break;
                case 2 : momentum.setZ( -momentum.z() ); break;
            }
             
            aStep.SetPrimaryTrackFinalMomentum( momentum );
        } else {
            // do full reflection
        }

        //LOG_TERM_ERROR("Filter Lambda {0} nm", (hc/aTrack.GetPrimaryTrack()->GetTotalEnergy())/nm);
}

void DichroicFilterModel::TransmitPhoton(const G4FastTrack &aTrack, G4FastStep &aStep){

        if(m_KillOnTransmission) {

            aStep.KillPrimaryTrack();
            return;
        }

        auto position = aTrack.GetPrimaryTrack()->GetStep()->GetPreStepPoint()->GetPosition();
        auto normal = aTrack.GetEnvelopePhysicalVolume()->GetLogicalVolume()->GetSolid()->SurfaceNormal(position);
        auto tranlation = aTrack.GetEnvelopePhysicalVolume()->GetObjectTranslation();
        position = position - tranlation;
        position.setY( position.y() + CONFIG->Value("FILTER_THICKNESS")); 

        aStep.SetPrimaryTrackFinalTime( aTrack.GetPrimaryTrack()->GetGlobalTime()*ns + CONFIG->Value("FILTER_THICKNESS")/c_light);        
        aStep.SetPrimaryTrackFinalPosition( position );
}

void DichroicFilterModel::DoIt(const G4FastTrack &aTrack, G4FastStep &aStep) {

    auto energy = aTrack.GetPrimaryTrack()->GetKineticEnergy();
    
    if( (m_Type==DichroicFilterModelType::LowPass  && energy>m_Cut) ||
        (m_Type==DichroicFilterModelType::HighPass && energy<m_Cut) )
    {   
        ReflectPhoton(aTrack,aStep);
        return;
    }


    if(m_Type==DichroicFilterModelType::LUT){

        auto reflect = Interpolate(energy, &CONFIG->X("FILTER_REFLEC")[0], &CONFIG->Y("FILTER_REFLEC")[0], CONFIG->Size("FILTER_REFLEC"));
        bool doReflect = G4UniformRand() >= reflect;

        if(doReflect) ReflectPhoton(aTrack,aStep);        

    }


    TransmitPhoton(aTrack,aStep);
    return;
}

}