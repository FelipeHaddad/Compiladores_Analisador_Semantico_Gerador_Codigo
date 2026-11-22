#!/bin/bash
# Uso (no Ubuntu, na pasta do compilador.c): bash testes/roda_testes.sh
cd "$(dirname "$0")/.." || exit 1
rm -f compilador

echo "== compilacao =="
gcc -Wall -Wno-unused-result -g -Og compilador.c hash_mack.o -o compilador || exit 1

falhas=0

echo "== testes de erro =="
for f in testes/erros/*.txt; do
    obtido=$(./compilador "$f")
    esperado=$(cat "${f%.txt}.esp")
    if [ "$obtido" == "$esperado" ]; then
        echo "OK     $f"
    else
        echo "FALHOU $f"
        echo "       esperado: $esperado"
        echo "       obtido:   $obtido"
        falhas=$((falhas + 1))
    fi
done

echo "== programas corretos (gera .mepa) =="
for f in testes/corretos/*.txt; do
    if ./compilador "$f" | grep -q ":erro "; then
        echo "FALHOU $f"
        ./compilador "$f"
        falhas=$((falhas + 1))
    else
        echo "OK     ${f%.txt}.mepa  entrada: [$(cat "${f%.txt}.in")]  saida esperada: [$(cat "${f%.txt}.esp")]"
    fi
done

if command -v valgrind > /dev/null; then
    echo "== valgrind =="
    for f in testes/*/*.txt; do
        valgrind --leak-check=yes --error-exitcode=99 ./compilador "$f" > /dev/null 2> valgrind.log
        if [ $? -eq 99 ] || ! grep -q "All heap blocks were freed" valgrind.log; then
            echo "FALHOU $f"
            falhas=$((falhas + 1))
        fi
    done
    rm -f valgrind.log
    echo "valgrind concluido"
fi

echo "== $falhas falha(s) =="
