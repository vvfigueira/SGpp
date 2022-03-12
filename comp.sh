#!/bin/sh

mkdir SGpp_build
cd SGpp_build

source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/setup.sh
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/share/Garfield/setupGarfield.sh

cmake ../source
make

./build 100
