#pragma once

#define ARAPUCA_VER 0
#define ARAPUCA_SUB 0
#define ARAPUCA_ITE 1

//#include "../3rdparty/lua/src/lua.h"

#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UnitsTable.hh"

#define UNUSED(x) (void)(x)

using namespace CLHEP;

#define MARKHERE LOG_TERM_ERROR("MARK");
#define STOPHERE LOG_TERM_FATAL("STOP");

namespace ARAPUCA{
    constexpr double hc = h_Planck * c_light; // [hc] = MeV mm
}

#define CONFIG ARAPUCA::Config::Access()
#define HITS   ARAPUCA::Hits::Access()