#include <iostream>
#include <vector>
 
#include "leitura.hpp"
#include "solucao.hpp"
#include "construcao_solucao.hpp"
#include "busca_local.hpp"



int main() {
    lerInstancia("Teste_01.dat");
    std::cout << "Instancia: " << N_LINHAS << " linhas, " << N_COLUNAS << " colunas\n\n";

    if (N_COLUNAS > MAX_COLUNAS) {
        std::cerr << "Erro: N_COLUNAS excede MAX_COLUNAS!\n";
        return 1;
    }


    // teste de representação
    Solucao sol;
    sol.colunas_escolhidas.set(15);   
    sol.colunas_escolhidas.set(32);   
    sol.custo_total = custo[15] + custo[32];

    sol.imprimir();

    return 0;
}