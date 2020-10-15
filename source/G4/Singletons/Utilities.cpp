#include "Utilities.h"
#include <fstream>
#include <algorithm>
#include <cmath>

namespace ARAPUCA{

    bool IsFloat(const std::string& s) {
        try {
            std::stof(s);
            return true;
        } catch(...) {
            return false;
        }
    }

    bool IsInt(const std::string& s) {
        try {
            std::stoi(s);
            return true;
        } catch(...) {
            return false;
        }
    }

    float StoF(const std::string& s) {
        try {
            auto result = std::stof(s);
            return result;
        } catch(...) {
            return nanf("");
        }
    }

    float StoI(const std::string& s) {
        try {
            auto result = std::stoi(s);
            return result;
        } catch(...) {
            return nanf("");
        }
    }

    bool SourceExists(const std::string &fileName)
    {

        bool itDoes = false;

        std::ifstream file(fileName);
        itDoes = file.is_open();
        file.close();

        return itDoes;
    }

    void CumulativePDF(const int n, double *yPDF, double *yCDF){

        yCDF[0] = yPDF[0];
        for(int i=1; i<n; i++){
            yCDF[i] = yPDF[i]+yCDF[i-1];
        }
        for(int i=0; i<n; i++) yCDF[i] /= yCDF[n-1];
    }

    int FindClosest(double x0, double *x, int nf, int ni){

        int midPoint = ni + (nf-ni)/2;

        if(midPoint==ni) return ni;

        if( x0<x[midPoint] ) return FindClosest(x0, x, nf = midPoint, ni);
        else return FindClosest(x0, x, nf, ni = midPoint);

    }

    double Interpolate(double x0, double *x, double *y, const int n){

        if(x0<x[0]) return y[0];
        if(x0>x[n-1]) return y[n-1];

        int index = FindClosest(x0,x,n);
        
        if(index>=n) return y[n-1];

        double m = (y[index+1]-y[index])/(x[index+1]-x[index]);
        return y[index]+m*(x0-x[index]);
    }


    CommandLineParameters *CommandLineParameters::m_Instance = nullptr;

    void CommandLineParameters::Parse(int argc, char** argv){

        m_Parameters.clear();
        m_Parameters.reserve(argc);
        for(int32_t i=0; i<argc; i++) m_Parameters.emplace_back(argv[i]);

    }

    bool CommandLineParameters::Has(const std::string& key){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);

        return tag != m_Parameters.end();
    }

    std::string CommandLineParameters::GetParameter(const std::string& key){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);
        if(tag==m_Parameters.end()) return std::string("");

        auto param = m_Parameters[ tag - m_Parameters.begin() +1 ];
        if( param.find("-")==0 ) return std::string("");

        return param;
    }

    void CommandLineParameters::GetParameter(const std::string& key, std::string &str){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);
        if(tag<m_Parameters.end()) {
            auto param = m_Parameters[ tag - m_Parameters.begin() +1 ];
            if( param.find("-")!=0 ) str = param;
        }

    }

    void CommandLineParameters::GetParameter(const std::string& key, int64_t &value){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);
        if(tag<m_Parameters.end()) {
            auto param = m_Parameters[ tag - m_Parameters.begin() +1 ];
            if( param.find("-")!=0 && IsInt(param) ) value = StoI(param);
        }

    }

    void CommandLineParameters::GetParameter(const std::string& key, uint8_t &value){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);
        if(tag<m_Parameters.end()) {
            auto param = m_Parameters[ tag - m_Parameters.begin() +1 ];
            if( param.find("-")!=0 && IsInt(param) ) value = StoI(param);
        }

    }

    void CommandLineParameters::GetParameter(const std::string& key, float &value){

        auto tag = std::find(m_Parameters.begin(), m_Parameters.end(), key);
        if(tag<m_Parameters.end()) {
            auto param = m_Parameters[ tag - m_Parameters.begin() +1 ];
            if( param.find("-")!=0 && IsFloat(param) ) value = StoI(param);
        }

    }        

}