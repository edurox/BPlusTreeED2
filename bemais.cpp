#ifndef _BEMAIS_
#define _BEMAIS_
#include "bemais.h"
using namespace std;

/*
keys minimas = (ordem-1)/2
filhos minimos = (ordem+1)/2
 */

Hash hashFunction(char *str) { //funcao hash djb2
  Hash hash = 5381;
  int c;
  while ((c = *(str++))) { hash = ((hash << 5) + hash) + c; }
  return hash;
}

void leituraArquivo(vind &indices, int nChar, int atributo, FILE *entrada){
  int tamanho, i, j, virgula;
  char linha[MAXLINHA], aux[nChar+1];
  Hash hash = 0;
  Offset offsetAux;

  while (offsetAux = ftell(entrada), fgets(linha, MAXLINHA, entrada)) {
    virgula = atributo - 1;
    linha[strlen(linha)-1] = '\0';
    //passa o atributo pro aux
    for (i = 0, tamanho = strlen(linha); i < tamanho && virgula; i++)
      if (linha[i] == ',') virgula--;
    for (j = 0; j < nChar && linha[i] != '\0' && linha[i] != '\n' && linha[i] != ','; i++)
      if (linha[i] != '"') aux[j++] = linha[i];
    aux[j] = '\0';

    //faz hash
    hash = hashFunction(aux);

    //empurra no vetor
    indices.push_back(index_t(hash, offsetAux));
  }

  //ordena de acordo com as hashs
  sort(indices.begin(), indices.end(), compareIndex);
}

FILE* abrirArquivo(char arquivoEntrada[]){
  FILE *entrada;
  entrada = fopen(arquivoEntrada, "a+"); //abre o arquivo de entrada que vai ser passado de parametro quando executar o programa
  if (!entrada) { printf("Nao abriu entrada\n"); return NULL; }
  fseek(entrada, 0, SEEK_SET); //retorna o buffer pro comeÃ§o (so pra garantir)
  return entrada;
}

bool compareIndex(const index_t &_a, const index_t &_b) {
  return _a.hash < _b.hash;
}

nodo_t* trataExcecoes(nodo_t* paiAtual, nodo_t *filhoAtual, int ordem) {
  //excecÃ£o: Ãºnico filho tem menos entrada que o mÃ­nimo da ordem
  if (!paiAtual->pai && paiAtual->quantidadeFilhos == 1 && filhoAtual) {
    filhoAtual->pai = NULL;
    paiAtual->quantidadeFilhos = 0;
    mataArvore(paiAtual);
    return filhoAtual;
  }
  //excecÃ£o: Ãºltimo filho tem menos que a ordem â??
  if (filhoAtual && filhoAtual->quantidadeKeys < (ordem-1)/2) {
    nodo_t *familiarBeneficiario = NULL;
    int i, j;

    //acha o parente que vai receber as keys e os offsets
    if (paiAtual->quantidadeFilhos > 1) familiarBeneficiario = paiAtual->filhos[paiAtual->quantidadeFilhos-2];
    else { //else: pega do primo
      nodo_t *voAtual = paiAtual->pai;
      nodo_t *tioAtual = voAtual->filhos[voAtual->quantidadeFilhos-2];
      familiarBeneficiario = tioAtual->filhos[tioAtual->quantidadeFilhos-1];
    }
    for (i = familiarBeneficiario->quantidadeKeys, j = 0; j < filhoAtual->quantidadeKeys; i++, j++) {
      familiarBeneficiario->keys[i] = filhoAtual->keys[j];
      familiarBeneficiario->offsets[i] = filhoAtual->offsets[j];
      familiarBeneficiario->quantidadeKeys++;
    }
    nodo_t *temp = filhoAtual;
    filhoAtual = familiarBeneficiario;
    removeUltimo(paiAtual, ordem); //remove um de todos os ancestrais do antigo filhoAtual
  }
  return filhoAtual;
}

