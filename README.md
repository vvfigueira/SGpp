# SGpp
  De forma similar aos projetos [SG4](https://github.com/vvfigueira/SG4) e [SG4-Gpp](https://github.com/vvfigueira/SG4-Gpp), este projeto trata-se da simulação de um detector gasoso, porém, apenas é utilizado aqui a parte do código referente as bibliotecas [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/).
## Requisitos
  É necessário uma instalação de Garfield++ e todas suas dependências.
## Compilação
  Compilação segue o método padrão de `cmake [path_to_CMakeLists.txt]` seguido de `make`.
  
  É possível ainda compilar e rodar a simulação com HTCondor, desde que instalado o Garfield++ na pasta `/cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/share/Garfield/setupGarfield.sh`, para submeter um job basta utilizar o arquivo comp.sub.
## Uso
  Após a compilação, será criado um arquivo binário `build`, para executá-lo basta usar `./build [Numero_de_Runs]`.
## Características
  Atualmente o projeto está definido como um cilindro de alumínio completo de uma mistura de gás Argônio(70%) com CO2(30%), 1 atm de pressão e 300 K de temperatura, em seu interior, à distâncias de 96 mm e 20 mm do centro do detector estâo posicionados respectivamente os cátodos externo e interno, sendo ambos compostos de 60 fios distribuídos uniformemente pela circunferência. No centro temos o ânodo de ouro. O campo elétrico está definido pelo Garfield++ com os potencias de cada Cátodo no arquivo Constantes.hh. Os métodos de avalanche e drift utilizados são da classe AvalancheMicroscopic. O transporte geral de elétrons delta é realizado pelo TrackHeed. Os arquivos de saída da simulação são `AvaPEle.tsv`, `AvaPTemp.tsv`, `AvaTot.tsv`, `EleEner.tsv`, `ElePos.tsv` e `EEnerPos.tsv`; são respectivamente arquivos de Tamanho da avalanche por elétron, por temperatura, avalanche total da run, energia final dos elétrons, posição final dos elétrons e Energia dos elétrons por posição.
