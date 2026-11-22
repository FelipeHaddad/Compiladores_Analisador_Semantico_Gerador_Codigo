//Arthur Roldan Slikta - 10353847
//Felipe Haddad - 10437372

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>


   // DEFINIÇÃO DOS ÁTOMOS DA LINGUAGEM
typedef enum {
    // Palavras Reservadas
    ATOMO_ALGORITMO,
    ATOMO_CARACTERE,
    ATOMO_DIV,
    ATOMO_E,
    ATOMO_ENQUANTO,
    ATOMO_ENTAO,
    ATOMO_ESCREVA,
    ATOMO_FACA,
    ATOMO_FALSO,
    ATOMO_FIM,
    ATOMO_FUNCAO,
    ATOMO_IDENTIFICADOR,
    ATOMO_INICIO,
    ATOMO_INTEIRO,
    ATOMO_LEIA,
    ATOMO_LOGICO,
    ATOMO_MOD,
    ATOMO_OU,
    ATOMO_PROCEDIMENTO,
    ATOMO_SE,
    ATOMO_SENAO,
    ATOMO_VAR,
    ATOMO_VERDADEIRO,

    // Constantes
    ATOMO_CONSTINT,
    ATOMO_CONSTCHAR,

    // Símbolos e Operadores Delimitadores
    ATOMO_PONTO_VIRGULA,  // ';'
    ATOMO_PONTO,          // '.'
    ATOMO_VIRGULA,        // ','
    ATOMO_DOIS_PONTOS,    // ':'
    ATOMO_ATRIBUICAO,     // ':='
    ATOMO_ABRE_PAR,       // '('
    ATOMO_FECHA_PAR,      // ')'

    // Operadores Relacionais
    ATOMO_OP_DIFERENTE,   // '<>'
    ATOMO_OP_MENOR_IGUAL, // '<='
    ATOMO_OP_MAIOR_IGUAL, // '>='
    ATOMO_OP_MENOR,       // '<'
    ATOMO_OP_MAIOR,       // '>'
    ATOMO_OP_IGUAL,       // '='

    // Operadores
    ATOMO_OP_SOMA,        // '+'
    ATOMO_OP_SUB,         // '-'
    ATOMO_OP_MULT,        // '*'
    ATOMO_NAO,            // nao

    // Especiais
    ATOMO_COMENTARIO,
    ATOMO_ERRO,
    ATOMO_FIM_ARQUIVO
} TAtomo;
 
//   ESTRUTURA TInfoAtomo
    
typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;     // atributo para constint
        char id[16];    // atributo para identificador (máx 15 chars + \0)
        char ch;        // atributo para constchar
    } atributo;
} TInfoAtomo;

//   TABELA DE SÍMBOLOS (Hash)

#define PRIME_NUMER 211
#define MAX_PARAMETROS 32

typedef enum {
    CAT_VARIAVEL,
    CAT_PARAMETRO,
    CAT_FUNCAO,
    CAT_PROCEDIMENTO
} TCategoria;

typedef struct _TNo{
   char ID[16];
   int endereço;
   TCategoria categoria;
   int escopo;                          
   int ativo;                           
   int por_referencia;                  
   int rotulo;                          
   int num_parametros;
   int parametros_ref[MAX_PARAMETROS]; 

   struct _TNo *prox;
}TNo;

typedef struct {
    TNo *entradas[PRIME_NUMER];
}TTabelaSimbolos;

//   VARIÁVEIS GLOBAIS

FILE *arquivo_fonte = NULL;
int linha_atual = 1;
TInfoAtomo lookahead;
TTabelaSimbolos tabela;
int escopo_atual = 0;
int endereco_atual = 0;
TNo *rotina_atual = NULL;
char *codigo = NULL;
int tam_codigo = 0;
int capacidade_codigo = 0;
int rotulo_atual = 0;
int num_globais = 0;
int usa_mod = 0;

//  PROTÓTIPOS DAS FUNÇÕES

TInfoAtomo obter_atomo(void);
void consome(TAtomo atomo_esperado);
const char* nome_atomo(TAtomo atomo);

// Função de hash implementada em hash_mack.o
int hashMack(char *s);

// Funções da Tabela de Símbolos
TNo* insere(char *id, TCategoria categoria, int endereco, int linha);
TNo* busca(char *id);
TNo* verifica_declarado(TInfoAtomo info);
void fecha_escopo(void);
void imprime_tabela(void);
void libera_tabela(void);

// Funções da Geração de Código MEPA
void gera(const char *formato, ...);
int proximo_rotulo(void);
void gera_armazena(TNo *no);
void imprime_codigo(void);
void libera_codigo(void);

// Funções do ASDR para a Gramática EBNF de Portugol
void programa(void);
void bloco(void);
void declaracao_variaveis(void);
void lista_variaveis(TCategoria categoria, int por_referencia);
void declaracao_de_rotinas(void);
void declaracao_de_funcao(void);
void declaracao_de_procedimento(void);
void tipo(void);
void parametros_formais(TNo *rotina);
void parametro_formal(void);
void comando_composto(void);
void comando(void);
void comando_entrada(void);
void comando_saida(void);
void comando_condicional(void);
void comando_repeticao(void);
int lista_expressao(TNo *rotina);
void argumento(TNo *rotina, int indice);
void expressao(void);
void operador_relacional(void);
void expressao_simples(void);
void operador_adicao(void);
void termo(void);
void operador_multiplicacao(void);
void fator(void);
 