int bulk_loading(nodo_t* &arvore, vind &indices, int ordem){
  nodo_t *filhoAtual = NULL, *ant, *paiAtual = ant = NULL;
  offsets_t *novo;
  int iteradorIndices = 0, first = 1, condicaoParaFor = (ordem-1)/2;

  //cria o primeiro pai
  paiAtual = criaNodo(ordem, false);
  if (!paiAtual) { printf("Erro criando o primeiro pai\n"); return 1; }

  while (iteradorIndices < (int)indices.size()) {
    filhoAtual = criaNodo(ordem, true);

    //preenche o nodo filhoAtual, que Ã© o filho do paiAtual
    for(int j = 0; j <= condicaoParaFor && iteradorIndices < (int)indices.size(); iteradorIndices++, j++){
      //se o hash a ser inserido for o mesmo que o anterior, ele coloca o offset no mesmo
      if (j && filhoAtual->keys[j-1] == indices[iteradorIndices].hash) { j--; }
      else if (j == condicaoParaFor && indices[iteradorIndices].hash != filhoAtual->keys[j-1]) { break; }
      else {
	filhoAtual->keys[j] = indices[iteradorIndices].hash;
	filhoAtual->quantidadeKeys++;
      }

      //cria novo offset, passando como parametro o offset da hash atual e ligando o novo offset no comeÃ§o da lista
      novo = NULL;
      novo = criaOffset(indices[iteradorIndices].offset, filhoAtual->offsets[j]);
      if (!novo) { printf("Erro ao criar offset %lld", indices[iteradorIndices].offset); return 1; }
      filhoAtual->offsets[j] = novo;
    }

    if (first) { //primeiro caso/folha
      first = 0;
      paiAtual->filhos[0] = filhoAtual;
      filhoAtual->pai = paiAtual;
      paiAtual->quantidadeFilhos = 1;
    }
    else if (checaPai(filhoAtual, &paiAtual, filhoAtual->keys[0], ordem)) { return 1; }

    if (ant) {
      ant->prox = filhoAtual;
    }
    ant = filhoAtual;
  }

  filhoAtual = trataExcecoes(paiAtual, filhoAtual, ordem);

  //atualiza a raiz
  while (filhoAtual->pai != NULL) filhoAtual = filhoAtual->pai;
  arvore = filhoAtual;
  return 0;
}

int checaPai(nodo_t *filhoAtual, nodo_t** pAtual, Hash hashQueVem, int ordem) { //checa se precisa trocar o pai atual
  nodo_t *tioAtual = NULL, *voAtual = (*pAtual)->pai, *paiAtual = *pAtual;
  Hash hashQueSobe;
  int i, j;

  if (paiAtual->quantidadeFilhos >= ordem) {
    tioAtual = criaNodo(ordem, false);
    if (!tioAtual) return 1; //retorna erro se nao criou

    //pega hash que sobe
    hashQueSobe = paiAtual->keys[(ordem-1)/2];

    //divide paiAtual com tioAtual
    for (i = (ordem + 1) / 2, j = 0; i < ordem - 1; i++, j++) {
      tioAtual->keys[j] = paiAtual->keys[i];
      tioAtual->filhos[j] = paiAtual->filhos[i];
      tioAtual->filhos[j]->pai = tioAtual;
    }
    tioAtual->filhos[j] = paiAtual->filhos[i];
    tioAtual->filhos[j]->pai = tioAtual;
    //coloca novos numeros
    tioAtual->quantidadeKeys = ordem / 2 - 1;
    tioAtual->quantidadeFilhos = tioAtual->quantidadeKeys + 1;
    paiAtual->quantidadeKeys = (ordem-1)/2;
    paiAtual->quantidadeFilhos = (ordem+1)/2;

    if (!paiAtual->pai) {
      voAtual = criaNodo(ordem, false);
      paiAtual->pai = voAtual;
      voAtual->filhos[0] = paiAtual;
      voAtual->quantidadeFilhos = 1;
    }
    checaPai(tioAtual, &voAtual, hashQueSobe, ordem);
    paiAtual = tioAtual;
  }

  //coloca hashQueVem no final
  paiAtual->keys[paiAtual->quantidadeKeys++] = hashQueVem;
  paiAtual->filhos[paiAtual->quantidadeFilhos++] = filhoAtual;
  filhoAtual->pai = paiAtual;
  *pAtual = paiAtual;
  return 0;
}

