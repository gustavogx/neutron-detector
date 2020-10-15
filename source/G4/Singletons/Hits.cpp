#include "Hits.h"
#include "../Core.h"

namespace ARAPUCA{

    Hits* Hits::m_Instance = nullptr;

    Hits* Hits::Access(){

        if(m_Instance==nullptr) m_Instance = new Hits();

        return m_Instance;
    }

    void Hits::Clear(){

        m_Counter.PerEvent.clear();
        m_Counter.Observable.clear();
        m_Counter.Sum = 0;
        
    }

}