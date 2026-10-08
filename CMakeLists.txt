cmake_minimum_required(VERSION 3.27.9)
project(FrameCounter VERSION ${GEODE_MOD_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geode REQUIRED)

# Includi la libreria slc che abbiamo copiato
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} MODULE src/main.cpp)
setup_geode_mod(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
