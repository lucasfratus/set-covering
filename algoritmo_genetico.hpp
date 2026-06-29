#pragma once

#include <algorithm>
#include <random>
#include <vector>

#include "solucao.hpp"
#include "construcao_solucao.hpp"
#include "busca_local.hpp"

struct Parametros {
    int tamanho_populacao = 0;
    int max_geracoes = 0;
    int max_geracoes_sem_melhora = 0;
    int n_torneio = 0;
    int n_elite = 0;
    double alfa_grasp = 0.00;
    double taxa_crossover = 0.00;
    double taxa_mutacao = 0.00;
    double taxa_busca_troca = 0.20;
    bool aplicar_bl_pop_inicial = true;
};


struct Resultado {
    Solucao melhor;
    int geracao_do_melhor = 0;

    std::vector<double> historico_melhor_global;
    std::vector<double> historico_melhor_geracao;
    std::vector<double> historico_media_populacao;
};


inline bool melhorQue(const Solucao& a, const Solucao& b) {
    return a.custo_total < b.custo_total;
}


inline std::vector<Solucao> gerarPopulacaoInicial(const Parametros& parametros) {
    std::vector<Solucao> populacao;
    populacao.reserve(parametros.tamanho_populacao);
    
    for (int i = 0; i < parametros.tamanho_populacao; i++) {
        Solucao individuo = construcaoGRASP(parametros.alfa_grasp);

        if (parametros.aplicar_bl_pop_inicial) {
            buscaLocalBasica(individuo);
        }

        populacao.push_back(individuo);
    }

    return populacao;
}


inline const Solucao& selecionarPorTorneio(
    const std::vector<Solucao>& populacao,
    int n,
    std::mt19937& num_aleatorio
) {
    std::uniform_int_distribution<int> indice_aleatorio(
        0,
        static_cast<int>(populacao.size()) - 1
    );

    int melhor_indice = indice_aleatorio(num_aleatorio);

    for (int i = 1; i < n; i++) {
        int candidato = indice_aleatorio(num_aleatorio);

        if (melhorQue(populacao[candidato], populacao[melhor_indice])) {
            melhor_indice = candidato;
        }
    }

    return populacao[melhor_indice];
}


inline Solucao realizaCrossover(const Solucao& pai1, const Solucao& pai2, std::mt19937& num_aleatorio) {
    /*
    Realiza crosover baseado na união de soluções
    O filho recebe inicialmente as colunas comuns aos dois pais. As colunas que
    aparecem em apenas um dos pais são embaralhadas e adicionadas com uma
    probabilidade fixa, preservando diversidade.

    Caso o filho gerado fique inviável, aplica-se um reparo guloso da solução. Em seguida,
    as colunas redundantes são removidas.
    */
    Solucao filho;

    std::vector<bool> esta_no_pai1(N_COLUNAS, false);
    std::vector<bool> esta_no_pai2(N_COLUNAS, false);

    for (int coluna : pai1.colunas_escolhidas) {
        esta_no_pai1[coluna] = true;
    }

    for (int coluna : pai2.colunas_escolhidas) {
        esta_no_pai2[coluna] = true;
    }

    std::vector<int> diferentes;

    for (int j = 0; j < N_COLUNAS; j++) {
        if (esta_no_pai1[j] && esta_no_pai2[j]) {
            filho.adicionarColuna(j);
        } else if (esta_no_pai1[j] || esta_no_pai2[j]) {
            diferentes.push_back(j);
        }
    }

    for (int i = static_cast<int>(diferentes.size()) - 1; i > 0; i--) {
        std::uniform_int_distribution<int> indice_aleatorio(0, i);

        int j = indice_aleatorio(num_aleatorio);

        std::swap(diferentes[i], diferentes[j]);
    }

    std::uniform_int_distribution<int> sorteio_porcentagem(1, 100);

    for (int coluna : diferentes) {
        int sorteio = sorteio_porcentagem(num_aleatorio);

        if (sorteio <= 50) {
            filho.adicionarColuna(coluna);
        }
    }

    filho.recalcularCusto();

    if (!filho.ehViavel()) {
        repararSolucao(filho);
    }

    removerRedundantes(filho);

    return filho;
}


