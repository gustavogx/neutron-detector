#pragma once
#include "G4ParticleGun.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include <vector>

namespace ARAPUCA{

class GPS : public G4ParticleGun {
private:
    
    static GPS *m_Instance;

    std::vector<double> m_Lambda, m_Energy, m_PDF, m_CDF;
 
    GPS();

    static double Eval(double x0, double *x, double *y, const int n);
    static int Find(double x0, double *x, int nf, int ni = 0);
    static void CumulativePDF(const int n, double *yPDF, double *yCDF);

public:
    static GPS *Access();
    
    GPS(GPS const &) = delete;
    void operator=(GPS const &) = delete;

    void FillEnergy(const std::vector<double> &x);
    void FillLambda(const std::vector<double> &x);
    void FillPDF(const std::vector<double> &y);

    double *GetEnergyTable() { return &m_Energy[0]; }
    double *GetLambdaTable() { return &m_Lambda[0]; }
    double *GetPDFTable() { return &m_PDF[0]; }    
    double *GetCDFTable() { return &m_CDF[0]; }

    double GetRandomEnergy();
    double GetRandomLambda();

    inline void DoSampleEnergy() { SetParticleEnergy( Eval(m_CDF.size(), &m_CDF[0], &m_Energy[0], m_CDF.size()) );  };
    inline void DoRandomDirection() { SetParticleMomentumDirection(G4RandomDirection()); }
    inline void DoRandomPolaziration() { SetParticlePolarization(G4RandomDirection()); }
};

#define SOURCE GPS::Access()

}