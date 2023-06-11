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

#define LIN 9
#define COL 9

/*Definição da quantidade de linhas e colunas do macrobloco*/

#define MLIN 3
#define MCOL 3

/*Definição do numero de macroblocos por linha e coluna*/

#define MACROLINHAS (LIN / MLIN)
#define MACROCOLUNAS (COL / MCOL)

/*Definição dos valores de verdadeiro (1) e falso (0) */

#define FALSE 0
#define TRUE 1

/*Definição da quantidade de Threads implementadas*/

#define NUM_THREADS 16

/*Variáveis globais (matriz, vetor para rastrear quais macroblocos ja foram processados e contador de números primos)*/

int** mat;
int macroblocosProcessados[MACROLINHAS][MACROCOLUNAS] = { 0 };
int contadorPrimos = 0;

/*Estrutura que contem dados sobre as linhas de início e término para a
leitura da matriz por uma thread */

typedef struct
{
	int id;
} Macrobloco;

/*Variáveis globais mutex*/

pthread_mutex_t contadorMutex;
pthread_mutex_t macroblocoMutex;


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

	/*Inicia o gerador de números randômicos, com a semente de valor 200*/

	srand(200);

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
	Macrobloco macrobloco[NUM_THREADS]; /*Vetor das estruturas delimitação do macrobloco*/

	pthread_mutex_init(&contadorMutex, NULL);
	pthread_mutex_init(&macroblocoMutex, NULL);

	for (i = 0; i < NUM_THREADS; i++)
	{

		macrobloco[i].id = i;

		if (pthread_create(&(tid[i]), NULL, contador, (void*)&macrobloco[i]) != 0)
		{
			perror("Pthread_create falhou!");
			exit(1);
		}
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

	pthread_mutex_destroy(&contadorMutex);
	pthread_mutex_destroy(&macroblocoMutex);

}

void* contador(void* arg)
{

	int contadorLocal = 0; /*Inicia um contador de números primos local*/

	Macrobloco* macrobloco = (Macrobloco*)arg;

	int id = macrobloco->id;

	for (int i = 0; i < MACROLINHAS; i++)
	{
		for (int j = 0; j < MACROCOLUNAS; j++)
		{

			pthread_mutex_lock(&macroblocoMutex);
			if (macroblocosProcessados[i][j])
			{
				pthread_mutex_unlock(&macroblocoMutex);
				continue;
			}
			macroblocosProcessados[i][j] = 1;
			pthread_mutex_unlock(&macroblocoMutex);

			int linhaInicial = i * MLIN;
			int linhaFinal = linhaInicial + MLIN;
			int colunaInicial = j * MCOL;
			int colunaFinal = colunaInicial + MCOL;

			for (int a = linhaInicial; a < linhaFinal; a++)
			{
				for (int b = colunaInicial; b < colunaFinal; b++)
				{
					if (ehPrimo(mat[a][b]))  
					{
						contadorLocal++;
					}
				}
			}
		}
	}

	pthread_mutex_lock(&contadorMutex);
	contadorPrimos += contadorLocal;
	pthread_mutex_unlock(&contadorMutex);

	pthread_exit(0); /*Ternina a Thread, retornando 0*/

}
