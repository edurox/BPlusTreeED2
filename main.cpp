#include <cstdio>
#include <cstdlib>
#include "bemais.h"

int main(int argc, char *argv[]) {
  int nChar, atributo, ordem, opMenu = 1, indiceBusca;
  char nomeArquivo[200], busca[MAXLINHA];
  FILE *entrada = NULL;
  vind indices;
  nodo_t *arvore = NULL, *nodoDeBusca;
  
  //pega a quantidade de caracteres que farao parte do indice, a ordem da arvore, o atributo que sera levado em conta e o nome do arquivo
  atributo = atoi(argv[1]);
  nChar = atoi(argv[2]); 
  ordem = atoi(argv[3]);
  strcpy(nomeArquivo,argv[4]);
  
  //abre o arquivo de entrada
  entrada = abrirArquivo(nomeArquivo); //passa o nome do arquivo
  if (!entrada) return 0;
  
  //le as entrada o coloca os offsets das tuplas no vetor de indices
  leituraArquivo(indices, nChar, atributo, entrada);
    
#include <cstdio>
#include <cstdlib>
#include "bemais.h"

int main(int argc, char *argv[]) {
  int nChar, atributo, ordem, opMenu = 1, indiceBusca;
  char nomeArquivo[200], busca[MAXLINHA];
  FILE *entrada = NULL;
  vind indices;
  nodo_t *arvore = NULL, *nodoDeBusca;
  
  //pega a quantidade de caracteres que farao parte do indice, a ordem da arvore, o atributo que sera levado em conta e o nome do arquivo
  atributo = atoi(argv[1]);
  nChar = atoi(argv[2]); 
  ordem = atoi(argv[3]);
  strcpy(nomeArquivo,argv[4]);
  
  //abre o arquivo de entrada
  entrada = abrirArquivo(nomeArquivo); //passa o nome do arquivo
  if (!entrada) return 0;
  
  //le as entrada o coloca os offsets das tuplas no vetor de indices
  leituraArquivo(indices, nChar, atributo, entrada);
    
  //realiza o bulkload que retornara 0 no sucesso
  if (bulk_loading(arvore, indices, ordem)) return 0;
    
  for (int i = 0; i < (int)indices.size(); i++)
    printf("%d: %llu\n", i, indices[i].hash);

  while (opMenu) {
    imprimeMenu();
    scanf("%d", &opMenu);
    getchar();
    //    system("clear\n");
    switch (opMenu) {
    case 0: break;
    case 1:
      imprimeArvore(arvore);
      break;
    case 2:
      printf("Digite o texto a ser buscado\n");
      fgets(busca, MAXLINHA, stdin);
      busca[strlen(busca)-1] = '\0';
      busca[nChar] = '\0';
      //      system("clear\n");
      printf("Você buscou: %s\n", busca);
      nodoDeBusca = achaElemento(arvore, indiceBusca, hashFunction(busca));
      if (!nodoDeBusca) printf("Linha não encontrada\n");
      else {
	printf("\nResultados encontrados:\n");
	imprimeTupla(nodoDeBusca, indiceBusca, entrada);
	putchar('\n');
      }
      break;
    case 3:
    	if(arvore){
			printf("Digite a informacao a ser apagada\n");
			fgets(busca, MAXLINHA, stdin);
			busca[strlen(busca)-1] = '\0';
			busca[nChar] = '\0';
			nodoDeBusca = achaElemento(arvore, indiceBusca, hashFunction(busca));
			if (!nodoDeBusca) printf("Linha não encontrada\n");
			else {
				printf("\nResultados Encontrados\n");
				imprimeTupla(nodoDeBusca, indiceBusca, entrada);
				putchar('\n');
			}
				
			removeElemento(nodoDeBusca, indiceBusca, ordem);
			
			if(!arvore->folha && !arvore->quantidadeKeys){
				if(arvore->filhos[0]) arvore = arvore->filhos[0];
				else  arvore = arvore->filhos[1];
			  	arvore->pai = NULL;
			}
			if(arvore->folha  && arvore->quantidadeKeys == 0){
		    		free(arvore);
		    		arvore = NULL;
			}
			printf("Remocao Concluida\n\n");
		}else{
			printf("Arvore Vazia\n\n");
		}
    	break;
    default:
      printf("Opção inválida\n");
      break;
    } 
  }  
  mataArvore(arvore);
  fclose(entrada);
  return 0;
}