inline void mutacao(Solucao& individuo, double taxa_mutacao, std::mt19937& num_aleatorio) {
    /*
    Mutação para representação por lista de inteiros.

    A quantidade de alterações é calculada a partir da taxa de mutação e do
    número de colunas da instância. Cada alteração pode adicionar, remover ou
    trocar uma coluna da solução.
    */
    int quantidade_mutacoes = static_cast<int>(taxa_mutacao * N_COLUNAS);

    if (quantidade_mutacoes < 1) {
        quantidade_mutacoes = 1;
    }

    std::uniform_int_distribution<int> operacao(0, 2);
    std::uniform_int_distribution<int> coluna_aleatoria(0, N_COLUNAS - 1);

    for (int i = 0; i < quantidade_mutacoes; i++) {
        int op = operacao(num_aleatorio);

        // adiciona uma coluna aleatória.
        if (op == 0 || individuo.colunas_escolhidas.empty()) {
            int coluna = coluna_aleatoria(num_aleatorio);
            individuo.adicionarColuna(coluna);
        }

        // remove uma coluna escolhida aleatória.
        else if (op == 1) {
            std::uniform_int_distribution<int> indice_coluna(
                0,
                static_cast<int>(individuo.colunas_escolhidas.size()) - 1
            );

            int indice = indice_coluna(num_aleatorio);
            int coluna = individuo.colunas_escolhidas[indice];
            individuo.removerColuna(coluna);
        }

        // troca uma coluna escolhida por outra aleatória.
        else {
            std::uniform_int_distribution<int> indice_coluna(
                0,
                static_cast<int>(individuo.colunas_escolhidas.size()) - 1
            );

            int indice = indice_coluna(num_aleatorio);
            int coluna_removida = individuo.colunas_escolhidas[indice];
            int coluna_adicionada = coluna_aleatoria(num_aleatorio);

            individuo.removerColuna(coluna_removida);
            individuo.adicionarColuna(coluna_adicionada);
        }
    }

    individuo.recalcularCusto();
}


double calcularMediaPopulacao(const std::vector<Solucao>& populacao) {
    double soma = 0.0;

    for (const Solucao& sol : populacao) {
        soma += sol.custo_total;
    }

    return soma / populacao.size();
}


inline std::vector<Solucao> atualizarPopulacao(
    std::vector<Solucao> populacao_atual,
    std::vector<Solucao> filhos,
    int n_elite,
    int tamanho_populacao
) {
    std::sort(populacao_atual.begin(), populacao_atual.end(), melhorQue);
    std::sort(filhos.begin(), filhos.end(), melhorQue);

    std::vector<Solucao> nova_populacao;
    nova_populacao.reserve(tamanho_populacao);

    int elite = std::min(n_elite, static_cast<int>(populacao_atual.size()));
    for (int i = 0; i < elite && static_cast<int>(nova_populacao.size()) < tamanho_populacao; i++) {
        nova_populacao.push_back(populacao_atual[i]);
    }

    for (const Solucao& filho : filhos) {
        if (static_cast<int>(nova_populacao.size()) >= tamanho_populacao) {
            break;
        }
        nova_populacao.push_back(filho);
    }

    for (const Solucao& individuo : populacao_atual) {
        if (static_cast<int>(nova_populacao.size()) >= tamanho_populacao) {
            break;
        }
        nova_populacao.push_back(individuo);
    }
    
    return nova_populacao;
}


inline Resultado executarAlgoritmoGenetico(const Parametros& parametros) {
    std::vector<Solucao> populacao = gerarPopulacaoInicial(parametros);
    std::sort(populacao.begin(), populacao.end(), melhorQue);

    Resultado resultado;
    resultado.melhor = populacao.front();
    resultado.geracao_do_melhor = 0;

    int geracoes_sem_melhora = 0;

    resultado.historico_melhor_global.push_back(resultado.melhor.custo_total);
    resultado.historico_melhor_geracao.push_back(populacao.front().custo_total);
    resultado.historico_media_populacao.push_back(calcularMediaPopulacao(populacao));

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
            int sorteio_bl1 = sorteio_porcentagem(num_aleatorio);

            if (sorteio_bl1 <= parametros.taxa_busca_troca * 100) {
                buscaLocalComTrocaRestrita(filho1);
            } else {
                buscaLocalBasica(filho1);
            }


            if (filho1.ehViavel()) {
                filhos.push_back(filho1);
            }

            if (static_cast<int>(filhos.size()) < parametros.tamanho_populacao) {
                mutacao(filho2, parametros.taxa_mutacao, num_aleatorio);

                int sorteio_bl2 = sorteio_porcentagem(num_aleatorio);

                if (sorteio_bl2 <= parametros.taxa_busca_troca * 100) {
                    buscaLocalComTrocaRestrita(filho2);
                } else {
                    buscaLocalBasica(filho2);
                }
                

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
            geracoes_sem_melhora = 0;
        } else {
            geracoes_sem_melhora++;
        }

        if (geracoes_sem_melhora >= parametros.max_geracoes_sem_melhora) {
            break;
        }
    }

    resultado.historico_melhor_global.push_back(resultado.melhor.custo_total);
    resultado.historico_melhor_geracao.push_back(populacao.front().custo_total);
    resultado.historico_media_populacao.push_back(calcularMediaPopulacao(populacao));

    return resultado;
}