nodo_t* criaNodo(int ordem, bool folha){
  nodo_t *nodo = NULL;

  nodo = (nodo_t*)malloc(sizeof(nodo_t));
  if (!nodo) { printf("Erro na criaNodo\n"); return NULL; }

  if(!folha) {
    nodo->filhos = NULL;
    nodo->filhos = (nodo_t**)malloc( sizeof(nodo_t*) * ordem );
    if (!nodo->filhos) { printf("Erro inicializando vetor dos filhos\n"); return NULL; }
    nodo->offsets = NULL;
  }
  else {
    nodo->offsets = NULL;
    nodo->offsets = (offsets_t**)malloc(sizeof(offsets_t*)*(ordem-1));
    if (!nodo->offsets) { printf("Erro inicializando vetor de offsets\n"); return NULL; }
    nodo->filhos = NULL;
    memset(nodo->offsets, 0, sizeof(offsets_t*)*(ordem-1));
  }
  nodo->keys = NULL;
  nodo->keys = (Hash*)malloc(sizeof(Hash)*(ordem-1));
  if (!nodo->keys) { printf("Erro inicializando vetor das chaves\n"); return NULL; }
  nodo->quantidadeKeys = nodo->quantidadeFilhos = 0;
  nodo->prox = nodo->pai = NULL;
  nodo->folha = folha;
  return nodo;
}

offsets_t* criaOffset(Offset o, offsets_t *p) {
  offsets_t *r = NULL;
  r = (offsets_t*)malloc(sizeof(offsets_t));
  r->offset = o;
  r->prox = p;
  return r;
}

void mataArvore(nodo_t *n) {
  if (!n) return;
  for (int i = 0; n->folha && i < n->quantidadeKeys; i++) {
    if (n->offsets[i]) mataOffsets(n->offsets[i]->prox);
  }

  for (int i = 0; !n->folha && i < n->quantidadeFilhos; i++) {
    mataArvore(n->filhos[i]);
  }

  if (n->filhos) free(n->filhos);
  if (n->offsets) free(n->offsets);
  free(n->keys);
}

void mataOffsets(offsets_t *o) {
  if (!o) return;
  mataOffsets(o->prox);
  free(o);
}

int imprimeArvore(nodo_t *arvore) {
  FILE *dotFile = NULL;
  char nomeArquivo[] = "saida.dot", comando[400];
  int numeroNodo = 0;

  //cria o comando
  sprintf(comando, "dot %s -Tpng -o saida.png && kde-open saida.png\n", nomeArquivo);

  //abre dotFile
  dotFile = fopen(nomeArquivo, "w");
  if (!dotFile) { printf("Erro abrindo %s\n", nomeArquivo); return 1; }
  fprintf(dotFile, "graph {\n");
  imprimeNodos(dotFile, arvore, &numeroNodo, 0);
  fprintf(dotFile, "}\n");
  fclose(dotFile);
  system(comando);
  return 0;
}

void imprimeNodos(FILE *dotFile, nodo_t *n, int *numeroNodo, int liga) {
  int esseNumero = *numeroNodo;
  if (!n) return;
  fprintf(dotFile, "%d [label=%d];\n", (*numeroNodo)++, liga);
  for (int i = 0; i < n->quantidadeFilhos; i++) {
    fprintf(dotFile, "%d -- ", esseNumero);
    (*numeroNodo)++;
    imprimeNodos(dotFile, n->filhos[i], numeroNodo, i);
  }

  //imprime esse nodo
  fprintf(dotFile, "%d [label=\"", esseNumero);
  for (int i = 0; i < n->quantidadeKeys; i++)
    fprintf(dotFile, "%s %lld", i? " Â»":"", n->keys[i]);
  fprintf(dotFile, "\"];\n");
  fprintf(dotFile, "%d [shape=box]\n", esseNumero);
}

void removeUltimo(nodo_t *paiAtual, int ordem) {
  if (paiAtual->quantidadeKeys > (ordem-1)/2 || paiAtual->pai == NULL) { //se ele tem suficiente ou Ã© raiz
    paiAtual->quantidadeKeys--;
    paiAtual->quantidadeFilhos--;
    if (paiAtual->quantidadeKeys <= 0) {
      paiAtual->filhos[0]->pai = NULL;
      mataArvore(paiAtual);
    }
    return;
  }
  nodo_t *tioAtual, *voAtual;
  int i, j;
  voAtual = paiAtual->pai;
  tioAtual = voAtual->filhos[--voAtual->quantidadeFilhos - 1];

  //abaixa uma hash pro tio
  tioAtual->keys[tioAtual->quantidadeKeys++] = voAtual->keys[--voAtual->quantidadeKeys];
  printf("%d]", tioAtual->quantidadeKeys);

  for (i = tioAtual->quantidadeKeys, j = 0; j < paiAtual->quantidadeKeys; j++, i++) {
    paiAtual->filhos[j]->pai = tioAtual;
    tioAtual->keys[i] = paiAtual->keys[j];
    tioAtual->filhos[i] = paiAtual->filhos[j];
    tioAtual->quantidadeKeys++;
    tioAtual->quantidadeFilhos++;
  }
  tioAtual->filhos[i] = paiAtual->filhos[j];
  paiAtual->filhos[j]->pai = tioAtual;
  tioAtual->quantidadeFilhos++;

  paiAtual->quantidadeFilhos = paiAtual->quantidadeKeys = 0;
  mataArvore(paiAtual);
  removeUltimo(voAtual, ordem);
}

