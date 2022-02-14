# Use the code in this section for standalone projects
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(Teste)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

#---Build executable------------------------------------------------------------
add_executable(build GarfieldDetector.cc Constantes.hh)

target_link_libraries(build Garfield::Garfield)

# ---Copy all files locally to the build directory-------------------------------
foreach(_file ar_70_co2_30_1000mbar.gas)
  configure_file(${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file} COPYONLY)
endforeach()