COMPILADOR PORTUGOL - FASE 2: ANÁLISE SEMÂNTICA E GERAÇÃO DE CÓDIGO MEPA

Integrantes:
  Arthur Roldan Slikta   - 10353847
  Felipe Bertacco Haddad - 10437372


1. COMO COMPILAR E EXECUTAR
Deixar o hash_mack.o na mesma pasta do compilador.c e compilar com:

  gcc -Wall -Wno-unused-result -g -Og compilador.c hash_mack.o -o compilador

Para executar:

  ./compilador <arquivo de entrada>

Se não tiver erro no programa, o compilador mostra o código MEPA e depois a
tabela de símbolos. O código MEPA também é salvo em um arquivo .mepa com o
mesmo nome do arquivo de entrada (teste.txt gera teste.mepa), para rodar no
interpretador:

  ./mepa -f teste.mepa

Se tiver erro, mostra só o primeiro erro com a linha e o tipo, por exemplo:

  #  5:erro semantico, identificador [z] nao declarado

Para testar a memória com o valgrind:

  valgrind --leak-check=yes ./compilador <arquivo de entrada>

2. O QUE FOI CONCLUÍDO
- Análise léxica e sintática da fase 1 (sem a impressão dos átomos).
- Tabela de Símbolos com Tabela de Dispersão (PRIME_NUMER = 211, função hashMack
  e nós TNo conforme o enunciado). Para cada identificador são guardados:
  categoria (variável, parâmetro, função, procedimento), escopo (0 global,
  1 local), endereço, se o parâmetro é por referência (var), o rótulo da rotina,
  o número de parâmetros e o modo de cada parâmetro.
- Verificações semânticas na declaração:
  - identificador repetido no mesmo escopo (variáveis globais e locais,
    parâmetros, local com o mesmo nome de um parâmetro, rotina com o mesmo nome
    de uma variável global).
- Verificações semânticas no corpo do programa:
  - identificador não declarado;
  - atribuição somente para variável, parâmetro ou para o nome da função dentro
    da própria função (retorno);
  - chamada de procedimento somente com identificador de procedimento;
  - procedimento não pode ser usado em expressão e variável não pode ser
    chamada com parênteses;
  - leia() somente com variáveis ou parâmetros;
  - número de argumentos igual ao número de parâmetros;
  - argumento de parâmetro por referência (var) deve ser uma variável ou
    parâmetro.
- Geração de código MEPA:
  - programa: INPP, AMEM, DSVS para o programa principal, rotinas, DMEM e PARA;
  - expressões aritméticas, relacionais e lógicas (SOMA, SUBT, MULT, DIVI,
    INVR, CONJ, DISJ, NEGA, CMIG, CMDG, CMME, CMEG, CMMA, CMAG), constantes
    (CRCT), variáveis (CRVL);
  - atribuição (ARMZ), leia (LEIT), escreva (IMPR);
  - se/senao e enquanto com DSVF, DSVS e rótulos Lk:NADA (proximo_rotulo());
  - procedimentos e funções com variáveis locais, parâmetros por valor e por
    referência e recursão (ENPR, AMEM, DMEM, RTPR n, CHPR, CREN, CRVI, ARMI).
    O i-ésimo de n parâmetros fica no endereço 1,-(n+3-i) e o retorno da função
    em 1,-(n+3). A chamada de função reserva o retorno com AMEM 1.

3. DECISÕES DE PROJETO
- Escopo na Tabela de Símbolos: ao terminar uma função ou procedimento, as
  variáveis locais e os parâmetros não são removidos, apenas marcados como
  inativos. Assim eles não são mais encontrados na busca, mas continuam na
  tabela para a impressão final. A busca encontra primeiro o identificador local,
  pois ele é inserido depois do global no início da lista da entrada.
- O nome do programa (algoritmo <nome>;) não é inserido na Tabela de Símbolos,
  pois não é variável, parâmetro, função ou procedimento.
- Os nomes de funções e procedimentos ficam no escopo global; parâmetros e
  variáveis locais ficam no escopo local.
- Identificadores diferenciam letras maiúsculas e minúsculas (x e X são
  identificadores diferentes). As palavras reservadas não diferenciam.
- Impressão da Tabela de Símbolos: as entradas são percorridas de 0 a 210 e,
  para cada identificador, são impressos a entrada, o ID, a categoria e, para
  variáveis e parâmetros, o endereço no formato m,n usado pela MEPA. Exemplo:
    entrada [ 97] id [a] variavel endereco [0,0]
    entrada [120] id [x] parametro endereco [1,-4]
    entrada [193] id [troca] procedimento
- O código MEPA é acumulado em um buffer e só é impresso no final, quando não
  houve erro. Assim, um programa com erro não mostra parte do código MEPA antes
  da mensagem de erro.
- Operador mod: a MEPA não tem instrução de resto, então a mod b é calculado como
  a - (a div b) * b. Para isso são usadas duas posições auxiliares no fim da área
  das variáveis globais (0,n e 0,n+1, onde n é o número de variáveis globais),
  reservadas no AMEM inicial somente se o programa usa mod.
- Tipos: não há distinção entre expressões inteiras e
  lógicas. verdadeiro e falso são gerados como CRCT 1 e CRCT 0 e uma constante
  caractere é gerada com o seu código ASCII (ex.: 'A' -> CRCT 65).
- Função ou procedimento sem parâmetros é chamado sem parênteses (ex.: p; ou
  x := f), pois a gramática exige ao menos uma expressão dentro dos parênteses.
  Esses casos geram RTPR 0 no retorno da rotina.
- Erro sintático encontrado no fim do arquivo é informado na linha do último
  átomo lido, e o comentário não fechado é informado na linha onde foi aberto.


4. LIMITAÇÕES E BUGS CONHECIDOS
- Uma rotina pode ter no máximo 32 parâmetros (MAX_PARAMETROS); acima disso é
  gerado um erro semântico.
- Não há verificação de tipos entre inteiro, logico e caractere (todos são
  tratados como inteiros).
- O nome do arquivo .mepa gerado é limitado a 511 caracteres.
