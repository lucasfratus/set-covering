#pragma once

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


void lerInstancia(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    std::string var;
    std::string linha;

    arquivo >> var >> N_LINHAS;    
    arquivo >> var >> N_COLUNAS;   
    arquivo >> var;       

    custo.resize(N_COLUNAS);
    cobertura.resize(N_COLUNAS);

    std::getline(arquivo, linha);    // consome resto da linha "DADOS"
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        std::stringstream ss(linha);
        int id;
         float c;
        if (!(ss >> id >> c)) {
            continue;
        }
        
        int j = id - 1;            
        custo[j] = c;
        int row;
        while (ss >> row)
            cobertura[j].push_back(row);
    }
}
