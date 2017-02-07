#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>

#define MAXLINHA 1123

using namespace std;

typedef long long int Offset;
typedef unsigned long long int Hash;

//indices para o vetor de indices (serao inseridos na arvore)
typedef struct index_t{
    Hash hash;
    long long int offset;

    index_t(long long int _h, unsigned long int _o) : hash(_h), offset(_o) {}
} index_t;

//offsets. O prox e pra quando se tem mais de um hash igual
typedef struct offsets_t{    //lista encadeada dos offsets
    Offset offset;
    struct offsets_t *prox;
} offsets_t;

typedef struct nodo_t{
    nodo_t **filhos, *prox, *pai;
    offsets_t **offsets;
    Hash *keys;
    int quantidadeKeys, quantidadeFilhos;
    bool folha;

    /*~nodo_t() { //deconstrutor
        free(keys);
        free(filhos);
    }*/
}nodo_t;

typedef vector<index_t> vind;

//Inicializa��o e fun��e auxiliares
//Cria a hash a partir de uma string. Fun��o que foi pega na internet.
Hash hashFunction(char *str);
//Fun��o que recebe o nome de arquivo a ser aberto e retorna o ponteiro para o arquivo. A fun��o j� trata dos possiveis erros que podem acontecer na abertura de um arquivo.
FILE* abrirArquivo(char arquivoEntrada[]);
//Fun��o para comparar duas vari�ves de tipo index_t. Foi criada pra dar suporte a fun��o de sort da biblioteca algorithm.
bool compareIndex(const index_t &_a, const index_t &_b);
//Fun��o que le o arquivo .csv e guarda as hashs e os offsets no vector de index_t
void leituraArquivo(vind &indices, int nChar, int atributo, FILE *entrada);

//BulkLoading
//Fun��o principal do Bulk Loading. Retorna se nenhum erro aconteceu.
int bulk_loading(nodo_t* &arvore, vind &indices, int ordem);
//Fun��o que liga o filho rec�m criado de Bulk Loading com o pai. Se n�o tem espa�o no pai, a fun��o tamb�m lida com isso criando novos n�s.
int checaPai(nodo_t *filhoAtual, nodo_t** pAtual, Hash hashQueVem, int ordem);
//Fun��o que cria e inicaliza nodo e retorna o ponteiro.
nodo_t* criaNodo(int ordem, bool folha);
//Fun��o que cria e inicializa offset e retorna o ponteiro pro offset. Ela recebe o offset a ser inserido e o antigo offset de um hash, para poder inserir o novo offset no come�o da lista, e n�o no final.
offsets_t* criaOffset(Offset o, offsets_t *p);

//Destruir a �rvore
//Fun��o para destruir a arvore (desalocar da mem�ria)
void mataArvore(nodo_t *n);
//Fun��o para destruir os offsets (desaloc�-los da mem�ria)
void mataOffsets(offsets_t *o);

//Imprimir a �rvore
//Fun��o que cria o arquivo .dot, transforma-o em .png e abre a imagem da arvore.
int imprimeArvore(nodo_t *arvore);
//Fun��o auxiliar da imprimeArvore que imprime os nodos.
void imprimeNodos(FILE *dotFile, nodo_t *n, int *numeroNodo, int liga);
//Fun��o que remove o Ultimo elemento de um n� e ajeita o n� de acordo com o resultado.
void removeUltimo(nodo_t *paiAtual, int ordem);
//Fun��o que imprime a tupla, dado um no e uma posi��o
void imprimeTupla(nodo_t *nodoAtual, int indiceElemento, FILE *arquivo);

//Buscar Elemento
//Fun��o que retorna o endere�o do n� em que se encontra o elemento "procurando" e coloca a posi��o do elemento na vari�vel indice. A fun��o retorna NULL caso o elemento n�o seja encontrado
nodo_t *achaElemento(nodo_t* noAtual, int &indice, Hash procurando);
//Busca binaria
int bbin(nodo_t *nodoAtual, Hash numero);

//Com o usu�rio
void imprimeMenu();

nodo_t* trataExcecoes(nodo_t* paiAtual, nodo_t *filhoAtual, int ordem);


void removeElemento(nodo_t* atual, int indice, int ordem);
void verificaFolha(nodo_t *atual, int indicePai, int ordem);
void verificaInterno(nodo_t *atual, int indicePai, int ordem);
void emprestadoDireitaFolha(nodo_t** pai, nodo_t** atual, nodo_t** irmao, int indicePai);
void emprestadoEsquerdaFolha(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai);
void mergeFolha(nodo_t **nodoQueObedece, nodo_t **nodoQueManda);
void emprestadoDireitaInterna(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai);
void emprestadoEsquerdaInterna(nodo_t** pai, nodo_t** atual, nodo_t** irmao,int indicePai);
void mergeInterno(nodo_t **nodoQueObedece, nodo_t **nodoQueManda);