int bbin(nodo_t *nodoAtual, Hash numero) {
  int li = 0, ls = nodoAtual->quantidadeKeys;
  while (li < ls) {
    int meio = (li + ls) / 2;
    if (nodoAtual->keys[meio] < numero) li = meio + 1;
    else ls = meio;
  }
  return ls + (!nodoAtual->folha && nodoAtual->keys[ls] == numero);
}

nodo_t *achaElemento(nodo_t* noAtual, int &indice, Hash procurando) {
  int ind = bbin(noAtual, procurando);
  if (noAtual->keys[ind] == procurando && noAtual->folha) { indice = ind; return noAtual; }
  if (!noAtual->folha) return achaElemento(noAtual->filhos[ind], indice, procurando);
  return NULL;
}

void imprimeTupla(nodo_t *nodoAtual, int indiceElemento, FILE *arquivo) {
  offsets_t *o;
  char tupla[MAXLINHA];
  if (!nodoAtual || !arquivo || indiceElemento < 0) return;
  for (o = nodoAtual->offsets[indiceElemento]; o; o = o->prox) {
    fseek(arquivo, o->offset, SEEK_SET);
    fgets(tupla, MAXLINHA, arquivo);
    if (tupla[strlen(tupla)-1] == '\n') tupla[strlen(tupla)-1] = '\0';
    printf("%s\n", tupla);
  }
}

void imprimeMenu() {
  printf("Digite a opÃ§Ã£o desejada\n");
  printf("0 Sair\n");
  printf("1 Imprimir a Ã¡rvore\n");
  printf("2 Buscar\n");
  printf("3 Apagar\n");
}



void emprestadoDireitaFolha(nodo_t** pai, nodo_t** atual, nodo_t** irmao, int indicePai){
	int i;
	(*atual)->keys[(*atual)->quantidadeKeys] = (*irmao)->keys[0];
	(*atual)->offsets[(*atual)->quantidadeKeys] = (*irmao)->offsets[0];
	(*atual)->quantidadeKeys++;
	//AGRUPO OS ELEMENTOS DO IRMAO NO INICIO NOVAMENTE
	for(i = 1; i < (*irmao)->quantidadeKeys; i++){
		(*irmao)->keys[i-1] = (*irmao)->keys[i];
		(*irmao)->offsets[i-1] = (*irmao)->offsets[i];
	}
	(*irmao)->keys[i] = NULL;
	mataOffsets((*irmao)->offsets[i]);
	(*irmao)->quantidadeKeys--;
	(*pai)->keys[indicePai-1] = (*irmao)->keys[0];
}

void emprestadoEsquerdaFolha(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai){
	int i;
	for (i = (*atual)->quantidadeKeys; i > 0; i--) {
		(*atual)->keys[i] = (*atual)->keys[i-1];
		(*atual)->offsets[i] = (*atual)->offsets[i-1];
	}
	(*irmao)->quantidadeKeys--;
	(*atual)->quantidadeKeys++;
	(*atual)->keys[0] = (*irmao)->keys[(*irmao)->quantidadeKeys];
	(*atual)->offsets[0] = (*irmao)->offsets[(*irmao)->quantidadeKeys];
	(*irmao)->keys[(*irmao)->quantidadeKeys] = NULL;
	mataOffsets((*irmao)->offsets[(*irmao)->quantidadeKeys]);
	(*pai)->keys[indicePai-2] = (*atual)->keys[0];
}

