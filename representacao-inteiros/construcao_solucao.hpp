#pragma once
 
#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

#include "leitura.hpp"
#include "solucao.hpp"


// Gerador global Mersenne Twister usado pelas etapas aleatórias do algoritmo.
inline std::mt19937 num_aleatorio;

inline void setSeed(unsigned int seed) {
    num_aleatorio.seed(seed);
}


inline Solucao construcaoGRASP(double alfa) {
    /*
    Constrói uma solução inicial viável utilizando uma estratégia gulosa
    aleatorizada.

    A solução é representada como uma lista de inteiros, em que cada inteiro
    indica uma coluna escolhida. A construção adiciona colunas à lista até que
    todas as linhas estejam cobertas.

    A razão usada para avaliar cada coluna candidata é:

        custo da coluna / quantidade de novas linhas cobertas

    O parâmetro alfa controla o nível de aleatoriedade da construção:
    - alfa próximo de 0: comportamento mais guloso;
    - alfa próximo de 1: comportamento mais aleatório.
    */

    Solucao sol;
    std::vector<bool> cobertas(N_LINHAS + 1, false);
    int total_cobertas = 0;

    while (total_cobertas < N_LINHAS) {
        std::vector<int> candidatos;
        std::vector<double> razoes;

        // Calcula a razão para cada coluna que ainda cobre alguma linha nova.
        for (int j = 0; j < N_COLUNAS; j++) {
            if (sol.contemColuna(j)) {
                continue;
            }

            int novas_linhas = 0;
            for (int linha : cobertura[j]) {
                if (!cobertas[linha]) {
                    novas_linhas++;
                }
            }

            if (novas_linhas == 0) {
                continue;
            }

            candidatos.push_back(j);
            razoes.push_back(custo[j] / novas_linhas);
        }

        if (candidatos.empty()) {
            break;
        }

        double razao_min = *std::min_element(razoes.begin(), razoes.end());
        double razao_max = *std::max_element(razoes.begin(), razoes.end());
        double limite = razao_min + alfa * (razao_max - razao_min);

        // Monta a Lista Restrita de Candidatos (RLC).
        std::vector<int> rlc;
        for (int k = 0; k < static_cast<int>(candidatos.size()); k++) {
            if (razoes[k] <= limite) {
                rlc.push_back(candidatos[k]);
            }
        }

        // Escolhe aleatoriamente uma coluna da RLC.
        std::uniform_int_distribution<int> indice_aleatorio(
            0,
            static_cast<int>(rlc.size()) - 1
        );

        int escolhido = rlc[indice_aleatorio(num_aleatorio)];

        // Adiciona a coluna escolhida à solução.
        sol.adicionarColuna(escolhido);

        // Atualiza as linhas cobertas pela coluna escolhida.
        for (int linha : cobertura[escolhido]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;
            }
        }
    }
    
    sol.recalcularCusto();
    return sol;
}
