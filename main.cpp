#include <iostream>
#include <vector>
 
#include "leitura.hpp"
#include "solucao.hpp"
#include "construcao_solucao.hpp"
#include "busca_local.hpp"
#include "algoritmo_genetico.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_instancia> [seed]\n";
        return 1;
    }

    std::string caminho = argv[1];

    unsigned int seed = 42;
    if (argc >= 3) {
        seed = static_cast<unsigned int>(std::stoul(argv[2]));
    }

    try {
        lerInstancia(caminho);
        setSeed(seed);

        std::cout << "Instancia: " << N_LINHAS << " linhas, " << N_COLUNAS << " colunas\n";
        std::cout << "Arquivo: " << caminho << "\n";
        std::cout << "Seed: " << seed << "\n\n";

        Parametros parametros;
        parametros.tamanho_populacao = 30;
        parametros.max_geracoes = 100;
        parametros.n_torneio = 3;
        parametros.n_elite = 2;
        parametros.alfa_grasp = 0.30;
        parametros.taxa_crossover = 0.90;
        parametros.taxa_mutacao = 1.0 / N_COLUNAS;
        parametros.aplicar_bl_pop_inicial = true;

        Resultado resultado = executarAlgoritmoGenetico(parametros);

        std::cout << "Melhor solucao encontrada na geracao "
                  << resultado.geracao_do_melhor << "\n";

        resultado.melhor.imprimir();

    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\n";
        return 1;
    }

    return 0;
}