#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "leitura.hpp"
#include "solucao.hpp"
#include "construcao_solucao.hpp"
#include "busca_local.hpp"
#include "algoritmo_genetico.hpp"


void salvarHistoricoConvergencia(const Resultado& resultado, const std::string& caminho_saida) {
    std::ofstream arquivo(caminho_saida);

    if (!arquivo.is_open()) {
        throw std::runtime_error("Nao foi possivel criar o arquivo: " + caminho_saida);
    }

    arquivo << "geracao,melhor_global,melhor_geracao,media_populacao\n";

    for (int i = 0; i < (int)resultado.historico_melhor_global.size(); i++) {
        arquivo << i << ","
                << resultado.historico_melhor_global[i] << ","
                << resultado.historico_melhor_geracao[i] << ","
                << resultado.historico_media_populacao[i] << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_instancia> [seed | --random | -r]\n";
        return 1;
    }

    std::string caminho = argv[1];

    unsigned int seed = 42;
    if (argc >= 3) {
        std::string argumento_seed = argv[2];
        if (argumento_seed == "--random" || argumento_seed == "-r") {
            std::random_device rd;
            std::mt19937 gerador_seed(rd());
            std::uniform_int_distribution<unsigned int> distribuicao_seed;

            seed = distribuicao_seed(gerador_seed);
        } else {
            seed = static_cast<unsigned int>(std::stoul(argumento_seed));
        }
    }

    try {
        lerInstancia(caminho);
        setSeed(seed);

        std::cout << "Instância: " << N_LINHAS << " linhas, " << N_COLUNAS << " colunas\n";
        std::cout << "Arquivo: " << caminho << "\n";
        std::cout << "Seed usada: " << seed << "\n\n";

        Parametros parametros;
        parametros.tamanho_populacao = 200;
        parametros.max_geracoes = 500;
        parametros.max_geracoes_sem_melhora = 100;
        parametros.n_torneio = 3;
        parametros.n_elite = 2;
        parametros.alfa_grasp = 0.45;
        parametros.taxa_crossover = 0.90;
        parametros.taxa_busca_troca = 0.20;
        parametros.taxa_mutacao = 0.05;
        parametros.aplicar_bl_pop_inicial = false;

        Resultado resultado = executarAlgoritmoGenetico(parametros);

        salvarHistoricoConvergencia(resultado, "convergencia.csv");

        std::cout << "Melhor solucao encontrada na geracao "
                  << resultado.geracao_do_melhor << "\n";

        resultado.melhor.imprimir();

    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\n";
        return 1;
    }

    return 0;
}