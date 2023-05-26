#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*Definidor da matriz N x N*/	

#define MAX 10000

/*Variáveis globais*/

int** mat;
int qtdNumerosPrimos = 0;

/*Funções usadas*/

int ehPrimo(int numero);
int** alocarMatriz();
int** desalocarMatriz();
void inserirValoresEContarNumeros();

int main(int argc, char* argv[])
{
	double tempo;
	clock_t tInicio, tFim;
	mat = alocarMatriz(); /*Aloca dinâmicamento a memória para a matriz*/
	tInicio = clock();
	inserirValoresEContarNumeros(); /*Insere os valores randomicos de 0 a 31999 na matriz e conta a quantidade de valores primos presentes na matriz*/
	tFim = clock();
	tempo = (double)(tFim - tInicio)/CLOCKS_PER_SEC;
	printf("Quantidade de elementos primos na matriz: %d\n", qtdNumerosPrimos); /*Informa a quantidade de elementos na lista*/
	printf("Tempo total de contagem: %f segundos (%.2f%% do tempo)", tempo, (tempo/tempo)*100);	
	mat = desalocarMatriz(); /*Desaloca a memória da matriz*/
	return 0;
}

int ehPrimo(int numero)
{
	int i;
	if (numero == 0 || numero == 1) return 0; /*Retorna falso caso os valores forem 0 ou 1*/
	for (i = 2; i < numero; i++)
	{
		if (numero % i == 0) return 0; /*Caso o resto da divisão entre o número e i for zero, o número não é primo (falso)*/
	}
	return 1; /*Caso tenha se passado pelo laço for e não entrou no if, o número é primo (verdadeiro)*/
}

int** alocarMatriz()
{
	int** mat;
	int i;
	mat = malloc(MAX * sizeof(int*)); /*Aloca um vetor de ponteiros para as linhas*/
	if (mat == NULL) /*Caso o vetor for nulo, retorna o erro*/
	{
		printf("** Erro: Memória Insuficiente **");
		return NULL;
	}
	for (i = 0; i < MAX; i++)
	{
		mat[i] = malloc(MAX * sizeof(int)); /*Aloca um vetor para cada uma das linhas*/
		if (mat[i] == NULL) /*Caso o vetor for nulo, retorna o erro*/
		{
			printf("** Erro: Memória Insuficiente **");
			return NULL;
		}
	}
	return mat;/*Retorna a matriz*/
}

int** desalocarMatriz()
{
	int i;
	if (mat == NULL) return NULL; /*Se a matriz for nula, retorne nulo*/
	for (i = 0; i < MAX; i++) free(mat[i]); /*Para cada elemento no vetor de linhas da matriz, se desaloca com free()*/
	free(mat); /*Desaloca o vetor de linhas com o free()*/
	return NULL;
}

void inserirValoresEContarNumeros()
{
	int i, j;
	time_t t;
	srand((unsigned)time(&t)); /*Inicia o gerador de números randomicos*/
	for (i = 0; i < MAX; i++)
	{
		for (j = 0; j < MAX; j++)
		{
			mat[i][j] = (rand() % 32000); /*Para cada elemento na matriz, insere um valor randomico de 0 a 31999*/
			if (ehPrimo(mat[i][j])) qtdNumerosPrimos++; /*Para cada elemento na matriz, se o elemento for primo soma 1 ao contador*/
		}
	}
}