//   AUXILIARES E IMPRESSÃO DE MENSAGENS DE ERRO
    
const char* nome_atomo(TAtomo atomo) {
    switch (atomo) {
        case ATOMO_ALGORITMO: return "algoritmo";
        case ATOMO_CARACTERE: return "caractere";
        case ATOMO_DIV: return "div";
        case ATOMO_E: return "e";
        case ATOMO_ENQUANTO: return "enquanto";
        case ATOMO_ENTAO: return "entao";
        case ATOMO_ESCREVA: return "escreva";
        case ATOMO_FACA: return "faca";
        case ATOMO_FALSO: return "falso";
        case ATOMO_FIM: return "fim";
        case ATOMO_FUNCAO: return "funcao";
        case ATOMO_INICIO: return "inicio";
        case ATOMO_INTEIRO: return "inteiro";
        case ATOMO_LEIA: return "leia";
        case ATOMO_LOGICO: return "logico";
        case ATOMO_MOD: return "mod";
        case ATOMO_OU: return "ou";
        case ATOMO_PROCEDIMENTO: return "procedimento";
        case ATOMO_SE: return "se";
        case ATOMO_SENAO: return "senao";
        case ATOMO_VAR: return "var";
        case ATOMO_VERDADEIRO: return "verdadeiro";
        case ATOMO_IDENTIFICADOR: return "identificador";
        case ATOMO_CONSTINT: return "constint";
        case ATOMO_CONSTCHAR: return "constchar";
        case ATOMO_PONTO_VIRGULA: return ";";
        case ATOMO_PONTO: return ".";
        case ATOMO_VIRGULA: return ",";
        case ATOMO_DOIS_PONTOS: return ":";
        case ATOMO_ATRIBUICAO: return ":=";
        case ATOMO_ABRE_PAR: return "(";
        case ATOMO_FECHA_PAR: return ")";
        case ATOMO_OP_DIFERENTE: return "<>";
        case ATOMO_OP_MENOR_IGUAL: return "<=";
        case ATOMO_OP_MAIOR_IGUAL: return ">=";
        case ATOMO_OP_MENOR: return "<";
        case ATOMO_OP_MAIOR: return ">";
        case ATOMO_OP_IGUAL: return "=";
        case ATOMO_OP_SOMA: return "+";
        case ATOMO_OP_SUB: return "-";
        case ATOMO_OP_MULT: return "*";
        case ATOMO_NAO: return "nao";
        case ATOMO_COMENTARIO: return "comentario";
        case ATOMO_ERRO: return "erro";
        case ATOMO_FIM_ARQUIVO: return "fim de arquivo";
        default: return "desconhecido";
    }
}

const char* nome_categoria(TCategoria categoria) {
    switch (categoria) {
        case CAT_VARIAVEL: return "variavel";
        case CAT_PARAMETRO: return "parametro";
        case CAT_FUNCAO: return "funcao";
        case CAT_PROCEDIMENTO: return "procedimento";
        default: return "desconhecido";
    }
}

void erro_lexico(const char *msg) {
    printf("# %2d:erro lexico, %s\n", linha_atual, msg);
    if (arquivo_fonte) fclose(arquivo_fonte);
    libera_tabela();
    libera_codigo();
    exit(1);
}

void erro_sintatico(TAtomo atomo_esperado, TInfoAtomo atomo_encontrado) {
    printf("# %2d:erro sintatico, esperado [%s] encontrado [%s]\n",
           atomo_encontrado.linha,
           nome_atomo(atomo_esperado),
           nome_atomo(atomo_encontrado.atomo));
    if (arquivo_fonte) fclose(arquivo_fonte);
    libera_tabela();
    libera_codigo();
    exit(1);
}

void erro_semantico(int linha, const char *id, const char *msg) {
    printf("# %2d:erro semantico, identificador [%s] %s\n", linha, id, msg);
    if (arquivo_fonte) fclose(arquivo_fonte);
    libera_tabela();
    libera_codigo();
    exit(1);
}

//   TABELA DE SÍMBOLOS

TNo* insere(char *id, TCategoria categoria, int endereco, int linha) {
    int entrada = hashMack(id);
    for (TNo *no = tabela.entradas[entrada]; no != NULL; no = no->prox) {
        if (no->ativo && no->escopo == escopo_atual && strcmp(no->ID, id) == 0) {
            erro_semantico(linha, id, "ja declarado neste escopo");
        }
    }

    TNo *novo = (TNo*)calloc(1, sizeof(TNo));
    strncpy(novo->ID, id, 15);
    novo->ID[15] = '\0';
    novo->endereço = endereco;
    novo->categoria = categoria;
    novo->escopo = escopo_atual;
    novo->ativo = 1;
    novo->prox = tabela.entradas[entrada];
    tabela.entradas[entrada] = novo;
    return novo;
}

TNo* busca(char *id) {
    for (TNo *no = tabela.entradas[hashMack(id)]; no != NULL; no = no->prox) {
        if (no->ativo && strcmp(no->ID, id) == 0) {
            return no;
        }
    }
    return NULL;
}

TNo* verifica_declarado(TInfoAtomo info) {
    TNo *no = busca(info.atributo.id);
    if (no == NULL) {
        erro_semantico(info.linha, info.atributo.id, "nao declarado");
    }
    return no;
}

