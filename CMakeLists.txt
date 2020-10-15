cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(ArapucaSim VERSION 0.1.0 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE=Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(source/3rdparty/spdlog)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


# telling CMake where to find the ROOT installation
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# locating the ROOT package
find_package(ROOT 6.18 CONFIG REQUIRED)
include("${ROOT_USE_FILE}")

add_executable(neutronSim
    source/main.cpp

    source/G4/Singletons/Logger.cpp
    source/G4/Singletons/SensDet.cpp
    source/G4/Singletons/InputTable.cpp
    source/G4/Singletons/MaterialProperties.cpp
    source/G4/Singletons/Hits.cpp
    source/G4/Singletons/Utilities.cpp
    source/G4/Singletons/GDMLBuilder.cpp

    source/G4/Geometry/XARAPUCA.cpp
    source/G4/Geometry/AbsorptionTest.cpp
    source/G4/Geometry/NeutronDetectors.cpp
    
    source/G4/FastModels/DichoricFilterModel.cpp

    source/G4/Generators/GeneralParticleSource.cpp
    
    source/G4/Commands.cpp
    source/G4/RunAction.cpp
    source/G4/EventAction.cpp
    source/G4/DetectorConstruction.cpp
    source/G4/PrimaryGenerator.cpp
    source/G4/PhysicsList.cpp
    source/G4/ActionInitialization.cpp
    source/G4/GeneralCounterSD.cpp
    source/G4/CounterHit.cpp
)

target_link_libraries(neutronSim
    PUBLIC
    spdlog
    ${Geant4_LIBRARIES}
    ${ROOT_LIBRARIES}
    
)

set(CMAKE_INSTALL_PREFIX /home/gvaldivi/Sandbox/Arapuca/ArapucaSim)
install(FILES build/arapuca DESTINATION bin)
