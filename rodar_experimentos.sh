#!/bin/bash

EXEC="./programa"

INSTANCIAS=("Teste_01.dat" "Teste_02.dat" "Teste_03.dat")
MELHORES=(557.44 537.89 517.58)

REPETICOES=30

echo "instancia,melhor_conhecido,execucao,seed,geracao_melhor,custo,gap_percentual,tempo_segundos" > resultados.csv

for i in "${!INSTANCIAS[@]}"; do
    INSTANCIA="${INSTANCIAS[$i]}"
    MELHOR_CONHECIDO="${MELHORES[$i]}"

    for execucao in $(seq 1 $REPETICOES); do
        INICIO=$(date +%s.%N)

        SAIDA=$($EXEC "$INSTANCIA" --random)

        FIM=$(date +%s.%N)
        TEMPO=$(echo "$FIM - $INICIO" | bc)

        SEED=$(echo "$SAIDA" | grep "Seed usada:" | awk '{print $3}')
        GERACAO=$(echo "$SAIDA" | grep "Melhor solucao encontrada na geracao" | awk '{print $6}')
        CUSTO=$(echo "$SAIDA" | grep "Custo total" | awk '{print $4}')

        GAP=$(echo "scale=6; (($CUSTO - $MELHOR_CONHECIDO) / $MELHOR_CONHECIDO) * 100" | bc)

        echo "$INSTANCIA,$MELHOR_CONHECIDO,$execucao,$SEED,$GERACAO,$CUSTO,$GAP,$TEMPO" >> resultados.csv

        echo "$INSTANCIA execução $execucao: custo=$CUSTO gap=$GAP% seed=$SEED"
    done
done