void fecha_escopo(void) {
    for (int i = 0; i < PRIME_NUMER; i++) {
        for (TNo *no = tabela.entradas[i]; no != NULL; no = no->prox) {
            if (no->escopo == 1) no->ativo = 0;
        }
    }
    escopo_atual = 0;
}

void imprime_tabela(void) {
    printf("\nTABELA DE SIMBOLOS\n");
    for (int i = 0; i < PRIME_NUMER; i++) {
        for (TNo *no = tabela.entradas[i]; no != NULL; no = no->prox) {
            if (no->categoria == CAT_VARIAVEL || no->categoria == CAT_PARAMETRO) {
                printf("entrada [%3d] id [%s] %s endereco [%d,%d]\n",
                       i, no->ID, nome_categoria(no->categoria), no->escopo, no->endereço);
            } else {
                printf("entrada [%3d] id [%s] %s\n", i, no->ID, nome_categoria(no->categoria));
            }
        }
    }
}

void libera_tabela(void) {
    for (int i = 0; i < PRIME_NUMER; i++) {
        TNo *no = tabela.entradas[i];
        while (no != NULL) {
            TNo *prox = no->prox;
            free(no);
            no = prox;
        }
        tabela.entradas[i] = NULL;
    }
}

//   GERAÇÃO DE CÓDIGO MEPA

void gera(const char *formato, ...) {
    char linha[64];
    va_list args;
    va_start(args, formato);
    int tam = vsnprintf(linha, sizeof(linha), formato, args);
    va_end(args);
    if (tam_codigo + tam + 1 > capacidade_codigo) {
        capacidade_codigo = (tam_codigo + tam + 1) * 2;
        codigo = (char*)realloc(codigo, capacidade_codigo);
    }
    strcpy(codigo + tam_codigo, linha);
    tam_codigo += tam;
}

int proximo_rotulo(void) {
    return ++rotulo_atual;
}

void gera_armazena(TNo *no) {
    if (no->categoria == CAT_FUNCAO) {
        gera("   ARMZ 1,%d\n", no->endereço);
    } else if (no->por_referencia) {
        gera("   ARMI %d,%d\n", no->escopo, no->endereço);
    } else {
        gera("   ARMZ %d,%d\n", no->escopo, no->endereço);
    }
}

void imprime_codigo(void) {
    int total = num_globais + (usa_mod ? 2 : 0);
    printf("   INPP\n");
    if (total > 0) printf("   AMEM %d\n", total);
    printf("%s", codigo);
}

void libera_codigo(void) {
    free(codigo);
    codigo = NULL;
}

