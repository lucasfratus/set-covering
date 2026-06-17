#pragma once
 
#include <iostream>
#include <vector>
#include "instancia.hpp"

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