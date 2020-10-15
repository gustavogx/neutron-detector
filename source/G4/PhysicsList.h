#pragma once
#include "G4VModularPhysicsList.hh"
//#include "FTFP_BERT.hh"

namespace ARAPUCA{

class PhysicsList : public G4VModularPhysicsList 
{
public:
  PhysicsList(u_int8_t verbose = 1);
  virtual ~PhysicsList()=default;

  PhysicsList(const PhysicsList &) = delete;
  PhysicsList & operator=(const PhysicsList &)=delete;
  
};

}