//   ANALISADOR LÉXICO (obter_atomo)
    
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    int c;

    while (1) {
        c = fgetc(arquivo_fonte);

        if (c == EOF) {
            info.atomo = ATOMO_FIM_ARQUIVO;
            info.linha = linha_atual;
            return info;
        }

        // Trata delimitadores (espaço, tabulação, nova linha, retorno)
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '\n') {
            linha_atual++;
            continue;
        }

        info.linha = linha_atual;

        // Comentários de múltiplas linhas: {- ... -}
        if (c == '{') {
            int proximo = fgetc(arquivo_fonte);
            if (proximo == '-') {
                while (1) {
                    int ch = fgetc(arquivo_fonte);
                    if (ch == EOF) {
                        erro_lexico("comentario nao fechado ate o fim do arquivo");
                    }
                    if (ch == '\n') {
                        linha_atual++;
                    }
                    if (ch == '-') {
                        int ch2 = fgetc(arquivo_fonte);
                        if (ch2 == '}') {
                            info.atomo = ATOMO_COMENTARIO;
                            return info;
                        } else {
                            ungetc(ch2, arquivo_fonte);
                        }
                    }
                }
            } else {
                ungetc(proximo, arquivo_fonte);
                erro_lexico("caractere '{' invalido (esperado comentario '{-')");
            }
        }

        // Identificadores e Palavras Reservadas
        if (isalpha(c)) {
            char lexema[100];
            int idx = 0;
            lexema[idx++] = (char)c;

            while (1) {
                int ch = fgetc(arquivo_fonte);
                if (isalnum(ch) || ch == '_') {
                    if (idx < 99) lexema[idx++] = (char)ch;
                } else {
                    ungetc(ch, arquivo_fonte);
                    break;
                }
            }
            lexema[idx] = '\0';

            // Verifica tamanho máximo de 15 caracteres para identificador
            if (idx > 15) {
                erro_lexico("identificador com mais de 15 caracteres");
            }

            // Converter para minúsculo para comparar palavra reservada (case-insensitive)
            char lexema_lower[100];
            for (int i = 0; i <= idx; i++) {
                lexema_lower[i] = (char)tolower(lexema[i]);
            }

            if (strcmp(lexema_lower, "algoritmo") == 0) info.atomo = ATOMO_ALGORITMO;
            else if (strcmp(lexema_lower, "caractere") == 0) info.atomo = ATOMO_CARACTERE;
            else if (strcmp(lexema_lower, "div") == 0) info.atomo = ATOMO_DIV;
            else if (strcmp(lexema_lower, "e") == 0) info.atomo = ATOMO_E;
            else if (strcmp(lexema_lower, "enquanto") == 0) info.atomo = ATOMO_ENQUANTO;
            else if (strcmp(lexema_lower, "entao") == 0) info.atomo = ATOMO_ENTAO;
            else if (strcmp(lexema_lower, "escreva") == 0) info.atomo = ATOMO_ESCREVA;
            else if (strcmp(lexema_lower, "faca") == 0) info.atomo = ATOMO_FACA;
            else if (strcmp(lexema_lower, "falso") == 0) info.atomo = ATOMO_FALSO;
            else if (strcmp(lexema_lower, "fim") == 0) info.atomo = ATOMO_FIM;
            else if (strcmp(lexema_lower, "funcao") == 0) info.atomo = ATOMO_FUNCAO;
            else if (strcmp(lexema_lower, "inicio") == 0) info.atomo = ATOMO_INICIO;
            else if (strcmp(lexema_lower, "inteiro") == 0) info.atomo = ATOMO_INTEIRO;
            else if (strcmp(lexema_lower, "leia") == 0) info.atomo = ATOMO_LEIA;
            else if (strcmp(lexema_lower, "logico") == 0) info.atomo = ATOMO_LOGICO;
            else if (strcmp(lexema_lower, "mod") == 0) info.atomo = ATOMO_MOD;
            else if (strcmp(lexema_lower, "ou") == 0) info.atomo = ATOMO_OU;
            else if (strcmp(lexema_lower, "procedimento") == 0) info.atomo = ATOMO_PROCEDIMENTO;
            else if (strcmp(lexema_lower, "se") == 0) info.atomo = ATOMO_SE;
            else if (strcmp(lexema_lower, "senao") == 0) info.atomo = ATOMO_SENAO;
            else if (strcmp(lexema_lower, "var") == 0) info.atomo = ATOMO_VAR;
            else if (strcmp(lexema_lower, "verdadeiro") == 0) info.atomo = ATOMO_VERDADEIRO;
            else if (strcmp(lexema_lower, "nao") == 0) info.atomo = ATOMO_NAO;
            else {
                info.atomo = ATOMO_IDENTIFICADOR;
                strcpy(info.atributo.id, lexema);
            }
            return info;
        }

        // Constantes Inteiras: digito+((E(+|ε)digito+)|ε)
        if (isdigit(c)) {
            char num_str[100];
            int idx = 0;
            num_str[idx++] = (char)c;

            while (1) {
                int ch = fgetc(arquivo_fonte);
                if (isdigit(ch)) {
                    if (idx < 99) num_str[idx++] = (char)ch;
                } else {
                    ungetc(ch, arquivo_fonte);
                    break;
                }
            }

            // Notação exponencial 'E' ou 'e'
            int ch_exp = fgetc(arquivo_fonte);
            if (ch_exp == 'E' || ch_exp == 'e') {
                num_str[idx++] = 'E';
                int ch_sinal = fgetc(arquivo_fonte);
                if (ch_sinal == '+') {
                    num_str[idx++] = '+';
                } else if (isdigit(ch_sinal)) {
                    num_str[idx++] = (char)ch_sinal;
                } else {
                    ungetc(ch_sinal, arquivo_fonte);
                }

                while (1) {
                    int ch = fgetc(arquivo_fonte);
                    if (isdigit(ch)) {
                        if (idx < 99) num_str[idx++] = (char)ch;
                    } else {
                        ungetc(ch, arquivo_fonte);
                        break;
                    }
                }
            } else {
                ungetc(ch_exp, arquivo_fonte);
            }
            num_str[idx] = '\0';

            if (strchr(num_str, 'E')) {
                double val = atof(num_str);
                info.atributo.numero = (int)val;
            } else {
                info.atributo.numero = atoi(num_str);
            }
            info.atomo = ATOMO_CONSTINT;
            return info;
        }

        // Constante Caractere: 'c'
        if (c == '\'') {
            int char_val = fgetc(arquivo_fonte);
            if (char_val == EOF || char_val == '\n') {
                erro_lexico("constante caractere mal formatada");
            }
            int fecho = fgetc(arquivo_fonte);
            if (fecho != '\'') {
                erro_lexico("esperado ' para fechar constante caractere");
            }
            info.atomo = ATOMO_CONSTCHAR;
            info.atributo.ch = (char)char_val;
            return info;
        }

        // Operadores e Símbolos Especiais
        if (c == ';') { info.atomo = ATOMO_PONTO_VIRGULA; return info; }
        if (c == '.') { info.atomo = ATOMO_PONTO; return info; }
        if (c == ',') { info.atomo = ATOMO_VIRGULA; return info; }
        if (c == '(') { info.atomo = ATOMO_ABRE_PAR; return info; }
        if (c == ')') { info.atomo = ATOMO_FECHA_PAR; return info; }
        if (c == '+') { info.atomo = ATOMO_OP_SOMA; return info; }
        if (c == '-') { info.atomo = ATOMO_OP_SUB; return info; }
        if (c == '*') { info.atomo = ATOMO_OP_MULT; return info; }
        if (c == '=') { info.atomo = ATOMO_OP_IGUAL; return info; }

        if (c == ':') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '=') {
                info.atomo = ATOMO_ATRIBUICAO;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_DOIS_PONTOS;
            }
            return info;
        }

        if (c == '<') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '>') {
                info.atomo = ATOMO_OP_DIFERENTE;
            } else if (prox == '=') {
                info.atomo = ATOMO_OP_MENOR_IGUAL;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_OP_MENOR;
            }
            return info;
        }

        if (c == '>') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '=') {
                info.atomo = ATOMO_OP_MAIOR_IGUAL;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_OP_MAIOR;
            }
            return info;
        }

        erro_lexico("caractere invalido ou desconhecido");
    }
}
 
