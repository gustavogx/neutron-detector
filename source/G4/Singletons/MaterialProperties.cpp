#include "MaterialProperties.h"
#include "../Core.h"
#include "Logger.h"
#include "InputTable.h"

namespace ARAPUCA{

G4MaterialPropertyIndex MaterialProperties::GetPropertyIndex(const std::string &tag){
    size_t pos = tag.find("_");
    if(pos<tag.length()-1) return GetPropertyIndex( tag.substr(pos+1) );

    return kNullPropertyIndex;
}

std::string MaterialProperties::GetPropertyName(const std::string &tag){

    size_t pos = tag.find("_");
    if(pos<tag.length()-1) return tag.substr(pos+1);

    return "";

}

std::string MaterialProperties::GetMaterialName(const std::string &tag){
    size_t pos = tag.find("_");

    return tag.substr(0,pos );

}

MaterialProperties* MaterialProperties::m_Instance=0;

G4MaterialPropertyVector* MaterialProperties::Load(const std::string &fileName, const double &unit, const bool &isLambda){

    std::ifstream srcFile(fileName);
    if(srcFile.is_open()){ 
        std::string key;
        bool isX = true;
        std::vector<double> X,Y;
        do{
            if(!srcFile.eof()) {
                srcFile >> key;
                if(isX) { 
                    if(!isLambda) X.emplace_back(std::atof(key.c_str())*unit);
                    else X.emplace_back(hc/(std::atof(key.c_str())*unit));
                } else {
                    Y.emplace_back(std::atof(key.c_str()));
                }
                isX = !isX;
            }
        }while(!srcFile.eof());
        size_t size = std::min(X.size(),Y.size());
        if(size>1) return new G4MaterialPropertyVector( &(X[0]), &(Y[0]), X.size() );
        else LOG_TERM_ERROR("Property File <{0}> has not enough data.",fileName);
    };
    LOG_TERM_ERROR("Property File <{0}> has not found.",fileName);
    return nullptr;
}

void MaterialProperties::Fill(){

    std::string material, property;
    for(auto &entry : Config::Access()->Map()){
        if(entry.second.G4Type=="property" || entry.second.G4Type=="PROPERTY" || entry.second.G4Type=="Property"){
            auto table = MaterialProperties::Load(entry.second.Source);                        
            material = MaterialProperties::GetMaterialName(entry.first);
            property = MaterialProperties::GetPropertyName(entry.first);
            LOG_TERM_TRACE("File {0} has {2} values:", entry.second.Source, property, table->GetVectorLength());
            for(size_t i=0; i<table->GetVectorLength(); i++) LOG_TERM_TRACE("\t{0}\t{1}", table->Energy(i), table->Value(table->Energy(i)) );
            if(table!=nullptr) MaterialProperties::Access()->Map()[material][property] = table;
        }
    }
    if(Config::Access()->Value("VERBOSITY_LEVEL")==Verbosity::kAll){
        for(auto &mat : MaterialProperties::Access()->Map() ) for(auto &prop : mat.second )
            LOG_TERM_TRACE("\tStored for material {0}, property {1} with {2} data points.", mat.first, prop.first, prop.second->GetVectorLength());
    }
}

}
