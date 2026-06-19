#pragma once
 
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>
#include "leitura.hpp"
#include "solucao.hpp"


// gerador global Mersenne Twister
std::mt19937 num_aleatorio;

void setSeed(int seed) {
    num_aleatorio.seed(seed);
}


Solucao construcaoGRASP(double alfa){
    Solucao sol;
    std::vector<bool> cobertas(N_LINHAS + 1, false);
    int total_cobertas = 0;

    while (total_cobertas < N_LINHAS) {
        // calcula razão para cada coluna ainda não utilizada
        std::vector<int> candidatos;
        std::vector<float> razoes;

        for (int j = 0; j < N_COLUNAS; j++) {
            if (sol.colunas_escolhidas.test(j)) {
                continue;
            }

            int novas = 0;
            for (int linha : cobertura[j]) {
                if (!cobertas[linha]) {
                    novas = novas + 1;
                }
            }

            if (novas == 0) {
                continue;
            }

            candidatos.push_back(j);
            razoes.push_back(custo[j]/novas);
        }

        if (candidatos.empty()) {
            break;
        }

        // encontra menor (melhor) e maior (pior) razão
        float razao_min = *std::min_element(razoes.begin(), razoes.end());
        float razao_max = *std::max_element(razoes.begin(), razoes.end());


        // RLC: candidatos com razão < (min + alfa * (max - min))
        std::vector<int> rlc;
        for (int k = 0; k < (int)candidatos.size(); k++) {
            if (razoes[k] <= (razao_min + alfa * (razao_max - razao_min))) {
                rlc.push_back(candidatos[k]);
            }
        }

        // escolha aleatória de um índice da RLC
        std::uniform_int_distribution indice_aleatorio(0, (int)rlc.size() - 1);
        int escolhido = rlc[indice_aleatorio(num_aleatorio)];

        // adiciona à solução
        sol.colunas_escolhidas.set(escolhido);
        sol.custo_total = sol.custo_total + custo[escolhido];
        for (int linha : cobertura[escolhido]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas = total_cobertas + 1;
            }
        }
    }
    
    return sol;
}