//  ANALISADOR SINTÁTICO (consome e ASDR)
    
void consome(TAtomo atomo_esperado) {
    if (lookahead.atomo == atomo_esperado) {
        lookahead = obter_atomo();
        while (lookahead.atomo == ATOMO_COMENTARIO) {
            lookahead = obter_atomo();
        }
    } else {
        erro_sintatico(atomo_esperado, lookahead);
    }
}

// <programa> ::= algoritmo identificador ';' <bloco> '.'
void programa(void) {
    consome(ATOMO_ALGORITMO);
    consome(ATOMO_IDENTIFICADOR);
    consome(ATOMO_PONTO_VIRGULA);
    bloco();
    consome(ATOMO_PONTO);
}

// <bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco(void) {
    declaracao_variaveis();
    num_globais = endereco_atual;
    int rotulo_principal = 0;
    if (lookahead.atomo == ATOMO_FUNCAO || lookahead.atomo == ATOMO_PROCEDIMENTO) {
        rotulo_principal = proximo_rotulo();
        gera("   DSVS L%d\n", rotulo_principal);
    }
    declaracao_de_rotinas();
    if (rotulo_principal > 0) {
        gera("L%d:NADA\n", rotulo_principal);
    }
    comando_composto();
    int total = num_globais + (usa_mod ? 2 : 0);
    if (total > 0) {
        gera("   DMEM %d\n", total);
    }
    gera("   PARA\n");
}

// <declaração_variáveis> ::= [ var <lista_variaveis> ';' { <lista_variaveis> ';' } ]
void declaracao_variaveis(void) {
    endereco_atual = 0;
    if (lookahead.atomo == ATOMO_VAR) {
        consome(ATOMO_VAR);
        lista_variaveis(CAT_VARIAVEL, 0);
        consome(ATOMO_PONTO_VIRGULA);
        while (lookahead.atomo == ATOMO_IDENTIFICADOR) {
            lista_variaveis(CAT_VARIAVEL, 0);
            consome(ATOMO_PONTO_VIRGULA);
        }
    }
}

// <lista_variaveis> ::= identificador { ',' identificador } ':' <tipo>
void lista_variaveis(TCategoria categoria, int por_referencia) {
    while (1) {
        TInfoAtomo info = lookahead;
        consome(ATOMO_IDENTIFICADOR);
        if (categoria == CAT_PARAMETRO && endereco_atual >= MAX_PARAMETROS) {
            erro_semantico(info.linha, info.atributo.id, "excede o numero maximo de parametros");
        }
        TNo *no = insere(info.atributo.id, categoria, endereco_atual++, info.linha);
        no->por_referencia = por_referencia;
        if (lookahead.atomo != ATOMO_VIRGULA) break;
        consome(ATOMO_VIRGULA);
    }
    consome(ATOMO_DOIS_PONTOS);
    tipo();
}

// <tipo> ::= caractere | inteiro | logico
void tipo(void) {
    if (lookahead.atomo == ATOMO_CARACTERE) {
        consome(ATOMO_CARACTERE);
    } else if (lookahead.atomo == ATOMO_INTEIRO) {
        consome(ATOMO_INTEIRO);
    } else if (lookahead.atomo == ATOMO_LOGICO) {
        consome(ATOMO_LOGICO);
    } else {
        erro_sintatico(ATOMO_INTEIRO, lookahead);
    }
}

// <declaracao_de_rotinas> ::= { <declaração_de_função> | <declaração_de_procedimento> }
void declaracao_de_rotinas(void) {
    while (lookahead.atomo == ATOMO_FUNCAO || lookahead.atomo == ATOMO_PROCEDIMENTO) {
        if (lookahead.atomo == ATOMO_FUNCAO) {
            declaracao_de_funcao();
        } else {
            declaracao_de_procedimento();
        }
    }
}

// <declaração_de_função> ::= funcao <tipo> identificador <parametros_formais> <declaracao_variaveis> <comando_composto>
void declaracao_de_funcao(void) {
    consome(ATOMO_FUNCAO);
    tipo();
    TInfoAtomo info = lookahead;
    consome(ATOMO_IDENTIFICADOR);
    rotina_atual = insere(info.atributo.id, CAT_FUNCAO, 0, info.linha);
    rotina_atual->rotulo = proximo_rotulo();
    gera("L%d:NADA\n", rotina_atual->rotulo);
    gera("   ENPR\n");
    escopo_atual = 1;
    parametros_formais(rotina_atual);
    declaracao_variaveis();
    int num_locais = endereco_atual;
    if (num_locais > 0) {
        gera("   AMEM %d\n", num_locais);
    }
    comando_composto();
    if (num_locais > 0) {
        gera("   DMEM %d\n", num_locais);
    }
    gera("   RTPR %d\n", rotina_atual->num_parametros);
    fecha_escopo();
    rotina_atual = NULL;
}

