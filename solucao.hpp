#pragma once
 
#include <algorithm>
#include <iostream>
#include <vector>

#include "leitura.hpp"

struct Solucao {
    float custo_total = 0.0f;

    /*
    Representação por lista de inteiros.

    Cada inteiro armazenado em colunas_escolhidas representa uma coluna que
    pertence à solução. Assim, a solução guarda somente as colunas selecionadas,
    em vez de manter um vetor binário com todas as colunas da instância.

    Exemplo:
        colunas_escolhidas = {2, 10, 35}

    significa que as colunas 2, 10 e 35 foram escolhidas para compor a solução.
    Internamente, os índices continuam começando em 0. Na impressão, é mostrado
    j + 1 para ficar compatível com os arquivos de entrada.
    */
    std::vector<int> colunas_escolhidas;

    bool contemColuna(int coluna) const {
        return std::find(
            colunas_escolhidas.begin(),
            colunas_escolhidas.end(),
            coluna
        ) != colunas_escolhidas.end();
    }

    void adicionarColuna(int coluna) {
        if (!contemColuna(coluna)) {
            colunas_escolhidas.push_back(coluna);
            custo_total += custo[coluna];
        }
    }

    void removerColuna(int coluna) {
        auto it = std::find(
            colunas_escolhidas.begin(),
            colunas_escolhidas.end(),
            coluna
        );

        if (it != colunas_escolhidas.end()) {
            colunas_escolhidas.erase(it);
            custo_total -= custo[coluna];
        }
    }

    void normalizar() {
        std::sort(colunas_escolhidas.begin(), colunas_escolhidas.end());
        colunas_escolhidas.erase(
            std::unique(colunas_escolhidas.begin(), colunas_escolhidas.end()),
            colunas_escolhidas.end()
        );
    }

    void recalcularCusto() {
        normalizar();

        custo_total = 0.0f;
        for (int coluna : colunas_escolhidas) {
            custo_total += custo[coluna];
        }
    }

    int quantidadeColunas() const {
        return static_cast<int>(colunas_escolhidas.size());
    }

    bool ehViavel() const {
        std::vector<bool> cobertas(N_LINHAS + 1, false);

        for (int coluna : colunas_escolhidas) {
            for (int linha : cobertura[coluna]) {
                cobertas[linha] = true;
            }
        }

        for (int linha = 1; linha <= N_LINHAS; linha++) {
            if (!cobertas[linha]) {
                return false;
            }
        }

        return true;
    }

    void imprimir() const {
        std::cout << "Custo total = " << custo_total << "\n";
        std::cout << "Viavel = " << (ehViavel() ? "SIM" : "NAO") << "\n";
        std::cout << "Quantidade de colunas = " << quantidadeColunas() << "\n";
        std::cout << "Colunas = ";

        std::vector<int> ordenadas = colunas_escolhidas;
        std::sort(ordenadas.begin(), ordenadas.end());

        for (int coluna : ordenadas) {
            std::cout << (coluna + 1) << " ";
        }

        std::cout << "\n";
    }
};
