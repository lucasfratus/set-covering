#pragma once

#include <algorithm>
#include <vector>

#include "leitura.hpp"
#include "solucao.hpp"


inline constexpr double EPS_BUSCA_LOCAL = 1e-6;


inline std::vector<int> calcularCobertura(const Solucao& sol) {
    /*
    Calcula quantas colunas selecionadas cobrem cada linha.

    Como a solução é uma lista de inteiros, percorremos somente as colunas
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
    Remove colunas redundantes. É um procedimento de busca local.
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
    linhas cobertas. É utilizada no reparo de soluções inviáveis.
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
    std::vector<int> novas_linhas(N_COLUNAS, 0);

    for (int coluna : sol.colunas_escolhidas) {
        escolhida[coluna] = true;
    }

    // Calcula, uma única vez, quantas linhas ainda descobertas cada coluna cobre.
    for (int j = 0; j < N_COLUNAS; j++) {
        if (escolhida[j]) {
            continue;
        }

        int contador = 0;

        for (int linha : cobertura[j]) {
            if (!cobertas[linha]) {
                contador++;
            }
        }

        novas_linhas[j] = contador;
    }

    while (total_cobertas < N_LINHAS) {
        int melhor_coluna = -1;
        double melhor_razao = 1e18;

        for (int j = 0; j < N_COLUNAS; j++) {
            if (escolhida[j] || novas_linhas[j] == 0) {
                continue;
            }

            double razao = custo[j] / static_cast<double>(novas_linhas[j]);

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
        novas_linhas[melhor_coluna] = 0;

        for (int linha : cobertura[melhor_coluna]) {
            if (!cobertas[linha]) {
                cobertas[linha] = true;
                total_cobertas++;

                /*
                Essa linha não é mais "nova" para nenhuma coluna candidata.
                Atualiza somente as colunas que cobrem essa linha.
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
}


inline bool colunaCobreLinhasCriticas(int coluna_entra, const std::vector<bool>& linha_critica,
    int total_criticas ) {
    /*
    Verifica se coluna_entra cobre todas as linhas críticas de uma troca.

    Linha crítica é uma linha que ficaria descoberta caso a coluna escolhida
    para sair fosse removida. Para a troca ser viável, a coluna que entra deve
    cobrir todas essas linhas.
    */
    if (total_criticas == 0) {
        return true;
    }

    int cobertas = 0;

    for (int linha : cobertura[coluna_entra]) {
        if (linha_critica[linha]) {
            cobertas++;
        }
    }

    return cobertas == total_criticas;
}


inline bool aplicarTroca1Por1Restrita(Solucao& sol) {
    /*
    Segundo procedimento de busca local.
    Troca uma coluna selecionada por uma coluna não selecionada. A troca só é aceita se:

    1. a solução continua viável;
    2. o custo total diminui.

    Para controlar o tempo de execução, a troca é testada sobre a coluna mais
    cara da solução atual.
    */
    if (sol.colunas_escolhidas.empty()) {
        return false;
    }

    std::vector<int> contagem = calcularCobertura(sol);
    std::vector<bool> escolhida(N_COLUNAS, false);

    int coluna_sai = -1;
    double maior_custo = -1.0;

    for (int coluna : sol.colunas_escolhidas) {
        escolhida[coluna] = true;

        if (custo[coluna] > maior_custo) {
            maior_custo = custo[coluna];
            coluna_sai = coluna;
        }
    }

    if (coluna_sai == -1) {
        return false;
    }

    std::vector<bool> linha_critica(N_LINHAS + 1, false);
    int total_criticas = 0;

    for (int linha : cobertura[coluna_sai]) {
        if (contagem[linha] <= 1) {
            linha_critica[linha] = true;
            total_criticas++;
        }
    }

    int melhor_entra = -1;
    double melhor_delta = 0.0;

    for (int coluna_entra = 0; coluna_entra < N_COLUNAS; coluna_entra++) {
        if (escolhida[coluna_entra]) {
            continue;
        }

        double delta = custo[coluna_entra] - custo[coluna_sai];

        if (delta >= melhor_delta - EPS_BUSCA_LOCAL) {
            continue;
        }

        if (colunaCobreLinhasCriticas(coluna_entra, linha_critica, total_criticas)) {
            melhor_entra = coluna_entra;
            melhor_delta = delta;
        }
    }

    if (melhor_entra == -1) {
        return false;
    }

    sol.removerColuna(coluna_sai);
    sol.adicionarColuna(melhor_entra);
    sol.recalcularCusto();

    return true;
}


inline void buscaLocalBasica(Solucao& sol) {
    /*
    Busca local básica:
    1. repara a solução se estiver inviável;
    2. remove colunas redundantes.

    Essa etapa é aplicada em todos os filhos.
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


inline void buscaLocalComTrocaRestrita(Solucao& sol) {
    /*
    Busca local com troca 1-por-1 restrita.

    Essa etapa é mais intensiva e pode reduzir a diversidade da população,
    então deve ser aplicada apenas em uma parte dos filhos.
    */
    if (!sol.ehViavel()) {
        repararSolucao(sol);
    }

    if (!sol.ehViavel()) {
        return;
    }

    removerRedundantes(sol);

    const int max_movimentos = 1;
    int movimentos = 0;

    while (movimentos < max_movimentos && aplicarTroca1Por1Restrita(sol)) {
        removerRedundantes(sol);
        movimentos++;
    }

    sol.recalcularCusto();
}
