#include "Core.h"
#include "Globals.h"
#include "CounterHit.h"

namespace ARAPUCA{

    G4ThreadLocal G4Allocator<CounterHit>* CounterHitAllocator;

    void CounterHit::Print(){
        LOG_TERM_WARN("{0}\t{1}\t{2}\t{3}", ID, CONFIG->Value("BARWLS_MOLARITY")*1e6, Energy/eV, hc/Energy/nm);
    }
    
    std::ostream& CounterHit::operator<<(std::ostream& os)
    {
        os << ID << "\t";
        os << Position.x()/mm << "\t";
        os << Position.y()/mm << "\t";
        os << Position.z()/mm << "\t"; 
        os << Direction.x() << "\t";
        os << Direction.y() << "\t";
        os << Direction.z() << "\t";
        os << Energy/eV;
        return os;
    }

    void CounterHit::PrintToFile(std::string outFileName){
        //Print();
        //LOG_FILE(outFileName,"{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}", ID, Position.x(), Position.y(), Position.z(), Direction.x(), Direction.y(), Direction.z(), Energy/eV);
        LOG_FILE(outFileName,"{0}\t{1}\t{2}\t{3}", CONFIG->Value("BARWLS_MOLARITY")*1e6, ID, Energy/eV, hc/Energy/nm);
    }

}