// <declaracao_de_procedimento> ::= procedimento identificador <parametros_formais> <declaracao_variaveis> <comando_composto>
void declaracao_de_procedimento(void) {
    consome(ATOMO_PROCEDIMENTO);
    TInfoAtomo info = lookahead;
    consome(ATOMO_IDENTIFICADOR);
    rotina_atual = insere(info.atributo.id, CAT_PROCEDIMENTO, 0, info.linha);
    rotina_atual->rotulo = proximo_rotulo();
    gera("L%d:NADA\n", rotina_atual->rotulo);
    gera("   ENPR\n");
    escopo_atual = 1;
    parametros_formais(rotina_atual);
    declaracao_variaveis();
    int num_locais = endereco_atual;
    if (num_locais > 0) {
        gera("   AMEM %d\n", num_locais);
    }
    comando_composto();
    if (num_locais > 0) {
        gera("   DMEM %d\n", num_locais);
    }
    gera("   RTPR %d\n", rotina_atual->num_parametros);
    fecha_escopo();
    rotina_atual = NULL;
}

// <parâmetros_formais> ::= '(' [ <parâmetro_formal> { ';' <parâmetro_formal> } ] ')'
void parametros_formais(TNo *rotina) {
    endereco_atual = 0;
    consome(ATOMO_ABRE_PAR);
    if (lookahead.atomo == ATOMO_VAR || lookahead.atomo == ATOMO_IDENTIFICADOR) {
        parametro_formal();
        while (lookahead.atomo == ATOMO_PONTO_VIRGULA) {
            consome(ATOMO_PONTO_VIRGULA);
            parametro_formal();
        }
    }
    consome(ATOMO_FECHA_PAR);

    int n = endereco_atual;
    rotina->num_parametros = n;
    if (rotina->categoria == CAT_FUNCAO) {
        rotina->endereço = -(n + 3);
    }
    for (int i = 0; i < PRIME_NUMER; i++) {
        for (TNo *no = tabela.entradas[i]; no != NULL; no = no->prox) {
            if (no->ativo && no->escopo == 1 && no->categoria == CAT_PARAMETRO) {
                rotina->parametros_ref[no->endereço] = no->por_referencia;
                no->endereço = -(n + 3 - (no->endereço + 1));
            }
        }
    }
}

// <parâmetro_formal> ::= [var] <lista_variaveis>
void parametro_formal(void) {
    int por_referencia = 0;
    if (lookahead.atomo == ATOMO_VAR) {
        consome(ATOMO_VAR);
        por_referencia = 1;
    }
    lista_variaveis(CAT_PARAMETRO, por_referencia);
}

// <comando_composto> ::= inicio <comando> { ';' <comando> } fim
void comando_composto(void) {
    consome(ATOMO_INICIO);
    comando();
    while (lookahead.atomo == ATOMO_PONTO_VIRGULA) {
        consome(ATOMO_PONTO_VIRGULA);
        comando();
    }
    consome(ATOMO_FIM);
}

// <comando> ::= <comando_atribuição_ou_chamada> | <comando_entrada> | <comando_saida> | <comando_condicional> | <comando_repeticao> | <comando_composto>
void comando(void) {
    if (lookahead.atomo == ATOMO_IDENTIFICADOR) {
        TInfoAtomo info = lookahead;
        consome(ATOMO_IDENTIFICADOR);
        TNo *no = verifica_declarado(info);
        if (lookahead.atomo == ATOMO_ATRIBUICAO) {
            // <comando_atribuição>
            if (no->categoria != CAT_VARIAVEL && no->categoria != CAT_PARAMETRO &&
                !(no->categoria == CAT_FUNCAO && no == rotina_atual)) {
                erro_semantico(info.linha, info.atributo.id, "nao pode receber atribuicao");
            }
            consome(ATOMO_ATRIBUICAO);
            expressao();
            gera_armazena(no);
        } else {
            // <chamada_procedimento> com ou sem parâmetros
            if (no->categoria != CAT_PROCEDIMENTO) {
                erro_semantico(info.linha, info.atributo.id, "nao e um procedimento");
            }
            int qtd = 0;
            if (lookahead.atomo == ATOMO_ABRE_PAR) {
                consome(ATOMO_ABRE_PAR);
                qtd = lista_expressao(no);
                consome(ATOMO_FECHA_PAR);
            }
            if (qtd != no->num_parametros) {
                erro_semantico(info.linha, info.atributo.id, "chamado com numero de argumentos incorreto");
            }
            gera("   CHPR L%d\n", no->rotulo);
        }
    } else if (lookahead.atomo == ATOMO_LEIA) {
        comando_entrada();
    } else if (lookahead.atomo == ATOMO_ESCREVA) {
        comando_saida();
    } else if (lookahead.atomo == ATOMO_SE) {
        comando_condicional();
    } else if (lookahead.atomo == ATOMO_ENQUANTO) {
        comando_repeticao();
    } else if (lookahead.atomo == ATOMO_INICIO) {
        comando_composto();
    } else {
        erro_sintatico(ATOMO_IDENTIFICADOR, lookahead);
    }
}

// <comando_entrada> ::= leia '(' identificador { ',' identificador } ')'
void comando_entrada(void) {
    consome(ATOMO_LEIA);
    consome(ATOMO_ABRE_PAR);
    while (1) {
        TInfoAtomo info = lookahead;
        consome(ATOMO_IDENTIFICADOR);
        TNo *no = verifica_declarado(info);
        if (no->categoria != CAT_VARIAVEL && no->categoria != CAT_PARAMETRO) {
            erro_semantico(info.linha, info.atributo.id, "deve ser variavel ou parametro");
        }
        gera("   LEIT\n");
        gera_armazena(no);
        if (lookahead.atomo != ATOMO_VIRGULA) break;
        consome(ATOMO_VIRGULA);
    }
    consome(ATOMO_FECHA_PAR);
}

