#--------------------------------------------------------------------------------
# Definição do projeto
#

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(SGpp)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

#--------------------------------------------------------------------------------
# Definição do executável 'build' com as bibliotecas e códigos fonte
#

add_executable(build GarfieldDetector.cc Constantes.hh colors.hh)

target_link_libraries(build Garfield::Garfield)

#--------------------------------------------------------------------------------
# Cópia de arquivos
#

foreach(_file ar_70_co2_30_1000mbar.gas)
  configure_file(${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file} COPYONLY)
endforeach()