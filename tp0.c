#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "tp0.h"

#define R_F 0.038     //Taxa livre de risco -> 0.038%

void calcula_retornos(Resultados *resultado_moeda)
{
    float cotacao_do_dia[2];
    int aux = 0, i = 0;

    fscanf(resultado_moeda->arquivo_moeda, "%d\n", &resultado_moeda->numero_dias);

// Malloc para salvar todas os retornos de cada moeda
    resultado_moeda->retorno = (Retornos_diarios*) malloc(resultado_moeda->numero_dias * sizeof(Retornos_diarios));

    fscanf(resultado_moeda->arquivo_moeda, "%d/%d/%d,%f\n", &resultado_moeda->retorno[0].mes,
           &resultado_moeda->retorno[0].dia, &resultado_moeda->retorno[0].ano, &cotacao_do_dia[0]);

// Calculo do RETORNO TOTAL
    resultado_moeda->RTotal = cotacao_do_dia[0];

// Calculo dos RETORNOS DIARIOS
    for(i = 1; i < resultado_moeda->numero_dias; i++)
    {
        fscanf(resultado_moeda->arquivo_moeda, "%d/%d/%d,%f\n", &resultado_moeda->retorno[i].mes,
           &resultado_moeda->retorno[i].dia, &resultado_moeda->retorno[i].ano, &cotacao_do_dia[i%2]);

        aux = abs(i%2 - 1);
            //aux -> correspondera a posicao da cotacao mais RECENTE
            //i%2 -> correspondera a posicao da cotacao mais ANTIGA

        resultado_moeda->retorno[i - 1].valor = ((cotacao_do_dia[aux] - cotacao_do_dia[i%2]) / cotacao_do_dia[i%2]) * 100;

// Calculo da MEDIA
        resultado_moeda->Media += resultado_moeda->retorno[i - 1].valor;

        if(resultado_moeda->retorno[i - 1].valor > resultado_moeda->MaiorRetorno)
        {
            resultado_moeda->MaiorRetorno = resultado_moeda->retorno[i - 1].valor;
            sprintf(resultado_moeda->DataMaior, "%02d/%02d/20%d", resultado_moeda->retorno[i - 1].dia,
                    resultado_moeda->retorno[i - 1].mes, resultado_moeda->retorno[i - 1].ano); //Coloca a data na ordem correta DIA/MES/ANO e salva em uma string
        }
        else if(resultado_moeda->retorno[i - 1].valor < resultado_moeda->MenorRetorno)
        {
            resultado_moeda->MenorRetorno = resultado_moeda->retorno[i - 1].valor;
            sprintf(resultado_moeda->DataMenor, "%02d/%02d/20%d", resultado_moeda->retorno[i - 1].dia,
                    resultado_moeda->retorno[i - 1].mes, resultado_moeda->retorno[i - 1].ano);
        }
    }

    resultado_moeda->Media /= (resultado_moeda->numero_dias - 1);   //Calculo da MEDIA

// Calculo do DESVIO
    for(int j = 0; j < resultado_moeda->numero_dias - 1; j++)
        resultado_moeda->Desvio += pow(resultado_moeda->retorno[j].valor - resultado_moeda->Media, 2);
    resultado_moeda->Desvio = sqrt(resultado_moeda->Desvio / (resultado_moeda->numero_dias - 1));

    resultado_moeda->RTotal /= cotacao_do_dia[(i-1)%2];
    resultado_moeda->Sharpe = ((resultado_moeda->Media - R_F) / resultado_moeda->Desvio);   // Calculo do SHARPE
}

void define_proporcoes_aleatorias(Resultados *resultado_portfolio, int n, int N_moedas)
{
    float aux = 1;
    resultado_portfolio->proporcoes = (float*) calloc(N_moedas, sizeof(float));
    if(n == 0)  //Define as proporcoes do portfolio 1
        for(int i = 0; i < N_moedas; i++)
            resultado_portfolio->proporcoes[i] = (float) 1 / N_moedas;
    else
    {
        for(int i = 0; i < N_moedas - 1; i++) //Define as proporcoes dos demais portfolios
        {
            resultado_portfolio->proporcoes[i] = aux * (((float) rand()) / RAND_MAX);
            aux -= resultado_portfolio->proporcoes[i];
        }
        resultado_portfolio->proporcoes[N_moedas - 1] = aux;
    }
}

