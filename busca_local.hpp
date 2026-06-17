#pragma once

#include <vector>
#include "instancia.hpp"
#include "solucao.hpp"


void remocaoRedundante(Solucao& sol) {
    /* 
    Remove uma coluna redundante.
    Uma coluna é redundante se todas as linhas que ela
    cobre já estão cobertas por outras colunas selecionadas.
    */

    // conta quantas colunas selecionadas cobrem cada linha
    std::vector<int> contagem(N_LINHAS + 1, 0);
    for (int j = 0; j < N_COLUNAS; j++)
        if (sol.colunas_escolhidas.test(j))
            for (int linha : cobertura[j])
                contagem[linha]++;

    // tenta remover cada coluna selecionada
    for (int j = 0; j < N_COLUNAS; j++) {
        if (!sol.colunas_escolhidas.test(j)) {
            continue;
        } 
        bool redundante = true;

        for (int linha : cobertura[j]) {
            if (contagem[linha] <= 1) { 
                redundante = false; break; 
            }
        }
        
        if (redundante) {
            sol.colunas_escolhidas.reset(j);
            sol.custo_total -= custo[j];
            for (int linha : cobertura[j]) {
                contagem[linha]--;
            }
        }
    }
}