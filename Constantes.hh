#ifndef Constantes_h
#define Constantes_h 1

namespace Dim{

    static constexpr int n = 60; // Número de fios nos Cátodos
    static constexpr double twopi = M_PI*2;
    static constexpr double comprimento = 0.6*100; // Comprimento do Detector
    static constexpr double raio = 0.025*0.1; // Raio dos fios de Nicrômio
    static constexpr double distcatint = 20.003*0.1; // Distância, em relação ao Ânodo de Ouro, do Cátodo Interno
    static constexpr double distcatext = 96.003*0.1; // Distância, e100 relação ao Ânodo de Ouro, do Cátodo Externo
    static constexpr double raiofioouro = raio/2; // Raio do Fio de Ouro
    static constexpr double raiointal = 10.5*1; // Raio Interno Alumínio
    static constexpr double raioextal = 10.66*1; // Raio Externo Alumínio
    static constexpr double pressaogas = 760.*1; // Pressão do Gás
    static constexpr double potencialanodo = 2500*1; // Potencial do Ânodo
    static constexpr double potencialcatint = 750*1; // Potencial do Cátodo Interno
    static constexpr double potencialcatext = 20*1; // Potencial do Cátodo Externo
    static constexpr double potencialal = 0*1; // Potencial do Cátodo de Alumínio
    static constexpr double temperaturagas = 293.15*1; // Temperatura do Gás
}

#endif