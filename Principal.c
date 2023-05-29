#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#pragma comment(lib,"pthreadVC2.lib")

#define HAVE_STRUCT_TIMESPEC 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*Definição da quantidade de linhas e colunas de uma matriz N x M*/

#define LIN 10000
#define COL 10000

/*Definição dos valores de verdadeiro (1) e falso (0) */

#define FALSE 0
#define TRUE 1

/*Definição da quantidade de Threads implementadas*/

#define NUM_THREADS 8

/*Variáveis globais (matriz e contador de números primos)*/

int** mat;
int contadorPrimos = 0;

/*Estrutura que contem dados sobre as linhas de início e término para a
leitura da matriz por uma thread */

typedef struct
{
	int linhaInicial;
	int linhaFinal;
} LinhasMacrobloco;

/*Variável global mutex*/

pthread_mutex_t mutex;

/*Função que retorna se o valor é primo ou não*/

int ehPrimo(int numero);

/*Funções de alocação e desalocação de valores de memória da matriz*/

int** alocarMatriz();
int** desalocarMatriz();

/*Função de inserimento de valores randomicos na matriz (0 a 31999)*/

void inserirValores();

/*Funções de contagem do modo Serial e Paralelo*/

void contagemSerial();
void contagemParalela();

/*Função usada pelas Threads para a contagem de valores primos*/

void* contador(void*);

int main(int argc, char* argv[])
{

	double tempo1, tempo2;/*Tempo marcado de cada contagem*/
	clock_t tInicio, tFim;/*Cronometros de início e fim*/

	mat = alocarMatriz(); /*Aloca a memória para a matriz*/

	inserirValores(); /*Insere os valores randomicos na matriz*/

	tInicio = clock(); /*Inicia o cronômetro*/

	contagemSerial(); /*Inicia a função de contagem do estilo Serial*/

	tFim = clock(); /*Termina o cronômetro*/

	tempo1 = (double)(tFim - tInicio) / CLOCKS_PER_SEC; /*Valor de segundos do primeiro tempo marcado*/

	printf("Quantidade de elementos primos na matriz por busca serial: %d\n", contadorPrimos);
	printf("Tempo total de contagem: %f segundos\n\n", tempo1);

	contadorPrimos = 0; /*Zera o contador de números primos*/

	tInicio = clock(); /*Inicia o cronômetro*/

	contagemParalela(); /*Inicia a função de contagem do estilo Paralelo*/

	tFim = clock(); /*Termina o cronômetro*/

	tempo2 = (double)(tFim - tInicio) / CLOCKS_PER_SEC; /*Valor de segundos do primeiro tempo marcado*/

	printf("Quantidade de elementos primos na matriz por busca paralela: %d\n", contadorPrimos);
	printf("Tempo total de contagem: %f segundos\n", tempo2);

	mat = desalocarMatriz(); /*Desaloca a memória da matriz*/

	return 0;

}

int ehPrimo(int numero)
{

	int i;

	/*Se o número for zero ou um, retorna falso*/

	if (numero == 0 || numero == 1) return FALSE;

	/*Dos valores 2 a raiz do número, se o resto do número pelo valor for 
	igual a zero, retorna falso*/

	for (i = 2; i <= sqrt(numero); i++)
	{
		if (numero % i == 0) return FALSE;
	}

	/*Caso nada aconteceu, retorna verdadeiro*/

	return TRUE;

}

int** alocarMatriz()
{

	int** mat; /*Instancia um valor local para matriz*/
	int i;

	/*Faz a alocação dinâmica de um vetor do tamnho da linha, com 
	ponteiro de inteiros*/

	mat = malloc(LIN * sizeof(int*));

	/*Caso o mat for nulo, ouve erro na alocação*/

	if (mat == NULL)
	{
		printf("** Erro: Memória Insuficiente **");
		return NULL;
	}

	/*Para cada valor desse vetor, se faz a alocação dinâmica do tamanho de
	colunas da matriz. Caso der nulo um dos ponteiros, ouve erro de alocação*/

	for (i = 0; i < LIN; i++)
	{

		mat[i] = malloc(COL * sizeof(int));

		if (mat[i] == NULL)
		{
			printf("** Erro: Memória Insuficiente **");
			return NULL;
		}

	}

	return mat; /*Retorna a matriz*/

}

int** desalocarMatriz()
{

	int i;

	if (mat == NULL) return NULL; /*Caso a matriz for nula, retorna nulo*/

	for (i = 0; i < LIN; i++) free(mat[i]); /*Para cada elemento no vetor de linhas da matriz, de um free()*/

	free(mat); /*De um free() no vetor de linhas*/

	return NULL; /*Retorne nulo*/

}

void inserirValores()
{

	int i, j;
	time_t t;

	/*Inicia o gerador de números randômicos*/

	srand((unsigned)time(&t));

	/*Para cada elemento na matriz, instancia um valor randômico de 
	0 a 31999*/

	for (i = 0; i < LIN; i++)
	{
		for (j = 0; j < COL; j++)
		{
			mat[i][j] = (rand() % 32000);
		}
	}

}

void contagemSerial()
{

	int i, j;

	/*Para cada elemento presente na matriz, se o elemento
	for primo, soma 1 ao contador de números primos*/

	for (i = 0; i < LIN; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (ehPrimo(mat[i][j])) contadorPrimos++;
		}
	}

}

void contagemParalela()
{

	int i;

	pthread_t tid[NUM_THREADS]; /*Vetor de indentificadores da Thread*/
	LinhasMacrobloco linhas[NUM_THREADS]; /*Vetor das estruturas delimitação do macrobloco*/

	pthread_mutex_init(&mutex, NULL); /*Cria e iniaciliza o mutex lock*/

	int quantidadeLinhas = LIN / NUM_THREADS;
	int linhasExtras = LIN % NUM_THREADS;
	int linhaInicial = 0;

	/*Criação das Threads com o argumento recebido sendo a Struct contendo
	as linhas de início e fim da leitura*/

	for (i = 0; i < NUM_THREADS; i++)
	{

		int linhaFinal = linhaInicial + quantidadeLinhas;

		if (linhasExtras > 0)
		{
			linhaFinal++;
			linhasExtras--;
		}

		linhas[i].linhaInicial = linhaInicial;
		linhas[i].linhaFinal = linhaFinal;

		if (pthread_create(&(tid[i]), NULL, contador, (void*)&linhas[i]) != 0)
		{
			perror("Pthread_create falhou!");
			exit(1);
		}

		linhaInicial = linhaFinal;
	}

	/*Join nas Threads*/

	for (i = 0; i < NUM_THREADS; i++)
	{
		if (pthread_join((tid[i]), NULL) != 0)
		{
			perror("Pthread_join falhou!");
			exit(1);
		}

	}

	pthread_mutex_destroy(&mutex); /*Desaloca as estruturas do mutex*/

}

void* contador(void* arg)
{

	int i, j;

	int contadorLocal = 0; /*Inicia um contador de números primos local*/

	LinhasMacrobloco* linhas = (LinhasMacrobloco*)arg;

	int linhaInicial = linhas->linhaInicial;
	int linhaFinal = linhas->linhaFinal;

	for (i = linhaInicial; i < linhaFinal; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (ehPrimo(mat[i][j])) contadorLocal++;
		}
	}

	pthread_mutex_lock(&mutex); /*Adiquire o mutex lock*/

	contadorPrimos += contadorLocal; /*Soma o valor local ao valor global*/

	pthread_mutex_unlock(&mutex); /*Libera o mutex lock*/

	pthread_exit(0); /*Ternina a Thread, retornando 0*/

}