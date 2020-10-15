#include "ActionInitialization.h"
#include "PrimaryGenerator.h"
#include "EventAction.h"

namespace ARAPUCA
{
    void ActionInitialization::Build() const
    {
        SetUserAction(new PrimaryGenerator);
        SetUserAction(new EventAction);
    }

} // namespace ARAPUCA