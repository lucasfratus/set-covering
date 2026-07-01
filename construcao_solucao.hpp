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
    std::vector<bool> escolhida(N_COLUNAS, false);
    std::vector<int> novas_linhas(N_COLUNAS, 0);

    int total_cobertas = 0;

    for (int j = 0; j < N_COLUNAS; j++) {
        novas_linhas[j] = static_cast<int>(cobertura[j].size());
    }

    while (total_cobertas < N_LINHAS) {
        std::vector<int> candidatos;
        std::vector<double> razoes;

        for (int j = 0; j < N_COLUNAS; j++) {
            if (escolhida[j] || novas_linhas[j] == 0) {
                continue;
            }

            candidatos.push_back(j);
            razoes.push_back(custo[j] / static_cast<double>(novas_linhas[j]));
        }

        if (candidatos.empty()) {
            break;
        }

        double razao_min = *std::min_element(razoes.begin(), razoes.end());
        double razao_max = *std::max_element(razoes.begin(), razoes.end());
        double limite = razao_min + alfa * (razao_max - razao_min);

        std::vector<int> rlc;

        for (int k = 0; k < static_cast<int>(candidatos.size()); k++) {
            if (razoes[k] <= limite) {
                rlc.push_back(candidatos[k]);
            }
        }

        std::uniform_int_distribution<int> indice_aleatorio(
            0,
            static_cast<int>(rlc.size()) - 1
        );

        int escolhido = rlc[indice_aleatorio(num_aleatorio)];

        sol.adicionarColuna(escolhido);
        escolhida[escolhido] = true;
        novas_linhas[escolhido] = 0;

        for (int linha : cobertura[escolhido]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;

                /*
                Como essa linha acabou de ser coberta, todas as colunas que
                cobrem essa linha passam a cobrir uma linha nova a menos.
                */
                for (int coluna_afetada : colunas_por_linha[linha]) {
                    if (!escolhida[coluna_afetada] && novas_linhas[coluna_afetada] > 0) {
                        novas_linhas[coluna_afetada]--;
                    }
                }
            }
        }
    }

    sol.recalcularCusto();
    return sol;
}