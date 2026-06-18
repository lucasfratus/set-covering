#pragma once

#include <vector>

#include "solucao.hpp"

struct Parametros {
    int tamanho_populacao = 50;
    int max_geracoes = 100;
    int n_torneio = 4;
    int n_elite = 2;
    double alfa_grasp = 0.50;
    double taxa_crossover = 0.90;
    double taxa_mutacao = 0.005;
    bool aplicar_bl_pop_inicial = true;
};


std::vector<Solucao> gerarPopulacaoInicial(const Parametros& parametros) {
    std::vector<Solucao> populacao;
    populacao.reserve(parametros.tamanho_populacao);  // reserva espaço para o vetor de população.
    
    for (int i = 0; i < parametros.tamanho_populacao; i++) {
        Solucao individuo = construcaoGRASP(parametros.alfa_grasp);

        if (parametros.aplicar_bl_pop_inicial) {
            buscaLocal(individuo);
        }

        populacao.push_back(individuo);
    }

    return populacao
}


const Solucao& selecionarPorTorneio(const std::vector<Solucao>& populacao, int n, std::mt19937& num_aleatorio) {
    std::uniform_int_distribution<int> indice_aleatorio(0, ((int)populacao.size() - 1));

    int melhor_indice = indice_aleatorio(num_aleatorio)

    for (int = 1; i < n; i++) {
        int candidato = indice_aleatorio(num_aleatorio);
        if (candidato.custo_total < melhor_indice.custo_total) {
            melhor_indice = candidato;
        }
    }

    return populacao[melhor_indice]
}


Solucao realizaCrossover(const Solucao& pai1, const Solucao& pai2, std::mt19937& num_aleatorio) {
    Solucao filho;

        Solucao filho;

    std::uniform_int_distribution<int> moeda(0, 1);

    for (int j = 0; j < N_COLUNAS; j++) {
        int sorteio = moeda(num_aleatorio);

        if (sorteio == 0) {
            filho.colunas_escolhidas[j] = pai1.colunas_escolhidas[j];
        } else {
            filho.colunas_escolhidas[j] = pai2.colunas_escolhidas[j];
        }
    }

    filho.custo_total = calcularCustoSolucao(filho);

    return filho;
}


