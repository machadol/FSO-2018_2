#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM_SUDOKU 9
#define QTD_THREADS 27
#define QTD_ARQ 10

void *verificaLinha (void *);
void *verificaColuna(void *);
void *verificaMatriz(void *);

int sudoku[TAM_SUDOKU][TAM_SUDOKU];
int validado[QTD_THREADS];

int main(int argc, char const *argv[])
{
	for (int i = 0; i < QTD_ARQ; i++)
	{
		FILE *arq;
		pthread_t thread[QTD_THREADS];
		int aux = 0, contaValido = 0;
		char arqSudoku[15];
  	sprintf(arqSudoku, "%d.txt", i + 1);

  	arq = fopen(arqSudoku, "r");

  	for (int j = 0; j < TAM_SUDOKU; j++)
  	{
  		for (int k = 0; k < TAM_SUDOKU; k++)
  		{
  			fscanf(arq, "%d", &sudoku[j][k]);
  		}
  	}

  	for (int j = 0; j < TAM_SUDOKU; j++)
  	{
  		int linha[2] = {j, aux};
  		pthread_create(&thread[linha[0]], NULL, verificaLinha, (void *) linha);

  		int coluna[2] = {(j + TAM_SUDOKU), aux};
  		pthread_create(&thread[coluna[0]], NULL, verificaColuna, (void *) coluna);

  		for (int k = 0; k < 7; k = k + 3)
  		{
  			for (int w = 0; w < 7; w = w + 3)
  			{
  				int matriz[3] = {(j + (TAM_SUDOKU * 2)), k, w};
  				pthread_create(&thread[matriz[0]], NULL, verificaMatriz, (void *) matriz);
  			}
  		}

  		aux++;
  	}


    for(int j = 0; j < QTD_THREADS; j++)
    {
      pthread_join(thread[j], NULL);
    }

  	for(int j = 0; j < QTD_THREADS; j++)
  	{
    	if(validado[j] == 0)
    	{
    		printf("%.2d.txt - Sudoku inválido!\n",i+1);
    		break;
     	}
     	else
    		contaValido++;
    	
  	}

    if (contaValido == QTD_THREADS)
    {
    	printf("%.2d.txt - Sudoku válido!\n",i+1);
    }

  	fclose(arq);
	} 
	
	return 0;
}

void *verificaLinha(void *param)
{
  int *linha = (int*) param;
  int valida[TAM_SUDOKU] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  for(int i = 0; i < TAM_SUDOKU; i++)
    valida[sudoku[linha[1]][i] - 1] = 1;

  for(int i = 0; i < TAM_SUDOKU; i++)
  {
    if(valida[i] == 0)
    {
      validado[linha[0]] = 0;
      pthread_exit(0);
    }
  }

  validado[linha[0]] = 1;
  pthread_exit(0);
}

void *verificaColuna(void *param)
{
  int *coluna = (int*) param;
  int valida[TAM_SUDOKU] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  for(int i = 0; i < TAM_SUDOKU; i++)
    valida[sudoku[i][coluna[1]] - 1] = 1;

  for(int i = 0; i < TAM_SUDOKU; i++)
  {
    if(valida[i] == 0)
    {
      validado[coluna[0]] = 0;
      pthread_exit(0);
    }
  }

  validado[coluna[0]] = 1;
  pthread_exit(0);
}

void *verificaMatriz(void *param)
{
  int *matriz = (int*) param;
  int valida[TAM_SUDOKU] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  for(int i = matriz[1]; i < matriz[1] + 3; i++)
  {
    for(int j = matriz[2]; j < matriz[2] + 3; j++)
    {
      valida[sudoku[i][j] - 1] = 1;
    }
  }

  for(int i = 0; i < TAM_SUDOKU; i++)
  {
    if(valida[i] == 0)
    {
      validado[matriz[0]] = 0;
      pthread_exit(0);
    }
  }
  validado[matriz[0]] = 1;
  pthread_exit(0);
}