void verificaFolha(nodo_t *atual, int indicePai, int ordem){
	nodo_t *pai = atual->pai,*irmao;
	int i, qtdMinima = (ordem-1)/2;

	printf("keys = %i\n\n",pai->quantidadeKeys);
	printf("filhos = %i\n\n",pai->quantidadeFilhos);

	if(pai->quantidadeFilhos > indicePai){//EXISTE UM IRMAO A DIREITA? POSSO PEGAR EMPRESTA DESSE IRMAO?
	  irmao = pai->filhos[indicePai];
	  if(irmao->quantidadeKeys > qtdMinima){
		return emprestadoDireitaFolha(&pai,&atual,&irmao,indicePai);
	  }
	}
  if(indicePai-1){//EXISTE IRMAO A ESQUERDA? POSSO PEGAR EMPRESTA DESSE IRMAO?
    irmao = pai->filhos[indicePai-2];
    if(irmao->quantidadeKeys > qtdMinima){
      return emprestadoEsquerdaFolha(&pai,&atual,&irmao,indicePai);
    }
  }
  
	if(pai->quantidadeFilhos > indicePai){//EXISTE UM IRMAO A DIREITA? MERGE COM O IRMAO A DIREITA
		irmao = pai->filhos[indicePai];
		for(i = 0; i < irmao->quantidadeKeys; i++){
			atual->keys[atual->quantidadeKeys] = irmao->keys[i];
			atual->offsets[atual->quantidadeKeys] = irmao->offsets[i];
			atual->quantidadeKeys++;
		}
		atual->prox = irmao->prox;
		pai->filhos[indicePai] = atual;
		pai->keys[indicePai-1] = pai->keys[indicePai];
		free(irmao);
		return removeElemento(pai, indicePai, ordem);
	}else{//MERGE COM O IRMAO A ESQUERDA
		irmao = pai->filhos[indicePai-2];
		for(i = 0; i < atual->quantidadeKeys; i++){
			irmao->keys[irmao->quantidadeKeys] = atual->keys[i];
			irmao->offsets[irmao->quantidadeKeys] = atual->offsets[i];
			irmao->quantidadeKeys++;
		}
		pai->keys[indicePai-2] = pai->keys[indicePai-1];
		pai->filhos[indicePai-1] = irmao;
		irmao->prox = atual->prox;
		free(atual);
		return removeElemento(pai, indicePai-1, ordem);
	}
}

void emprestadoDireitaInterna(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai){
	int i;
	(*atual)->keys[(*atual)->quantidadeKeys] = (*pai)->keys[indicePai-1];
	(*atual)->filhos[(*atual)->quantidadeFilhos] = (*irmao)->filhos[0];
	(*atual)->quantidadeKeys++;
	(*atual)->quantidadeFilhos++;
	(*pai)->keys[indicePai-1] = (*irmao)->keys[0];
	for(i = 0; i < (*irmao)->quantidadeKeys; i++){
		(*irmao)->keys[i] = (*irmao)->keys[i+1];
		(*irmao)->filhos[i] = (*irmao)->filhos[i+1];
	}
	(*irmao)->keys[i] = NULL;
	(*irmao)->filhos[i] = NULL;
	(*irmao)->quantidadeKeys--;
	(*irmao)->quantidadeFilhos--;
}

void emprestadoEsquerdaInterna(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai){
	int i;
	for (i = (*atual)->quantidadeFilhos; i > 0; i--) {
		(*atual)->keys[i] = (*atual)->keys[i-1];
		(*atual)->filhos[i] = (*atual)->filhos[i-1];
	}
	(*atual)->keys[0] = (*pai)->keys[indicePai-2];
	(*atual)->filhos[0] = (*irmao)->filhos[(*irmao)->quantidadeFilhos-1];
	(*atual)->quantidadeKeys++;
	(*atual)->quantidadeFilhos++;
	(*irmao)->quantidadeKeys--;
	(*irmao)->quantidadeFilhos--;
	(*pai)->keys[indicePai-2] = (*irmao)->keys[(*irmao)->quantidadeKeys];
	(*irmao)->keys[(*irmao)->quantidadeKeys] = NULL;
	(*irmao)->filhos[(*irmao)->quantidadeFilhos] = NULL;
}