// <comando_saida> ::= escreva '(' <lista_expressão> ')'
void comando_saida(void) {
    consome(ATOMO_ESCREVA);
    consome(ATOMO_ABRE_PAR);
    expressao();
    gera("   IMPR\n");
    while (lookahead.atomo == ATOMO_VIRGULA) {
        consome(ATOMO_VIRGULA);
        expressao();
        gera("   IMPR\n");
    }
    consome(ATOMO_FECHA_PAR);
}

// <comando_condicional> ::= se <expressão> entao <comando> [ senao <comando> ]
void comando_condicional(void) {
    consome(ATOMO_SE);
    expressao();
    int rotulo_senao = proximo_rotulo();
    gera("   DSVF L%d\n", rotulo_senao);
    consome(ATOMO_ENTAO);
    comando();
    if (lookahead.atomo == ATOMO_SENAO) {
        int rotulo_fim = proximo_rotulo();
        gera("   DSVS L%d\n", rotulo_fim);
        gera("L%d:NADA\n", rotulo_senao);
        consome(ATOMO_SENAO);
        comando();
        gera("L%d:NADA\n", rotulo_fim);
    } else {
        gera("L%d:NADA\n", rotulo_senao);
    }
}

// <comando_repeticao> ::= enquanto <expressão> faca <comando>
void comando_repeticao(void) {
    int rotulo_inicio = proximo_rotulo();
    int rotulo_fim = proximo_rotulo();
    gera("L%d:NADA\n", rotulo_inicio);
    consome(ATOMO_ENQUANTO);
    expressao();
    gera("   DSVF L%d\n", rotulo_fim);
    consome(ATOMO_FACA);
    comando();
    gera("   DSVS L%d\n", rotulo_inicio);
    gera("L%d:NADA\n", rotulo_fim);
}

// <lista_expressão> ::= <expressão> { ',' <expressão> }
int lista_expressao(TNo *rotina) {
    int qtd = 0;
    argumento(rotina, qtd++);
    while (lookahead.atomo == ATOMO_VIRGULA) {
        consome(ATOMO_VIRGULA);
        argumento(rotina, qtd++);
    }
    return qtd;
}

void argumento(TNo *rotina, int indice) {
    if (indice < rotina->num_parametros && rotina->parametros_ref[indice]) {
        if (lookahead.atomo != ATOMO_IDENTIFICADOR) {
            erro_semantico(lookahead.linha, rotina->ID, "parametro por referencia exige variavel como argumento");
        }
        TInfoAtomo info = lookahead;
        consome(ATOMO_IDENTIFICADOR);
        TNo *no = verifica_declarado(info);
        if (no->categoria != CAT_VARIAVEL && no->categoria != CAT_PARAMETRO) {
            erro_semantico(info.linha, info.atributo.id, "deve ser variavel ou parametro");
        }
        if (lookahead.atomo != ATOMO_VIRGULA && lookahead.atomo != ATOMO_FECHA_PAR) {
            erro_semantico(info.linha, rotina->ID, "parametro por referencia exige variavel como argumento");
        }
        if (no->por_referencia) {
            gera("   CRVL %d,%d\n", no->escopo, no->endereço);
        } else {
            gera("   CREN %d,%d\n", no->escopo, no->endereço);
        }
    } else {
        expressao();
    }
}

// <expressão> ::= <expressão_simples> [ <operador_relacional> <expressão_simples> ]
void expressao(void) {
    expressao_simples();
    if (lookahead.atomo == ATOMO_OP_DIFERENTE || lookahead.atomo == ATOMO_OP_MENOR_IGUAL ||
        lookahead.atomo == ATOMO_OP_MAIOR_IGUAL || lookahead.atomo == ATOMO_OP_MENOR ||
        lookahead.atomo == ATOMO_OP_MAIOR || lookahead.atomo == ATOMO_OP_IGUAL) {
        TAtomo op = lookahead.atomo;
        operador_relacional();
        expressao_simples();
        if (op == ATOMO_OP_IGUAL) gera("   CMIG\n");
        else if (op == ATOMO_OP_DIFERENTE) gera("   CMDG\n");
        else if (op == ATOMO_OP_MENOR) gera("   CMME\n");
        else if (op == ATOMO_OP_MENOR_IGUAL) gera("   CMEG\n");
        else if (op == ATOMO_OP_MAIOR) gera("   CMMA\n");
        else gera("   CMAG\n");
    }
}

void operador_relacional(void) {
    if (lookahead.atomo == ATOMO_OP_DIFERENTE || lookahead.atomo == ATOMO_OP_MENOR_IGUAL ||
        lookahead.atomo == ATOMO_OP_MAIOR_IGUAL || lookahead.atomo == ATOMO_OP_MENOR ||
        lookahead.atomo == ATOMO_OP_MAIOR || lookahead.atomo == ATOMO_OP_IGUAL) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_IGUAL, lookahead);
    }
}

