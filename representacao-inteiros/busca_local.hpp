#pragma once

#include <vector>

#include "leitura.hpp"
#include "solucao.hpp"


inline std::vector<int> calcularCobertura(const Solucao& sol) {
    /*
    Calcula quantas colunas selecionadas cobrem cada linha.

    Como a solução agora é uma lista de inteiros, percorremos somente as colunas
    escolhidas, e não todas as colunas da instância.
    */
    std::vector<int> contagem(N_LINHAS + 1, 0);

    for (int coluna : sol.colunas_escolhidas) {
        for (int linha : cobertura[coluna]) {
            contagem[linha]++;
        }
    }

    return contagem;
}


inline bool ehRedundante(int coluna, const std::vector<int>& contagem) {
    /*
    Verifica se uma coluna é redundante.

    Uma coluna é redundante quando todas as linhas que ela cobre continuariam
    cobertas mesmo se ela fosse removida da solução.
    */
    for (int linha : cobertura[coluna]) {
        if (contagem[linha] <= 1) {
            return false;
        }
    }

    return true;
}


inline void removerRedundantes(Solucao& sol) {
    /*
    Remove colunas redundantes da solução.

    A vizinhança considerada é formada por soluções obtidas removendo uma coluna
    selecionada sem perder a cobertura de nenhuma linha. A cada iteração, remove-se
    a coluna redundante de maior custo.
    */

    std::vector<int> contagem = calcularCobertura(sol);

    while (true) {
        int melhor_coluna = -1;
        double maior_custo = -1.0;

        // Procura, entre as colunas escolhidas, a redundante mais cara.
        for (int coluna : sol.colunas_escolhidas) {
            if (ehRedundante(coluna, contagem) && custo[coluna] > maior_custo) {
                melhor_coluna = coluna;
                maior_custo = custo[coluna];
            }
        }

        if (melhor_coluna == -1) {
            break;
        }

        // Remove a coluna e atualiza a contagem de cobertura.
        sol.removerColuna(melhor_coluna);

        for (int linha : cobertura[melhor_coluna]) {
            contagem[linha]--;
        }
    }

    sol.recalcularCusto();
}


inline std::vector<bool> saoCobertas(const Solucao& sol, int& total_cobertas) {
    /*
    Identifica quais linhas estão cobertas pela solução e retorna o total de
    linhas cobertas. Essa função é usada no reparo de soluções inviáveis.
    */
    std::vector<bool> cobertas(N_LINHAS + 1, false);
    total_cobertas = 0;

    for (int coluna : sol.colunas_escolhidas) {
        for (int linha : cobertura[coluna]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;
            }
        }
    }

    return cobertas;
}


inline void repararSolucao(Solucao& sol) {
    /*
    Repara uma solução inviável de forma gulosa.

    Enquanto existir linha descoberta, adiciona-se a coluna com melhor razão:

        custo da coluna / quantidade de novas linhas cobertas
    */
    int total_cobertas = 0;
    std::vector<bool> cobertas = saoCobertas(sol, total_cobertas);
    std::vector<bool> escolhida(N_COLUNAS, false);

    for (int coluna : sol.colunas_escolhidas) {
        escolhida[coluna] = true;
    }

    while (total_cobertas < N_LINHAS) {
        int melhor_coluna = -1;
        double melhor_razao = 1e18;

        for (int j = 0; j < N_COLUNAS; j++) {
            if (escolhida[j]) {
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
            sol.recalcularCusto();
            return;
        }

        sol.adicionarColuna(melhor_coluna);
        escolhida[melhor_coluna] = true;

        for (int linha : cobertura[melhor_coluna]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;
            }
        }
    }

    sol.recalcularCusto();
}


inline void buscaLocal(Solucao& sol) {
    /*
    Busca local aplicada após a mutação e antes da atualização da população.

    Primeiro, a solução é reparada caso esteja inviável. Depois, remove-se
    colunas redundantes para reduzir o custo total.
    */
    if (!sol.ehViavel()) {
        repararSolucao(sol);
    }

    if (!sol.ehViavel()) {
        return;
    }

    removerRedundantes(sol);
}
