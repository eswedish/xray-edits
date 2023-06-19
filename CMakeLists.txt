cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# name of proj we want to start
project(Simulation)

# tell CMake it should find Geant4
find_package(Geant4 REQUIRED ui_all vis_all)

# tells cmake to include files within this path (Geant4_USE_FILE)
include(${Geant4_USE_FILE})

# compile all the source files we have in our folder (PROJECT_SOURCE_DIR) and compile later with gcc/g++ same thing for headers
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

file(GLOB MACRO_FILES
    "*.mac"
)
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

file(GLOB DATA_FILES
    "*.dat"
)
file(COPY ${DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR})

# tell cmake the executable, and the main file where the main source code will be placed in, basically compiles all the source code and uses all the headers together with the sim.cc to make an executable named sim
add_executable(sim sim.cc ${sources} ${headers})
target_link_libraries(sim ${Geant4_LIBRARIES})

add_custom_target(Simulation DEPENDS sim)
