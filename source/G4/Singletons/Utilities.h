#pragma once
#include <string>
#include <vector>

namespace ARAPUCA{

    bool SourceExists(const std::string &fileName);

    inline bool KeyIsColor(const std::string &key){
        return key=="Color" || key=="color" ||key=="COLOR" || key=="Colour" || key=="colour" || key=="COLOUR";
    }

    inline bool KeyIsSensDet(const std::string &key){
        return key=="SensDet" || key=="sensdet" ||key=="SENSDET" || key=="SD" || key=="sd";
    }

    void CumulativePDF(const int n, double *yPDF, double *yCDF);
    int FindClosest(double x0, double *x, int nf, int ni=0);
    double Interpolate(double x0, double *x, double *y, const int n);

    struct CommandLineParameters
    {
    private:
        static CommandLineParameters* m_Instance;
        std::vector<std::string> m_Parameters;
        CommandLineParameters() = default;

    public:
        
        static CommandLineParameters* Access() {
            if(m_Instance==nullptr) m_Instance = new CommandLineParameters();
            return m_Instance;
        }

        void Parse(int argc, char** argv);
        std::string GetParameter(const std::string& key);
        void GetParameter(const std::string& key, std::string &str);
        void GetParameter(const std::string& key, uint8_t &value);
        void GetParameter(const std::string& key, int64_t &value);
        void GetParameter(const std::string& key, float &value);
        bool Has(const std::string& key);
    };

}