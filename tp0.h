typedef struct Retornos_diarios {
    int dia;
    int mes;
    int ano;
    float valor;
} Retornos_diarios;

typedef struct Resultados {
    FILE *arquivo_moeda;
    Retornos_diarios *retorno;
    char DataMenor[11];
    char DataMaior[11];
    float MenorRetorno;
    float MaiorRetorno;
    float Media;
    float Desvio;
    float RTotal;
    float Sharpe;
    int numero_dias;
    float *proporcoes;
} Resultados;

void calcula_retornos(Resultados *resultado_moeda);

void define_proporcoes_aleatorias(Resultados *resultado_portfolio, int n, int N_moedas);

void calcula_portfolios(Resultados *resultado_portfolio, Resultados *resultado_moeda, int N_moedas);

void imprime_resultados(FILE *ResultadoAnalise, Resultados *resultado_moeda, Resultados *resultado_portfolio, int N_moedas, char **argv, int NUM_PORTFOLIOS);
