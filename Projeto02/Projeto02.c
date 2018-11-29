#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_QUADROS 256
#define TOTAL_QUADROS 256
#define TAMANHO_TLB 16
#define TABELA_DE_PAGINAS 256
#define numBytes 256

int numTabPaginas[TABELA_DE_PAGINAS];
int quadrosTabPaginas[TABELA_DE_PAGINAS];
int TLBp[TAMANHO_TLB];
int TLBq[TAMANHO_TLB];
int memoriaFisica[TOTAL_QUADROS][TAMANHO_QUADROS];
int pageFaults = 0;
int TLBHits    = 0;
int primQuadroDisp = 0;
int primNumTabPagDisp = 0;
int numTentativasTLB  = 0;
int enderecoLogico;
signed char lidosBackingStore[numBytes];
signed char valorByte;

FILE *arq_address;
FILE *arq_BackingStore;

void lerPag(int address);
void lerBackingStore(int numPagina);
void inserirTLB(int numPagina, int numQuadro);

int main(int argc, char *argv[])
{
	char address[10];
  double taxErrosPag;
  double taxTLB;
  int numEnderecos = 0;

  arq_BackingStore = fopen("BACKING_STORE.bin", "rb");
  arq_address = fopen(argv[1], "r");

  while (fgets(address, 10, arq_address) != NULL) 
  {
  	enderecoLogico = atoi(address);
    lerPag(enderecoLogico);
    numEnderecos++;
  }

  taxErrosPag = pageFaults / (double)numEnderecos;
  taxTLB = TLBHits / (double)numEnderecos;

  printf("Numero de endereços........ %d\n", numEnderecos);
  printf("Page Faults................ %d\n", pageFaults);
  printf("Taxa de erros de página.... %.3f\n",taxErrosPag);
  printf("TLB Hits................... %d\n", TLBHits);
  printf("Taxa de sucesso do TLB..... %.3f\n", taxTLB);

  fclose(arq_address);
  fclose(arq_BackingStore);

  return 0;
}

void lerPag(int enderecoLogico)
{
	int numPagina = ((enderecoLogico & 0xFFFF)>>8);
  int offset = (enderecoLogico & 0xFF);
  int numQuadro = -1;

  for(int i = 0; i < TAMANHO_TLB; i++)
  {
  	if(TLBp[i] == numPagina)
  	{
    	numQuadro = TLBq[i];
      TLBHits++;
    }
  }

  if(numQuadro == -1)
  {
    for(int i = 0; i < primNumTabPagDisp; i++)
    {
      if(numTabPaginas[i] == numPagina)
      	numQuadro = quadrosTabPaginas[i];
    }

    if(numQuadro == -1)
    {
    	lerBackingStore(numPagina);
      pageFaults++;
      numQuadro = primQuadroDisp - 1;
    }
  }

  inserirTLB(numPagina, numQuadro);
  valorByte = memoriaFisica[numQuadro][offset];
  printf("Endereço Logico... %d \n", enderecoLogico);
  printf("Valor do Quadro... %d \n", numQuadro);
  printf("Valor Offset...... %d \n", offset);
  printf("Endereço Fisico... %d \n", (numQuadro << 8) | offset);
  printf("Valor do byte..... %d \n\n", valorByte);
}

void inserirTLB(int numPagina, int numQuadro)
{
	int i;

  for(int i = 0; i < numTentativasTLB; i++)
  {
  	if(TLBp[i] == numPagina)
    	break;
  }

	if(i == numTentativasTLB)
	{
  	if(numTentativasTLB < TAMANHO_TLB)
  	{
    	TLBp[numTentativasTLB] = numPagina;
      TLBq[numTentativasTLB] = numQuadro;
    }
    else
    {
    	for(i = 0; i < TAMANHO_TLB - 1; i++)
    	{
      	TLBp[i] = TLBp[i + 1];
        TLBq[i] = TLBq[i + 1];
      }
      TLBp[numTentativasTLB - 1] = numPagina;
      TLBq[numTentativasTLB - 1] = numQuadro;
    }
  }
	else
	{
  	for(i = i; i < numTentativasTLB - 1; i++)
  	{
    	TLBp[i] = TLBp[i + 1];
      TLBq[i] = TLBq[i + 1];
    }
		if(numTentativasTLB < TAMANHO_TLB)
		{
    	TLBp[numTentativasTLB] = numPagina;
      TLBq[numTentativasTLB] = numQuadro;
    }
    else
    {
    	TLBp[numTentativasTLB - 1] = numPagina;
      TLBq[numTentativasTLB - 1] = numQuadro;
    }
  }

  if(numTentativasTLB < TAMANHO_TLB)
  	numTentativasTLB++;
}

void lerBackingStore(int numPagina)
{
	fseek(arq_BackingStore, numPagina * numBytes, SEEK_SET);
  fread(lidosBackingStore, sizeof(signed char), numBytes, arq_BackingStore);
	
	for(int i = 0; i < numBytes; i++)
		memoriaFisica[primQuadroDisp][i] = lidosBackingStore[i];

  numTabPaginas[primNumTabPagDisp] = numPagina;
  quadrosTabPaginas[primNumTabPagDisp] = primQuadroDisp;
  primQuadroDisp++;
  primNumTabPagDisp++;
}
