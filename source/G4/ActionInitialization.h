#pragma once

#include "G4VUserActionInitialization.hh"

namespace ARAPUCA
{

    class ActionInitialization : public G4VUserActionInitialization
    {
    public:
        virtual void Build() const override;
    };

} // namespace ARAPUCA
