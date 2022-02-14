#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "colors.hh"

#include <TCanvas.h>
#include <TROOT.h>
#include "TApplication.h"

#include "Constantes.hh"

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

  if(argc == 1){std::cout <<"\n Uso: ./build [nElétrons]\n";return 0;};
  // Define the gas mixture.
  //auto cfield = new TCanvas("field","",600., 600.);
  //auto field = new Garfield::ViewField();
  auto fMediumMagboltz = new Garfield::MediumMagboltz();
  fMediumMagboltz->SetComposition("ar", 70., "co2", 30.);
  fMediumMagboltz->SetTemperature(Dim::temperaturagas/1);
  fMediumMagboltz->SetPressure(Dim::pressaogas/1);
  fMediumMagboltz->Initialise(true);
  // Set the Penning transfer efficiency.
  const double rPenning = 0.57;
  const double lambdaPenning = 0.;
  fMediumMagboltz->EnablePenningTransfer(rPenning, lambdaPenning, "ar");
  fMediumMagboltz->LoadGasFile("ar_70_co2_30_1000mbar.gas");

  auto fComponentAnalyticField = new Garfield::ComponentAnalyticField();
  fComponentAnalyticField->SetMedium(fMediumMagboltz);

  fComponentAnalyticField->AddWire(0., 0., 2 * Dim::raiofioouro/1, Dim::potencialanodo/1, "Anodo");

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

  // Add the tube.
  fComponentAnalyticField->AddTube((Dim::raioextal+Dim::raiointal)/(2*1), 
    Dim::potencialal, 0, "Aluminio");

  fComponentAnalyticField->AddReadout("Anodo");

  //field->SetComponent(fComponentAnalyticField);
  //field->SetPlaneXY();
  //field->SetCanvas(cfield);
  //field->PlotContour();

  auto fSensor = new Garfield::Sensor();
  fSensor->AddComponent(fComponentAnalyticField);
  fSensor->SetArea();
  /*fSensor->AddElectrode(fComponentAnalyticField, "Anodo");
  fSensor->SetTimeWindow(-0.25,0.5,1000);
  if (!readTransferFunction(fSensor)) G4cout << "Erro, arquivo ´TransferFunction´ não providenciado.\n";
  fSensor->ClearSignal();*/

  auto fTrackHeed = new Garfield::TrackHeed();
  fTrackHeed->SetSensor(fSensor);
  fTrackHeed->EnableDeltaElectronTransport();
  fTrackHeed->EnableElectricField();
  fTrackHeed->EnablePhotonReabsorption();

  /*TCanvas* cS = nullptr;
  if (true) {
    cS = new TCanvas("cS", "", 600, 600);
    signalView.SetCanvas(cS);
    signalView.SetSensor(fSensor);
    signalView.SetLabelY("signal [fC]");
  } */
                            
  std::ofstream xFile ("AvaPEle.txt", std::ofstream::app);
  std::ofstream yFile ("ElePPhot.txt", std::ofstream::app);
  std::ofstream zFile ("AvaTot.txt", std::ofstream::app);
  std::ofstream aFile ("EleEner.txt", std::ofstream::app);
  std::ofstream bFile ("ElePos.txt", std::ofstream::app);
  std::ofstream cFile ("EEnerPos.txt", std::ofstream::app);

  Garfield::AvalancheMC drift; // Usar sem modelo2
  // Garfield::DriftLineRKF drift; // Usar com modelo2
  drift.SetSensor(fSensor);
  //drift.SetGainFluctuationsPolya(0.4, 20000.); // Usar com modelo 2
  drift.SetDistanceSteps(1.e-4);// Usar sem modelo2
  Garfield::AvalancheMicroscopic avalanche;
  avalanche.SetSensor(fSensor);
  avalanche.DisableAvalancheSizeLimit();

  // Wire radius [cm]
  const double rWire = Dim::raiofioouro/1 ;
  // Outer radius of the tube [cm]
  const double rTube = (Dim::raioextal+Dim::raiointal)/(2*1 );
  // Half-length of the tube [cm]
  const double lTube = Dim::comprimento/1 ;

  /*ViewDrift driftView;
    avalanche.EnablePlotting(&driftView);
    fTrackHeed->EnablePlotting(&driftView);
    //drift.EnablePlotting(&driftView);

     driftView.SetPlane(0, 0, 1, 0, 0, 0);
      driftView.SetArea(-Dim::raioextal/2,-Dim::raioextal/2,Dim::raioextal/2, Dim::raioextal/2);
      driftView.SetCanvas(cd);
      driftView.Plot(true);*/

  double xe1, ye1, ze1, te1, ee1;
  double xe2, ye2, ze2, te2, ee2;
  int avalsize;
  int nc;
  double theta, phi;
  int status;
  double xe, ye, ze, te;
  double ee, dxe, dye, dze;
  const double egamma = 200;//r < 100. ? 5898.8 : r < 150. ? 5887.6 : 6490.4; 
  const int nEvents = std::stoi(argv[1]);
  for (unsigned int i = 0; i < nEvents; ++i) {
  if (i % 1 == 0) std::cout << i << "/"<< nEvents<<"\n";//"/" << nEvents << "\n";

  avalsize = 0;
  // Number of electrons produced in a collision
  nc = 0;
  // Energy loss in a collision
  // Dummy variable (not used at present)

  //theta = RndmUniform()*Dim::twopi/2;

  //phi = RndmUniform()*Dim::twopi;

  //double z = 0;

  //double rho = RndmUniform()*(Dim::raiointal-0.1);

  //const double r = 167. * RndmUniform();
  
  
      fTrackHeed->TransportDeltaElectron(Dim::raiointal*sqrt(2)/2 - 0.1, Dim::raiointal*sqrt(2)/2-0.1,
                                   0, 0, egamma, -1,-1,0,nc);
                                  std::cout << nc << " Elétrons produzidos\n";
  
    for (int cl = 0; cl < nc; cl++) {
      
      fTrackHeed->GetElectron(cl, xe, ye, ze, te, ee, dxe, dye, dze);
      std::cout << "Simulando Elétron nº " << cl<<"\n";
      if (ze < lTube && ze > -lTube && sqrt(xe * xe + ye * ye) < rTube) {        

        //drift.DriftElectron(xe, ye, ze, te)

       
        avalanche.DriftElectron(xe,ye,ze,te,ee,dxe,dye,dze);
        avalanche.GetElectronEndpoint(0,xe1,ye1,ze1,te1,ee1,xe2,ye2,ze2,te2,ee2,status);
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
          avalanche.GetElectronEndpoint(j,xe1,ye1,ze1,te1,ee1,xe2,ye2,ze2,te2,ee2,status);
          
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