void verificaInterno(nodo_t *atual, int indicePai, int ordem){
	nodo_t *pai = atual->pai,*irmao;
	int i, qtdMinima = (ordem-1)/2;
	if(pai->quantidadeFilhos > indicePai){//EXISTE UM IRMAO A DIREITA? POSSO PEGAR EMPRESTA DESSE IRMAO?
	  irmao = pai->filhos[indicePai];
	  if(irmao->quantidadeKeys > qtdMinima){
		return emprestadoDireitaInterna(&pai,&atual,&irmao,indicePai);
	  }
	}
	if(indicePai-1){//EXISTE IRMAO A ESQUERDA? POSSO PEGAR EMPRESTA DESSE IRMAO?
	  irmao = pai->filhos[indicePai-2];
	  if(irmao->quantidadeKeys > qtdMinima){
	    return emprestadoEsquerdaInterna(&pai,&atual,&irmao,indicePai);
	  }
	}

	if(pai->quantidadeFilhos > indicePai){//EXISTE UM IRMAO A DIREITA? MERGE COM O IRMAO A DIREITA
	  irmao = pai->filhos[indicePai];
	  atual->keys[atual->quantidadeKeys] = pai->keys[indicePai-1];
	  atual->quantidadeKeys++;
	  for(i = 0; i < irmao->quantidadeKeys; i++){
	    atual->keys[atual->quantidadeKeys] = irmao->keys[i];
	    irmao->filhos[i]->pai = atual;
	    atual->filhos[atual->quantidadeFilhos] = irmao->filhos[i];
	    atual->quantidadeKeys++;
	    atual->quantidadeFilhos++;
	  }
	  irmao->filhos[i]->pai = atual;
	  atual->filhos[atual->quantidadeFilhos] = irmao->filhos[i];
	  atual->quantidadeFilhos++;
	  free(irmao);
	  pai->filhos[indicePai] = atual;
	  return removeElemento(pai, indicePai-1, ordem);
	}else{ //MERGE COM O IRMAO A ESQUERDA
	  irmao = pai->filhos[indicePai-2];
	  if(indicePai > 2) irmao->keys[irmao->quantidadeKeys] = pai->keys[indicePai-3];
	  else irmao->keys[irmao->quantidadeKeys] = pai->keys[indicePai-2];
	  irmao->quantidadeKeys++;
	  for(i = 0; i < atual->quantidadeKeys; i++){
	    irmao->keys[irmao->quantidadeKeys] = atual->keys[i];
	    atual->filhos[i]->pai = irmao;
	    irmao->filhos[irmao->quantidadeFilhos] = atual->filhos[i];
	    irmao->quantidadeKeys++;
	    irmao->quantidadeFilhos++;
	  }
	  atual->filhos[i]->pai = irmao;
	  irmao->filhos[irmao->quantidadeFilhos] = atual->filhos[i];
	  irmao->quantidadeFilhos++;
	  free(atual);
	  return removeElemento(pai, indicePai-1, ordem);
	}
}

void removeElemento(nodo_t* atual, int indice, int ordem){
	int i;
	if (!atual)//CASO O NODO COM O ITEM ATUAL SEJA NULO RETORNA
		return;

	if(atual->folha){
		indice++;
		while(indice < atual->quantidadeKeys){
			atual->keys[indice-1] = atual->keys[indice];
			atual->offsets[indice-1] = atual->offsets[indice];
			indice++;
		}
		atual->keys[indice] = NULL;
		atual->offsets[indice] = NULL;
		atual->quantidadeKeys--;

		if(atual->quantidadeKeys >= (ordem-1)/2)//AINDA RESTAM O MINIMO DE ELEMENTOS
			return;

		if(!atual->pai)//A REMOCAO FOI NA RAIZ
			return;

		//NAO TEM A QUANTIDADE DE ELEMTNTOS MINIMO
		//ENCONTRA O INDICE NO PAI PARA CONFERIR SE É POSSIVEL PEGAR EMPRESTADO
		for(i = 0; i <= atual->pai->quantidadeFilhos; i++){
			if(atual->pai->filhos[i] == atual){
				break;
			}
		}
		return verificaFolha(atual, i+1, ordem);

	}else{//NAO FOLHA
		indice++;
		while(indice < atual->quantidadeKeys){
			atual->keys[indice-1] = atual->keys[indice];
			atual->filhos[indice-1] = atual->filhos[indice];
			indice++;
		}
		atual->filhos[indice-1] = atual->filhos[indice];
		atual->keys[indice-1] = NULL;
		atual->filhos[indice] = NULL;
		atual->quantidadeKeys--;
		atual->quantidadeFilhos--;

		if(atual->quantidadeKeys >= (ordem-1)/2)//AINDA RESTAM O MINIMO DE ELEMENTOS
			return;

		if(!atual->pai)//A REMOCAO FOI NA RAIZ
			return;

		//NAO TEM A QUANTIDADE DE ELEMTNTOS MINIMO
		//ENCONTRA O INDICE NO PAI PARA CONFERIR SE É POSSIVEL PEGAR EMPRESTADO
		for(i = 0; i <= atual->pai->quantidadeFilhos; i++){
			if(atual->pai->filhos[i] == atual){
				break;
			}
		}

		return verificaInterno(atual, i+1, ordem);
	}
}

#endif
