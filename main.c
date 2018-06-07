/************************************************************/
/* ALGORITMOS E ESTRUTURAS DE DADOS II                      */
/* TP0 - Analise de Series Temporais de criptomoedas        */
/* Autor: Mike Dornas Oliveira                              */
/* Data: 02/04/2018                                         */
/************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tp0.h"

#define NUM_PORTFOLIOS 4

int main(int argc, char *argv[])
{
//Teste dos parametros passados
    if(argc < 2)
    {
        printf("\nParametros insuficientes, passe os codigos para as criptomoedas desejadas.\n");
        return 1;
    }

    Resultados *resultado_moeda = (Resultados*) calloc(argc - 1, sizeof(Resultados));               //Cria uma struct com os resultados para cada moeda
    Resultados *resultado_portfolio = (Resultados*) calloc(NUM_PORTFOLIOS, sizeof(Resultados));     //Struct para os resultados dos portfolios

/*Abrindo arquivos das criptomoedas

CONSIDEREI QUE AO EXECUTAR O PROGRAMA, OS ARQUIVOS COM OS DADOS
DAS CRIPTOMOEDAS ESTARIAM EM UMA PASTA "Moedas", CONFORME FOI
DISPONIBILIZADO NO MOODLE, LOCALIZADA NO MESMO DIRETORIO DO EXECUTAVEL
*/
    for(int i = 0; i < argc - 1; i++)
    {
        char codigoMoeda[23];
        sprintf(codigoMoeda, "Moedas//%s.csv", argv[i+1]);
        resultado_moeda[i].arquivo_moeda = fopen(codigoMoeda, "r");
        if(resultado_moeda[i].arquivo_moeda == NULL)
        {
            printf("\nArquivo %s nao encontrado! Passe o codigo correto para as criptomoedas desejadas\n", argv[i+1]);
            return 1;
        }
    }

//Cria arquivo de resultados
    FILE *ResultadoAnalise = fopen("saida.txt", "w");
    if(ResultadoAnalise == NULL)
    {
        printf("\n Erro ao criar arquivo!\n");
        return 1;
    }

// Contabilidade das moedas individuais
    for(int i = 0; i < (argc - 1); i++)
        calcula_retornos(&resultado_moeda[i]);

// Contabilidade dos portfolios
    srand(time(NULL));
    for(int i = 0; i < NUM_PORTFOLIOS; i++)
    {
        define_proporcoes_aleatorias(&resultado_portfolio[i], i, (argc - 1));
        calcula_portfolios(&resultado_portfolio[i], resultado_moeda, (argc - 1));
    }

//Imprime os resultados no arquivo
    imprime_resultados(ResultadoAnalise, resultado_moeda, resultado_portfolio, (argc - 1), argv, NUM_PORTFOLIOS);

//Fechando arquivos
    for(int i = 0; i < argc - 1; i++)
    {
        fclose(resultado_moeda[i].arquivo_moeda);
        free(resultado_moeda[i].retorno);
    }

    for(int i = 0; i < NUM_PORTFOLIOS; i++)
    {
        free(resultado_portfolio[i].retorno);
        free(resultado_portfolio[i].proporcoes);
    }

    free(resultado_moeda);
    free(resultado_portfolio);
    fclose(ResultadoAnalise);

    return 0;
}
