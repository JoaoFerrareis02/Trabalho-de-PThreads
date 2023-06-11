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

#define LINHA 10000 
#define COLUNA 10000 

/*Definição da quantidade de linhas e colunas do macrobloco*/

#define LIHNA_MACROBLOCO 100
#define COLUNA_MACROBLOCO 100

/*Definição do numero de macroblocos por linha e coluna*/

#define QTD_MACR0BLOCO_LINHAS (LINHA / LIHNA_MACROBLOCO) 
#define QTD_MACR0BLOCO_COLUNAS (COLUNA / COLUNA_MACROBLOCO) 

/*Definição dos valores de verdadeiro (1) e falso (0) */

#define FALSE 0
#define TRUE 1

/*Definição da semente de geração de números randômicos*/

#define SEED 200

/*Definição da quantidade de Threads implementadas*/

#define NUM_THREADS 4

/*Variáveis globais (matriz e contador de números primos)*/

int** matriz;
int contadorPrimos = 0;

/*Matriz que represente as regiões de cada macrobloco, todas iniciadas com o valor 0*/

int** matrizRegioes;

/*Variável global mutex*/

pthread_mutex_t mutex;

/*Funções usadas*/

int ehPrimo(int numero);
int** alocarMatriz();
int** desalocarMatriz();
int** alocarMatrizRegioes();
int** desalocarMatrizRegioes();
void contagemSerial();
void contagemParalela();
void* contador(void*);

int main(int argc, char* argv[])
{

	double tempo1, tempo2;/*Tempo marcado de cada contagem*/
	clock_t tInicio, tFim;/*Cronometros de início e fim*/

	matriz = alocarMatriz(); /*Aloca a memória para a matriz, inserindo os valores randomicos de 0 31999*/
	matrizRegioes = alocarMatrizRegioes(); /*Aloca a memória para a matriz de regiões de cada macrobloco, inserindo o valor 0*/ 

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

	matriz = desalocarMatriz(); /*Desaloca a memória da matriz*/
	matrizRegioes = desalocarMatrizRegioes(); /*Desaloca a memória da matriz de regiões*/

	return 0;

}


/*Função que retorna se o valor é primo ou não*/

int ehPrimo(int numero)
{

	int i;

	/*Se o número for menor que 2, retorna falso*/

	if (numero < 2) return FALSE;

	/*Dos valores 2 a raiz do número, se o resto do número pelo valor for
	igual a zero, retorna falso*/

	for (i = 2; i <= sqrt(numero); i++)
	{
		if (numero % i == 0) return FALSE;
	}

	/*Caso nada aconteceu, retorna verdadeiro*/

	return TRUE;

}

/*Funções de alocação e desalocação de valores de memória da matriz*/

int** alocarMatriz()
{

	int** matriz; /*Instancia um valor local para matriz*/
	int i, j;

	/*Inicia o gerador de números randômicos*/

	srand(SEED);

	/*Faz a alocação dinâmica de um vetor do tamnho da linha, com
	ponteiro de inteiros*/

	matriz = malloc(LINHA * sizeof(int*));

	/*Caso a matriz for nulo, ouve erro na alocação*/

	if (matriz == NULL)
	{
		printf("** Erro: Memória Insuficiente **");
		return NULL;
	}

	/*Para cada valor desse vetor, se faz a alocação dinâmica do tamanho de
	colunas da matriz. Caso der nulo um dos ponteiros, ouve erro de alocação*/

	for (i = 0; i < LINHA; i++)
	{

		matriz[i] = malloc(COLUNA * sizeof(int));

		if (matriz[i] == NULL)
		{
			printf("** Erro: Memória Insuficiente **");
			return NULL;
		}

	}

	/*Para cada elemento na matriz, instancia um valor randômico de
	0 a 31999*/

	for (i = 0; i < LINHA; i++)
	{
		for (j = 0; j < COLUNA; j++)
		{
			matriz[i][j] = (rand() % 32000);
		}
	}

	return matriz; /*Retorna a matriz*/

}

int** desalocarMatriz()
{

	int i;

	if (matriz == NULL) return NULL; /*Caso a matriz for nula, retorna nulo*/

	for (i = 0; i < LINHA; i++) free(matriz[i]); /*Para cada elemento no vetor de linhas da matriz, de um free()*/

	free(matriz); /*De um free() no vetor de linhas*/

	return NULL; /*Retorne nulo*/

}

/*Funções de alocação e desalocação de valores de memória da matriz de regiões*/

