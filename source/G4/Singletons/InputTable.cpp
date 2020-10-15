#include "InputTable.h"
#include "Logger.h"
#include "../Core.h"
#include "MaterialProperties.h"
#include "Utilities.h"
#include "G4MaterialPropertiesTable.hh"
#include "Randomize.hh"

#include <fstream>
#include <limits>

namespace ARAPUCA
{
    const auto MaxLength = std::numeric_limits<double>::max();

    Config *Config::m_Instance = nullptr;

    Config *Config::Access()
    {

        if (m_Instance == nullptr) m_Instance = new Config();

        return m_Instance;
    }

    void Config::SetConfigFile(const std::string &fileName)
    {

        if (!SourceExists(fileName))
        {
            LOG_TERM_FATAL("Parser could not find configuration file {0} ", fileName);
            return;
        }

        m_ConfigSource = fileName;
        //m_Table.clear();

        std::ifstream configFile(m_ConfigSource);
        std::string key, value, name, x, material, property;

        do
        {
            if (!configFile.eof())
            {
                configFile >> key; 
                if (!configFile.eof()) { 
                    configFile >> value; 
                    if (key == cfgTags[kData])
                    {
                        // new data entry
                        name = value;
                        m_Table[name] = DataEntry();
                        m_Table[name].IsConst = false;
                        m_Table[name].Size    = 0;
                    }
                    else if (key == cfgTags[kType])  m_Table[name].G4Type = value;
                    else if (key == cfgTags[kValue]) {
                        m_Table[name].Value = std::atof(value.c_str());
                        m_Table[name].IsConst = true;
                    }
                    else if (key == cfgTags[kString]) m_Table[name].String = value;
                    else if (key == cfgTags[kSource] && !m_Table[name].IsConst) {
                        if(SourceExists(value)){ 
                            m_Table[name].Source = value;
                            std::ifstream srcFile(value);
                            bool isX = true;
                            m_Table[name].X.clear();
                            m_Table[name].Y.clear();
                            do{
                                if(!srcFile.eof()) {
                                    srcFile >> x;
                                    if(isX) { 
                                        m_Table[name].X.emplace_back(std::atof(x.c_str()));
                                    } else {
                                        m_Table[name].Y.emplace_back(std::atof(x.c_str()));
                                    }
                                    isX = !isX;
                                }
                            }while(!srcFile.eof());
                            if(m_Table[name].X.size() == m_Table[name].Y.size() ){
                                m_Table[name].Size = m_Table[name].X.size();   
                                LOG_TERM_TRACE("Table\t{0} for property {1} loaded with {2} values.", m_Table[name].Source, name, m_Table[name].Size);                             
                            } else {
                                LOG_TERM_WARN("Table {0} for property {1} is NOT EVEN. Double check it!", value, name);
                                m_Table[name].Size = 
                                      m_Table[name].X.size() < m_Table[name].Y.size() 
                                    ? m_Table[name].X.size() : m_Table[name].Y.size();
                            }
                            if(m_Table[name].Size<2)
                                LOG_TERM_WARN("Table {0} for property {1} has TOO FEW values. Double check it!", m_Table[name].Source, name);
                        } else {
                            LOG_TERM_ERROR("Table {0} not found.", value);
                        }
                        m_Table[name].IsConst = true;
                    }
                }
            }
        } while (!configFile.eof());
    }

    void Config::FillProperty(const std::string &name, G4MaterialPropertiesTable *table, const double &unitX, const double &unitY, const char *key){
        
        if(Has(name)){
            if(m_Table[name].Size) {
                if(unitX!=1.0) for(auto &element : m_Table[name].X) element *= unitX;
                if(unitY!=1.0) for(auto &element : m_Table[name].Y) element *= unitY;
                
                table->AddProperty(
                    key==0 ? m_Table[name].G4Type.c_str() : key,  
                    &m_Table[name].X[0], 
                    &m_Table[name].Y[0],
                    m_Table[name].Size
                ); 
            } 
            else LOG_TERM_ERROR("File {0} empty or not found.", m_Table[name].Source);
        } else LOG_TERM_ERROR("Table {0} not found.", name);
    }

    void Config::ChangeProperty(const std::string &name, G4MaterialPropertiesTable *table, const double &newValue, const char *key){

        if(Has(name)){
            if(m_Table[name].Size) {
                std::vector<double> newY( m_Table[name].Y );
                for(auto &element : newY) element = newValue;
                table->AddProperty(
                    key==0 ? m_Table[name].G4Type.c_str() : key,  
                    &m_Table[name].X.front(), 
                    &newY.front(), 
                    m_Table[name].Size
                );
            } 
            else LOG_TERM_ERROR("File {0} empty or not found.", m_Table[name].Source);
        } else LOG_TERM_ERROR("Table {0} not found.", name);
    }


    void Config::BuildCDF(const std::string &pdfName,const std::string &cdfName){

        if(Has(pdfName)){ 
            m_Table[cdfName] = DataEntry(); 
            m_Table[cdfName].Source = m_Table[pdfName].Source; 
            m_Table[cdfName].IsConst = m_Table[pdfName].IsConst;
            m_Table[cdfName].G4Type = m_Table[pdfName].G4Type;
            m_Table[cdfName].X = m_Table[pdfName].X;
            m_Table[cdfName].Size = m_Table[pdfName].X.size();
            m_Table[cdfName].Y.resize( m_Table[cdfName].Size );
            CumulativePDF(m_Table[cdfName].Size, &m_Table[pdfName].Y[0], &m_Table[cdfName].Y[0]);
        } else LOG_TERM_ERROR("Table {0} not found.", pdfName);
    }


    double Config::RandomFromSample(const std::string &cdfName){

        if(Has(cdfName)) return Interpolate( G4UniformRand() , &m_Table[cdfName].Y[0], &m_Table[cdfName].X[0], m_Table[cdfName].Size );
        else LOG_TERM_ERROR("Table {0} not found.", cdfName);
        return 0;
    }


    void Config::BuildWLSAbs(const std::string &lambdaTableName,const std::string &energyTableName, const double &molarity){

        double energy;
        if(Has(lambdaTableName)){ 
            m_Table[energyTableName] = DataEntry(); 
            m_Table[energyTableName].Source = m_Table[lambdaTableName].Source; 
            m_Table[energyTableName].IsConst = m_Table[lambdaTableName].IsConst;
            m_Table[energyTableName].G4Type = m_Table[lambdaTableName].G4Type;
            m_Table[energyTableName].Size = m_Table[lambdaTableName].X.size();
            for(int64_t i = m_Table[energyTableName].Size-1; i>=0; i--){
                energy = hc/(m_Table[lambdaTableName].X[i]*nm);
                if(m_Table[lambdaTableName].Y[i]!=0.0) m_Table[energyTableName].Y.emplace_back( cm/(m_Table[lambdaTableName].Y[i]*molarity) );
                else m_Table[energyTableName].Y.emplace_back(MaxLength*mm);
                m_Table[energyTableName].X.emplace_back( energy*MeV );
                LOG_TERM_INFO("{0} nm\t{1} eV\t{2} mm",m_Table[lambdaTableName].X[i], energy/eV, m_Table[energyTableName].Y.back()/mm );
            }
        } else LOG_TERM_ERROR("Table {0} not found.", lambdaTableName);
    }

} // namespace ARAPUCA