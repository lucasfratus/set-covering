#include <iostream>
#include <fstream>  
#include <string>
#include <vector>
#include <sstream>

struct Solucao {
    int id;
    float custo;
    std::vector<int> i_linhas;

};


int main() {
    std::ifstream arquivo("Teste_01.dat");
    std::vector<Solucao> lista_solucoes;
    std::string linha;

    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;

        std::stringstream ss(linha);
        Solucao sol_atual;

        if (ss >> sol_atual.id >> sol_atual.custo) {
            int i_linha;
            while (ss >> i_linha) {
                sol_atual.i_linhas.push_back(i_linha);
            }
            lista_solucoes.push_back(sol_atual);
        }
    }

    for (const auto& sol : lista_solucoes) {
        std::cout << "ID: " << sol.id << " | Custo: " << sol.custo << " | Elementos: ";
        for (int elem : sol.i_linhas) {
            std::cout << elem << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
