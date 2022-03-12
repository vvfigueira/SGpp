// Includes do C++

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

// Includes do ROOT

#include <TCanvas.h>
#include <TROOT.h>
#include "TApplication.h"

// Includes da Simulação

#include "Constantes.hh"
#include "colors.hh"

// Includes do Garfield

#include "Garfield/AvalancheMC.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/DriftLineRKF.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/ViewDrift.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/Random.hh"
#include "Garfield/GarfieldConstants.hh"

using namespace Garfield;

int main(int argc,char * argv[]){

    if(argc == 1){
        std::cout <<"\n Uso: ./build [nElétrons]\n";
        return 0;
    }
    
    // Definição do meio gasoso

    auto fMediumMagboltz = new Garfield::MediumMagboltz();
    fMediumMagboltz->SetComposition("ar", 90., "ch4", 10.);
    fMediumMagboltz->SetTemperature(Dim::temperaturagas/1);
    fMediumMagboltz->SetPressure(Dim::pressaogas/1);
    fMediumMagboltz->Initialise(true);

    // Informar o coeficiente de transferência de Penning
    
    const double rPenning = 0.21;
    const double lambdaPenning = 0.;
    fMediumMagboltz->EnablePenningTransfer(rPenning, lambdaPenning, "ar");

    // Arquivo .gas com informações já calculadas do gás

    fMediumMagboltz->LoadGasFile("ar_90_ch4_10_1atm.gas");

    // Definição dos campos elétricos

    auto fComponentAnalyticField = new Garfield::ComponentAnalyticField();
    fComponentAnalyticField->SetMedium(fMediumMagboltz);

    // Definição do Ânodo de Ouro

    fComponentAnalyticField->AddWire(0., 0., 2 * Dim::raiofioouro/1, Dim::potencialanodo/1, "Anodo");

    // Definição do Cátodo Interno

    for (int i = 0; i < Dim::n; i++) {
        std::stringstream valori;
        valori << i;
        std::string icorrig;
        valori >> icorrig;
        fComponentAnalyticField->AddWire(Dim::distcatint*cos(i*Dim::twopi/60.)/1,
        Dim::distcatint*sin(i*Dim::twopi/60.)/1,
        2*Dim::raio/1,
        Dim::potencialcatint/1,
        "CatInt"+icorrig);
    }

    // Definição do Cátodo Externo

    for (int i = 0; i < Dim::n; i++) {
        std::stringstream valori;
        valori << i;
        std::string icorrig;
        valori >> icorrig;
        fComponentAnalyticField->AddWire(Dim::distcatext*cos(i*Dim::twopi/60.)/1,
        Dim::distcatext*sin(i*Dim::twopi/60.)/1,
        2*Dim::raio/1,
        Dim::potencialcatext/1,
        "CatExt"+icorrig);
    }

    // Definição do Cátodo de Alumínio

    fComponentAnalyticField->AddTube((Dim::raioextal+Dim::raiointal)/(2*1), 
        Dim::potencialal, 0, "Aluminio");

    // Definição do ponto de leitura de dados

    fComponentAnalyticField->AddReadout("Anodo");

    // Criação de um sensor

    auto fSensor = new Garfield::Sensor();
    fSensor->AddComponent(fComponentAnalyticField);
    fSensor->SetArea();

    // Utilização do método de transporte TrackHeed

    auto fTrackHeed = new Garfield::TrackHeed();
    fTrackHeed->SetSensor(fSensor);
    fTrackHeed->EnableDeltaElectronTransport();
    fTrackHeed->EnableElectricField();
    fTrackHeed->EnablePhotonReabsorption();

    // Definição dos arquivos de saída de dados
                                
    std::ofstream xFile ("AvaPEle.tsv", std::ofstream::app);
    std::ofstream yFile ("ElePPhot.tsv", std::ofstream::app);
    std::ofstream zFile ("AvaTot.tsv", std::ofstream::app);
    std::ofstream aFile ("EleEner.tsv", std::ofstream::app);
    std::ofstream bFile ("ElePos.tsv", std::ofstream::app);
    std::ofstream cFile ("EEnerPos.tsv", std::ofstream::app);

    // Definição do método de transporte Avalanche Macroscopic

    Garfield::AvalancheMC drift;
    drift.SetSensor(fSensor);
    drift.SetDistanceSteps(1.e-4);

    // Definição do método de transporte Avalanche Microscopic

    Garfield::AvalancheMicroscopic avalanche;
    avalanche.SetSensor(fSensor);
    avalanche.DisableAvalancheSizeLimit();

    // Raio do Ânodo

    const double rWire = Dim::raiofioouro/1 ;

    // Raio do Cátodo de Alumínio

    const double rTube = (Dim::raioextal+Dim::raiointal)/(2*1 );

    // Comprimento do Detector

    const double lTube = Dim::comprimento/1 ;

    // Definição de variáveis temporárias

    double xe1, ye1, ze1, te1, ee1;
    double xe2, ye2, ze2, te2, ee2;
    int avalsize;
    int nc;
    double theta, phi;
    int status;
    double xe, ye, ze, te;
    double ee, dxe, dye, dze;

    // Energia dos elétrons

    const double egamma = 200;

    const int nEvents = std::stoi(argv[1]);
    
    for (unsigned int i = 0; i < nEvents; ++i) {
        if (i % 1 == 0) std::cout << i << "/"<< nEvents<<"\n";

        avalsize = 0;

        nc = 0;        
        
        fTrackHeed->TransportDeltaElectron(Dim::raiointal*sqrt(2)/2 - 0.1, 
            Dim::raiointal*sqrt(2)/2-0.1, 0, 0, egamma, -1,-1,0,nc);
        
        std::cout << nc << " Elétrons produzidos\n";
    
        for (int cl = 0; cl < nc; cl++) {
            
            fTrackHeed->GetElectron(cl, xe, ye, ze, te, ee, dxe, dye, dze);

            std::cout << "Simulando Elétron nº " << cl<<"\n";

            if (ze < lTube && ze > -lTube && sqrt(xe * xe + ye * ye) < rTube) {        

                //drift.DriftElectron(xe, ye, ze, te)

            
                avalanche.DriftElectron(xe,ye,ze,te,ee,dxe,dye,dze);
                avalanche.GetElectronEndpoint(0,xe1,ye1,ze1,te1,ee1,xe2, 
                    ye2,ze2,te2,ee2,status);
                std::cout << "Distância percorrida: "<<
                    sqrt((xe1-xe2)*(xe1-xe2)+(ye1-ye2)*(ye1-ye2)+(ze1-ze2)*(ze1-ze2)) << " cm\n";
                //drift.GetElectronEndpoint(0, xe1, ye1, ze1, te1, xe2, ye2, ze2, te2, status );

                if (0 <= xe2 && xe2 <= rWire) {
                xe2 += rWire;
                } else if (0 >= xe2 && xe2 >= -rWire) {
                xe2 += -rWire;
                } 
                if (0 <= ye2 && ye2 <= rWire) {
                ye2 += rWire;
                } else if (0 >= ye2 && ye2 >= -rWire) {
                ye2 += -rWire;
                }

                //double e2 = 0.1;

                std::cout << "Simulando Avalanche do Elétron\n";

                avalanche.AvalancheElectron(xe2, ye2, ze2, te2, ee2);

                int ne = 0, ni = 0;

                avalanche.GetAvalancheSize(ne, ni);

                int Ne = avalanche.GetNumberOfElectronEndpoints();

                for (int j = 0;j<Ne;j++){
                    avalanche.GetElectronEndpoint(j,xe1,ye1,ze1,te1,ee1,xe2,ye2,
                        ze2,te2,ee2,status);
                
                    double r = sqrt(xe2*xe2 + ye2*ye2 + ze2*ze2);
                    aFile << ee2 << "\n";
                    bFile << r/Dim::raiofioouro << "\n";
                    cFile << ee2 << "\t" << r/Dim::raiofioouro<< "\n";
                }
                if (ne > 0)xFile << ne << "\n";
                if (ne > 1) avalsize = avalsize + ne;
            }
        }
        if (avalsize > 0)zFile << avalsize << "\n";
        yFile << nc << "\n";
    }
}