int** alocarMatrizRegioes()
{

	int** matrizRegioes; /*Instancia um valor local para matriz de regiões*/
	int i, j;

	/*Faz a alocação dinâmica de um vetor do tamnho da linha, com
	ponteiro de inteiros*/

	matrizRegioes = malloc(QTD_MACR0BLOCO_LINHAS * sizeof(int*));

	/*Caso o mat for nulo, ouve erro na alocação*/

	if (matrizRegioes == NULL)
	{
		printf("** Erro: Memória Insuficiente **");
		return NULL;
	}

	/*Para cada valor desse vetor, se faz a alocação dinâmica do tamanho de
	colunas da matriz. Caso der nulo um dos ponteiros, ouve erro de alocação*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++)
	{

		matrizRegioes[i] = malloc(QTD_MACR0BLOCO_COLUNAS * sizeof(int));

		if (matrizRegioes[i] == NULL)
		{
			printf("** Erro: Memória Insuficiente **");
			return NULL;
		}

	}

	/*Para cada elemento na matriz de regiões, iniciar com o valor 0*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++)
	{
		for (j = 0; j < QTD_MACR0BLOCO_COLUNAS; j++)
		{
			matrizRegioes[i][j] = 0;
		}
	}

	return matrizRegioes; /*Retorna a matriz de regiões*/

}

int** desalocarMatrizRegioes()
{

	int i;

	if (matrizRegioes == NULL) return NULL; /*Caso a matriz de regiões for nula, retorna nulo*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++) free(matrizRegioes[i]); /*Para cada elemento no vetor de linhas da matriz, de um free()*/

	free(matrizRegioes); /*De um free() no vetor de linhas*/

	return NULL; /*Retorne nulo*/

}

/*Funções de contagem do modo Serial e Paralelo*/

void contagemSerial()
{

	int i, j;

	/*Para cada elemento presente na matriz, se o elemento
	for primo, soma 1 ao contador de números primos*/

	for (i = 0; i < LINHA; i++)
	{
		for (j = 0; j < COLUNA; j++)
		{
			if (ehPrimo(matriz[i][j])) contadorPrimos++;
		}
	}

}

void contagemParalela()
{

	int i;

	pthread_mutex_init(&mutex, NULL); /*Inicia o mutex*/

	pthread_t tid[NUM_THREADS]; /*Vetor de indentificadores da Thread*/

	for (i = 0; i < NUM_THREADS; i++)
	{

		if (pthread_create(&(tid[i]), NULL, contador, NULL) != 0)
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

	pthread_mutex_destroy(&mutex); /*Destroi o mutex*/

}

/*Função usada pelas Threads para a contagem de valores primos*/

void* contador(void* arg)
{

	int contadorLocal = 0; /*Inicia um contador de números primos local*/

	/*Se faz um laço for para a leitura compartilhada entre as threads da matriz de regiões de macroblocos*/

	for (int macroLinhas = 0; macroLinhas < QTD_MACR0BLOCO_LINHAS; macroLinhas++)
	{
		for (int macroColunas = 0; macroColunas < QTD_MACR0BLOCO_COLUNAS; macroColunas++)
		{

			pthread_mutex_lock(&mutex); /*Faz o lock (região crítica que faz modificação na matriz de regiões)*/

			if (matrizRegioes[macroLinhas][macroColunas]) /*Caso o valor da matriz for igual a 1, significa que esse macrobloco já foi lido*/
			{
				pthread_mutex_unlock(&mutex);

				continue; /*Pula para o próximo valor (j+1)*/
			}

			matrizRegioes[macroLinhas][macroColunas] = 1; /*Caso o valor for 0, adiciona o valor 1 na matriz representando que essa região foi lida*/

			pthread_mutex_unlock(&mutex); /*Destrava o mutex da região crítica */

			int linhaInicial = macroLinhas * LIHNA_MACROBLOCO; /*Linha inicial do macrobloco*/
			int linhaFinal = linhaInicial + LIHNA_MACROBLOCO; /*Linha final do macrobloco*/
			int colunaInicial = macroColunas * COLUNA_MACROBLOCO; /*Coluna inicial do macrobloco*/
			int colunaFinal = colunaInicial + COLUNA_MACROBLOCO; /*Coluna final do macrobloco*/

			/*Leitura do macrobloco, somando ao contador local quando o valor for primo*/

			for (int i = linhaInicial; i < linhaFinal; i++)
			{
				for (int j = colunaInicial; j < colunaFinal; j++)
				{
					if (ehPrimo(matriz[i][j]))
					{
						contadorLocal++;
					}
				}
			}
		}
	}

	pthread_mutex_lock(&mutex); /*Faz o lock (região crítica que acrescenta o valor local ao valor global)*/
	contadorPrimos += contadorLocal;
	pthread_mutex_unlock(&mutex); /*Destrava o mutex da região crítica */

	pthread_exit(0); /*Ternina a Thread, retornando 0*/

}
