#include "SensDet.h"

namespace ARAPUCA{

SensDet *SensDet::m_Instance = nullptr;

SensDet *SensDet::Instance(){

    if (m_Instance == nullptr) m_Instance = new SensDet();

    return m_Instance;        
}

}