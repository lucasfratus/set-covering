#pragma once

#include <vector>
#include "leitura.hpp"
#include "solucao.hpp"


std::vector<int> calcularCobertura(const Solucao& sol) {
    /*
    Calcula a quantidade de vezes que cada linha está coberta pela solução.
    */
    std::vector<int> contagem(N_LINHAS + 1, 0);

    for (int j = 0; j < N_COLUNAS; j++) {
        if (!sol.colunas_escolhidas.test(j)) {
            continue;
        }

        for (int linha : cobertura[j]) {
            contagem[linha]++;
        }
    }
    return contagem;
}

bool ehRedundante(int coluna, const std::vector<int>& contagem) {
    /*
    Verifica se uma coluna selecionada é redundante. Uma coluna é redundante se todas as linhas que ela
    cobre já estão cobertas por outras colunas selecionadas.
    */
    for (int linha : cobertura[coluna]) {
        if (contagem[linha] <= 1) {
            return false;
        }
    }

    return true;
}


void removerRedundantes(Solucao& sol) {
    /* 
    Remove colunas redundantes.
    */

    std::vector<int> contagem(N_LINHAS + 1, 0);

    // Conta quantas colunas selecionadas cobrem cada linha.
    for (int j = 0; j < N_COLUNAS; j++) {
        if (sol.colunas_escolhidas.test(j)) {
            for (int linha : cobertura[j]) {
                contagem[linha]++;
            }
        }
    }

    while (true) {
        int melhor_coluna = -1;
        double maior_custo = -1.0;

        // Procura a coluna redundante mais cara.
        for (int j = 0; j < N_COLUNAS; j++) {
            if (!sol.colunas_escolhidas.test(j)) {
                continue;
            }
            bool redundante = true;
            for (int linha : cobertura[j]) {
                if (contagem[linha] <= 1) {
                    redundante = false;
                    break;
                }
            }
            if (redundante && custo[j] > maior_custo) {
                melhor_coluna = j;
                maior_custo = custo[j];
            }
        }

        // Se nenhuma coluna redundante foi encontrada, termina.
        if (melhor_coluna == -1) {
            break;
        }

        // Remove a coluna redundante escolhida.
        sol.colunas_escolhidas.reset(melhor_coluna);

        // Atualiza a contagem das linhas que essa coluna cobria.
        for (int linha : cobertura[melhor_coluna]) {
            contagem[linha]--;
        }
    }

    sol.recalcularCusto();
}


std::vector<bool> saoCobertas(const Solucao& sol, int& total_cobertas) {
    /*
    Identifica quais linhas estão cobertas pela solução.
    Guarda apenas uma informação booleana para cada linha.

    Também retorna o total de linhas cobertas.
    Essa função é usada principalmente no reparo de soluções inviáveis.
    */
    std::vector<bool> cobertas(N_LINHAS + 1, false);
    total_cobertas = 0;

    for (int j = 0; j < N_COLUNAS; j++) {
        if (!sol.colunas_escolhidas.test(j)) {
            continue;
        }

        for (int linha : cobertura[j]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;
            }
        }
    }

    return cobertas;
}


void repararSolucao(Solucao& sol) {
    int total_cobertas = 0;
    std::vector<bool> cobertas = saoCobertas(sol, total_cobertas);

    while (total_cobertas < N_LINHAS) {
        int melhor_coluna = -1;
        double melhor_razao = 1e18;

        for (int j = 0; j < N_COLUNAS; j++) {
            if (sol.colunas_escolhidas.test(j)) {
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

            double razao = custo[j] / novas_linhas;
            if (razao < melhor_razao) {
                melhor_razao = razao;
                melhor_coluna = j;
            }
        }

        if (melhor_coluna == -1) {
            return;
        }

        sol.colunas_escolhidas.set(melhor_coluna);
        sol.custo_total += custo[melhor_coluna];

        for (int linha : cobertura[melhor_coluna]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;
            }
        }
    }

    sol.recalcularCusto();
}


void buscaLocal(Solucao& sol) {
    /*
    A técnica utilizada é a de remoção de colunas redundantes.
    A vizinhança é formada por soluçoes obtidas removendo uma coluna selecionada
    sem perder a cobertura de nenhuma linha.
    Como a mutação pode gerar soluções inviáveis, é feito o reparo da solução de forma gulosa.
    */
    if (!sol.ehViavel()) {
        repararSolucao(sol);
    }

    if (!sol.ehViavel()) {
        return;
    }

    removerRedundantes(sol);
    sol.recalcularCusto();

}