// <expressão_simples> ::= <termo> { <operador_adição> <termo> }
void expressao_simples(void) {
    termo();
    while (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB ||
           lookahead.atomo == ATOMO_MOD || lookahead.atomo == ATOMO_OU) {
        TAtomo op = lookahead.atomo;
        operador_adicao();
        termo();
        if (op == ATOMO_OP_SOMA) gera("   SOMA\n");
        else if (op == ATOMO_OP_SUB) gera("   SUBT\n");
        else if (op == ATOMO_OU) gera("   DISJ\n");
        else {
            usa_mod = 1;
            gera("   ARMZ 0,%d\n", num_globais + 1);
            gera("   ARMZ 0,%d\n", num_globais);
            gera("   CRVL 0,%d\n", num_globais);
            gera("   CRVL 0,%d\n", num_globais);
            gera("   CRVL 0,%d\n", num_globais + 1);
            gera("   DIVI\n");
            gera("   CRVL 0,%d\n", num_globais + 1);
            gera("   MULT\n");
            gera("   SUBT\n");
        }
    }
}

void operador_adicao(void) {
    if (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB ||
        lookahead.atomo == ATOMO_MOD || lookahead.atomo == ATOMO_OU) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_SOMA, lookahead);
    }
}

// <termo> ::= <fator> { <operador_multiplicação> <fator> }
void termo(void) {
    fator();
    while (lookahead.atomo == ATOMO_OP_MULT || lookahead.atomo == ATOMO_DIV ||
           lookahead.atomo == ATOMO_E) {
        TAtomo op = lookahead.atomo;
        operador_multiplicacao();
        fator();
        if (op == ATOMO_OP_MULT) gera("   MULT\n");
        else if (op == ATOMO_DIV) gera("   DIVI\n");
        else gera("   CONJ\n");
    }
}

void operador_multiplicacao(void) {
    if (lookahead.atomo == ATOMO_OP_MULT || lookahead.atomo == ATOMO_DIV ||
        lookahead.atomo == ATOMO_E) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_MULT, lookahead);
    }
}

// <fator> ::= identificador [ '(' <lista_expressão> ')' ] | constint | constchar | '(' <expressão> ')' | ( '+' | '-' | nao ) <fator> | verdadeiro | falso
void fator(void) {
    if (lookahead.atomo == ATOMO_IDENTIFICADOR) {
        TInfoAtomo info = lookahead;
        consome(ATOMO_IDENTIFICADOR);
        TNo *no = verifica_declarado(info);
        if (no->categoria == CAT_PROCEDIMENTO) {
            erro_semantico(info.linha, info.atributo.id, "procedimento nao pode ser usado em expressao");
        }
        if (lookahead.atomo == ATOMO_ABRE_PAR && no->categoria != CAT_FUNCAO) {
            erro_semantico(info.linha, info.atributo.id, "nao e uma funcao");
        }
        if (no->categoria == CAT_FUNCAO) {
            gera("   AMEM 1\n");
            int qtd = 0;
            if (lookahead.atomo == ATOMO_ABRE_PAR) {
                consome(ATOMO_ABRE_PAR);
                qtd = lista_expressao(no);
                consome(ATOMO_FECHA_PAR);
            }
            if (qtd != no->num_parametros) {
                erro_semantico(info.linha, info.atributo.id, "chamado com numero de argumentos incorreto");
            }
            gera("   CHPR L%d\n", no->rotulo);
        } else if (no->por_referencia) {
            gera("   CRVI %d,%d\n", no->escopo, no->endereço);
        } else {
            gera("   CRVL %d,%d\n", no->escopo, no->endereço);
        }
    } else if (lookahead.atomo == ATOMO_CONSTINT) {
        gera("   CRCT %d\n", lookahead.atributo.numero);
        consome(ATOMO_CONSTINT);
    } else if (lookahead.atomo == ATOMO_CONSTCHAR) {
        gera("   CRCT %d\n", (unsigned char)lookahead.atributo.ch);
        consome(ATOMO_CONSTCHAR);
    } else if (lookahead.atomo == ATOMO_ABRE_PAR) {
        consome(ATOMO_ABRE_PAR);
        expressao();
        consome(ATOMO_FECHA_PAR);
    } else if (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB || lookahead.atomo == ATOMO_NAO) {
        TAtomo op = lookahead.atomo;
        consome(lookahead.atomo);
        fator();
        if (op == ATOMO_OP_SUB) gera("   INVR\n");
        else if (op == ATOMO_NAO) gera("   NEGA\n");
    } else if (lookahead.atomo == ATOMO_VERDADEIRO) {
        gera("   CRCT 1\n");
        consome(ATOMO_VERDADEIRO);
    } else if (lookahead.atomo == ATOMO_FALSO) {
        gera("   CRCT 0\n");
        consome(ATOMO_FALSO);
    } else {
        erro_sintatico(ATOMO_IDENTIFICADOR, lookahead);
    }
}
 
//  FUNÇÃO PRINCIPAL
    
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    arquivo_fonte = fopen(argv[1], "r");
    if (!arquivo_fonte) {
        printf("Erro ao abrir arquivo fonte: %s\n", argv[1]);
        return 1;
    }

    // Inicializa o lookahead com o primeiro átomo
    lookahead = obter_atomo();
    while (lookahead.atomo == ATOMO_COMENTARIO) {
        lookahead = obter_atomo();
    }

    // Executa a análise sintática
    programa();

    if (arquivo_fonte) fclose(arquivo_fonte);

    imprime_codigo();
    imprime_tabela();
    libera_tabela();
    libera_codigo();

    return 0;
}
