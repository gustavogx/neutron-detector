#pragma once

#include <vector>
#include <utility>
#include <string>
#include <map>

#include "G4MaterialPropertiesTable.hh"

namespace ARAPUCA
{

    enum InputTags { kData=0, kType, kSource, kValue, kString };
    const std::string cfgTags[] = { ".data", ".type", ".source", ".value", ".string" };
    
    struct DataEntry{
        DataEntry() = default;
        std::string G4Type, Source, String;
        uint32_t Size;
        std::vector<double> X, Y;
        double Value;
        bool IsConst;
    };

    bool IsFloat(const std::string& s);
    float StoF(const std::string& s);

    bool IsInt(const std::string& s);
    
    class Config
    {
    private:
        std::string m_ConfigSource;
        std::map<std::string, DataEntry  > m_Table;

        static Config *m_Instance;

        Config() = default;

    public:
        Config(Config const &) = delete;
        void operator=(Config const &) = delete;

        static Config *Access();
        void SetConfigFile(const std::string &fileName);

        inline std::string GetConfigFile() const { return m_ConfigSource; }

        inline const char* G4Type(const std::string &name) { return m_Table[name].G4Type.c_str(); }
        inline u_int32_t& Size(const std::string &name) { return m_Table[name].Size; }
        inline std::vector<double> &X(const std::string &name) { return m_Table[name].X; }
        inline std::vector<double> &Y(const std::string &name) { return m_Table[name].Y; }
        inline double& Value(const std::string &name) { return m_Table[name].Value; }
        inline std::string& String(const std::string &name) { return m_Table[name].String; }
        void FillProperty(const std::string &name, G4MaterialPropertiesTable *table, const double &unitX=1, const double &unitY=1,  const char *key=0);
        void ChangeProperty(const std::string &name, G4MaterialPropertiesTable *table, const double &unit = 1, const char *key=0);

        void BuildCDF(const std::string &pdfName,const std::string &cdfName);
        double RandomFromSample(const std::string &cdfName);

        void BuildWLSAbs(const std::string &lambdaTableName,const std::string &energyTableName, const double &molarity=1.0);

        inline bool Has(const std::string &name) {return m_Table.find(name) != m_Table.end(); }

        void SetValue(const std::string &name, const double& value) { m_Table[name].Value = value; }
        void SetString(const std::string &name, const std::string& value) { m_Table[name].String = value; }

        inline std::map<std::string, DataEntry>& Map() {return m_Table;}
        inline std::map<std::string, DataEntry> Map() const {return m_Table;}

    };   

} // namespace ARAPUCA
