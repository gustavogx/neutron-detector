#include "GeneralParticleSource.h"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"


#include "../Core.h"
#include "../Globals.h"

namespace ARAPUCA{

    GPS *GPS::m_Instance = nullptr;

	GPS *GPS::Access(){
		if(m_Instance == nullptr) m_Instance = new GPS();
		return m_Instance;
	}

	GPS::GPS() : G4ParticleGun( G4OpticalPhoton::Definition() ) {;}

	int GPS::Find(double x0, double *x, int nf, int ni){

		int midPoint = ni + (nf-ni)/2;

		if(midPoint==ni) return ni;

		if( x0<x[midPoint] ) return Find(x0, x, nf = midPoint, ni);
		else return Find(x0, x, nf, ni = midPoint);

	}

	double GPS::Eval(double x0, double *x, double *y, const int n){

		if(x0<x[0]) return y[0];
		if(x0>x[n-1]) return y[n-1];

		int index = Find(x0,x,n);

		return y[index];
	}

	void GPS::CumulativePDF(const int n, double *yPDF, double *yCDF){

		yCDF[0] = yPDF[0];
		for(int i=1; i<n; i++){
			yCDF[i] = yPDF[i]+yCDF[i-1];
		}
		for(int i=0; i<n; i++) yCDF[i] /= yCDF[n-1];
	}

    void GPS::FillEnergy(const std::vector<double> &x) { 
		m_Energy = x;
		m_Lambda.resize( m_Energy.size() );

		for(size_t i=0; i<m_Energy.size(); i++) m_Lambda[i] = hc/m_Energy[i];
	}

    void GPS::FillLambda(const std::vector<double> &x) { 
		m_Lambda = x;
		m_Energy.resize( m_Lambda.size() );

		for(size_t i=0; i<m_Lambda.size(); i++) m_Energy[i] = hc/m_Lambda[i];
	}	

	void GPS::FillPDF(const std::vector<double> &y) { 
		m_PDF = y; 
		CumulativePDF(m_PDF.size(), &m_PDF[0], &m_CDF[0]);
	}

	double GPS::GetRandomEnergy(){

		return Eval(G4UniformRand(), &m_CDF[0], &m_Energy[0], m_CDF.size());

	}

	double GPS::GetRandomLambda(){

		return Eval(G4UniformRand(), &m_CDF[0], &m_Lambda[0], m_CDF.size());

	}

}
