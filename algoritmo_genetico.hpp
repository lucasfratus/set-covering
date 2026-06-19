#pragma once

#include <vector>

#include "solucao.hpp"

struct Parametros {
    int tamanho_populacao = 0;
    int max_geracoes = 0;
    int n_torneio = 0;
    int n_elite = 0;
    double alfa_grasp = 0.00;
    double taxa_crossover = 0.00;
    double taxa_mutacao = 0.00;
    bool aplicar_bl_pop_inicial = true;
};


struct Resultado {
    Solucao melhor;
    int geracao_do_melhor = 0;
};


bool melhorQue(const Solucao& a, const Solucao& b) {
    // serve como uma função lambda para a função "sort()" do c++
    return a.custo_total < b.custo_total;
}


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

    return populacao;
}


const Solucao& selecionarPorTorneio(const std::vector<Solucao>& populacao, int n, std::mt19937& num_aleatorio) {
    std::uniform_int_distribution<int> indice_aleatorio(0, ((int)populacao.size() - 1));

    int melhor_indice = indice_aleatorio(num_aleatorio);

    for (int i = 1; i < n; i++) {
        int candidato = indice_aleatorio(num_aleatorio);
        if (populacao[candidato].custo_total < populacao[melhor_indice].custo_total) {
            melhor_indice = candidato;
        }
    }

    return populacao[melhor_indice];
}


Solucao realizaCrossover(const Solucao& pai1, const Solucao& pai2, std::mt19937& num_aleatorio) {
    Solucao filho;
    std::uniform_int_distribution<int> valor(0, 1);

    for (int j = 0; j < N_COLUNAS; j++) {
        int sorteio = valor(num_aleatorio);

        if (sorteio == 0) {
            filho.colunas_escolhidas[j] = pai1.colunas_escolhidas[j];
        } else {
            filho.colunas_escolhidas[j] = pai2.colunas_escolhidas[j];
        }
    }

    filho.recalcularCusto();
    return filho;
}


void mutacao(Solucao& individuo, double taxa_mutacao, std::mt19937& num_aleatorio) {
    std::uniform_int_distribution<int> valor(1, 10000);

    for (int j = 0; j < N_COLUNAS; j++) {
        int sorteio = valor(num_aleatorio);

        if (sorteio <= taxa_mutacao * 10000) {
            individuo.colunas_escolhidas.flip(j);
        }
    }

    individuo.recalcularCusto();
}


std::vector<Solucao> atualizarPopulacao(std::vector<Solucao> populacao_atual, 
    std::vector<Solucao> filhos, int n_elite, int tamanho_populacao) {
    
    std::sort(populacao_atual.begin(), populacao_atual.end(), melhorQue);
    std::sort(filhos.begin(), filhos.end(), melhorQue);

    std::vector<Solucao> nova_populacao;
    nova_populacao.reserve(tamanho_populacao);

    int elite = std::min(n_elite, (int)(populacao_atual.size()));
    for (int i = 0; i < elite && (int)(nova_populacao.size()) < tamanho_populacao; i++) {
        nova_populacao.push_back(populacao_atual[i]);
    }

    for (const Solucao& filho : filhos) {
        if ((int)(nova_populacao.size()) >= tamanho_populacao) {
            break;
        }
        nova_populacao.push_back(filho);
    }

    // caso gere menos filhos do que o esperado
    for (const Solucao& individuo : populacao_atual) {
        if ((int)(nova_populacao.size()) >= tamanho_populacao) {
            break;
        }
        nova_populacao.push_back(individuo);
    }
    
    return nova_populacao;
}


Resultado executarAlgoritmoGenetico(const Parametros& parametros) {
    std::vector<Solucao> populacao = gerarPopulacaoInicial(parametros);
    std::sort(populacao.begin(), populacao.end(), melhorQue);

    Resultado resultado;
    resultado.melhor = populacao.front();
    resultado.geracao_do_melhor = 0;

    std::uniform_int_distribution<int> sorteio_porcentagem(1, 100);

    for (int geracao = 1; geracao <= parametros.max_geracoes; geracao++) {
        std::vector<Solucao> filhos;
        filhos.reserve(parametros.tamanho_populacao);

        while (static_cast<int>(filhos.size()) < parametros.tamanho_populacao) {
            const Solucao& pai1 = selecionarPorTorneio(
                populacao,
                parametros.n_torneio,
                num_aleatorio
            );

            const Solucao& pai2 = selecionarPorTorneio(
                populacao,
                parametros.n_torneio,
                num_aleatorio
            );

            Solucao filho1;
            Solucao filho2;

            int sorteio = sorteio_porcentagem(num_aleatorio);

            if (sorteio <= parametros.taxa_crossover * 100) {
                filho1 = realizaCrossover(pai1, pai2, num_aleatorio);
                filho2 = realizaCrossover(pai2, pai1, num_aleatorio);
            } else {
                filho1 = pai1;
                filho2 = pai2;
            }

            mutacao(filho1, parametros.taxa_mutacao, num_aleatorio);
            buscaLocal(filho1);

            if (filho1.ehViavel()) {
                filhos.push_back(filho1);
            }

            if (static_cast<int>(filhos.size()) < parametros.tamanho_populacao) {
                mutacao(filho2, parametros.taxa_mutacao, num_aleatorio);
                buscaLocal(filho2);

                if (filho2.ehViavel()) {
                    filhos.push_back(filho2);
                }
            }
        }

        populacao = atualizarPopulacao(
            populacao,
            filhos,
            parametros.n_elite,
            parametros.tamanho_populacao
        );

        std::sort(populacao.begin(), populacao.end(), melhorQue);

        if (melhorQue(populacao.front(), resultado.melhor)) {
            resultado.melhor = populacao.front();
            resultado.geracao_do_melhor = geracao;
        }
    }

    return resultado;
}