void calcula_portfolios(Resultados *resultado_portfolio, Resultados *resultado_moeda, int N_moedas)
{
    resultado_portfolio->RTotal = 1;
//Aloca memoria para salvar todos os retornos diarios do portfolio
    resultado_portfolio->retorno = (Retornos_diarios*) calloc((resultado_moeda[0].numero_dias - 1), sizeof(Retornos_diarios));

    for(int i = 0; i < (resultado_moeda[0].numero_dias - 1); i++)
    {
        for(int j = 0; j < N_moedas; j++)
            resultado_portfolio->retorno[i].valor += resultado_portfolio->proporcoes[j] * resultado_moeda[j].retorno[i].valor;

        resultado_portfolio->Media += resultado_portfolio->retorno[i].valor;  // Calculo da MEDIA

        resultado_portfolio->RTotal *= (1 + (resultado_portfolio->retorno[i].valor / 100));  // Calculo do RETORNO TOTAL

        if(resultado_portfolio->retorno[i].valor > resultado_portfolio->MaiorRetorno)
        {
            resultado_portfolio->MaiorRetorno = resultado_portfolio->retorno[i].valor;
            sprintf(resultado_portfolio->DataMaior, "%02d/%02d/20%d", resultado_moeda[0].retorno[i].dia,
                    resultado_moeda[0].retorno[i].mes, resultado_moeda[0].retorno[i].ano);  //Coloca a data na ordem correta DIA/MES/ANO e salva em uma string
        }
        else if(resultado_portfolio->retorno[i].valor < resultado_portfolio->MenorRetorno)
        {
            resultado_portfolio->MenorRetorno = resultado_portfolio->retorno[i].valor;
            sprintf(resultado_portfolio->DataMenor, "%02d/%02d/20%d", resultado_moeda[0].retorno[i].dia,
                    resultado_moeda[0].retorno[i].mes, resultado_moeda[0].retorno[i].ano);
        }
    }

    resultado_portfolio->Media /= (resultado_moeda[0].numero_dias - 1); // Calculo da MEDIA

// Calculo do DESVIO
    for(int j = 0; j < (resultado_moeda[0].numero_dias - 1); j++)
        resultado_portfolio->Desvio += pow(resultado_portfolio->retorno[j].valor - resultado_portfolio->Media, 2);
    resultado_portfolio->Desvio = sqrt(resultado_portfolio->Desvio / (resultado_moeda[0].numero_dias - 1));

    resultado_portfolio->Sharpe = ((resultado_portfolio->Media - R_F) / resultado_portfolio->Desvio);   // Calculo do SHARPE
}

void imprime_resultados(FILE *ResultadoAnalise, Resultados *resultado_moeda, Resultados *resultado_portfolio, int N_moedas, char **argv, int NUM_PORTFOLIOS)
{
    fprintf(ResultadoAnalise, " Moeda\t\t DataMenor\tDataMaior\tMenor\t \tMaior\t\t Media\t\tDesvio\t\tRTotal\t\tSharpe\n");

    for(int i = 0; i < N_moedas; i++)
    {
        fprintf(ResultadoAnalise, " %s \t\t %s \t%s \t%.2f %%\t\t%.2f %%\t\t%.2f %%\t\t%.2f\t\t%.2f\t\t%.2f\n",
                argv[i + 1], resultado_moeda[i].DataMenor, resultado_moeda[i].DataMaior, resultado_moeda[i].MenorRetorno,
                resultado_moeda[i].MaiorRetorno, resultado_moeda[i].Media, resultado_moeda[i].Desvio, resultado_moeda[i].RTotal,
                resultado_moeda[i].Sharpe);
    }
    for(int i = 0; i < NUM_PORTFOLIOS; i++)
    {
       fprintf(ResultadoAnalise, " Portifolio %d\t %s \t%s \t%.2f %%\t\t%.2f %%\t\t%.2f %%\t\t%.2f\t\t%.2f\t\t%.2f\n", i+1,
                resultado_portfolio[i].DataMenor, resultado_portfolio[i].DataMaior, resultado_portfolio[i].MenorRetorno,
                resultado_portfolio[i].MaiorRetorno, resultado_portfolio[i].Media, resultado_portfolio[i].Desvio, resultado_portfolio[i].RTotal,
                resultado_portfolio[i].Sharpe);
    }
    fprintf(ResultadoAnalise, "\n Proporcoes:");
    for(int i = 0; i < NUM_PORTFOLIOS; i++)
    {
        fprintf(ResultadoAnalise, "\n Portfolio %d: ", i+1);
        for(int j = 0; j < N_moedas; j++)
        {
            fprintf(ResultadoAnalise, "%.2f   ", resultado_portfolio[i].proporcoes[j]);
        }
    }
    printf("\nArquivo de resultados criado\n");
}
