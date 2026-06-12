#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <bitset>

const int MAX_COLUNAS = 100000;  // tamanho fixo (maior q qualquer instância) para utilizar o bitset

int N_LINHAS, N_COLUNAS;

std::vector<float>            custo;      // custo[j]     = custo da coluna j
std::vector<std::vector<int>> cobertura;  // cobertura[j] = linhas cobertas pela coluna j

struct Solucao {
    float custo_total = 0.0f;
    std::bitset<MAX_COLUNAS> colunas_escolhidas;  // representação binária

    bool ehViavel() {
        std::vector<bool> cobertas(N_LINHAS + 1, false);
        for (int j = 0; j < N_COLUNAS; j++) {
            if (colunas_escolhidas.test(j)) {
                for (int linha : cobertura[j]) {
                    cobertas[linha] = true;
                }
            }
        }
        for (int i = 1; i <= N_LINHAS; i++) {
            if (!cobertas[i]) {
                return false;
            }
        }
        return true;
    }

    void imprimir() {
        std::cout << "Custo total = " << custo_total << "\n";
        std::cout << "Viavel = " << (ehViavel() ? "SIM" : "NAO") << "\n";
        std::cout << "Colunas = ";
        for (int j = 0; j < N_COLUNAS; j++)
            if (colunas_escolhidas.test(j)) std::cout << (j + 1) << " ";
        std::cout << "\nBinario = ";
        for (int j = 0; j < N_COLUNAS; j++)
            std::cout << colunas_escolhidas[j];
        std::cout << "\n";
    }
};


void lerInstancia(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    std::string var, linha;

    arquivo >> var >> N_LINHAS;    
    arquivo >> var >> N_COLUNAS;   
    arquivo >> var;       

    custo.resize(N_COLUNAS);
    cobertura.resize(N_COLUNAS);

    std::getline(arquivo, linha);    // consome resto da linha "DADOS"
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        int id; float c;
        if (!(ss >> id >> c)) continue;
        int j = id - 1;            
        custo[j] = c;
        int row;
        while (ss >> row)
            cobertura[j].push_back(row);
    }
}

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