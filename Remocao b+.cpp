nodo_t* emprestadoDireita(nodo_t** pai, nodo_t** remover, nodo_t** irmao, int indicePai){
	int i;
	(*remover)->keys[(*remover)->quantidadeKeys] = (*irmao)->keys[0];
	(*remover)->offsets[(*remover)->quantidadeKeys] = (*irmao)->offsets[0];
	(*remover)->quantidadeKeys++;
	//AGRUPO OS ELEMENTOS DO IRMAO NO INICIO NOVAMENTE
	for(i = 1; i <= (*irmao)->quantidadeKeys; i++){
		(*irmao)->keys[i-1] = (*irmao)->keys[i];
		(*irmao)->offsets[i-1] = (*irmao)->offsets[i];
	}
	(*irmao)->keys[(*irmao)->quantidadeKeys] = NULL;
	mataOffsets((*irmao)->offsets[(*irmao)->quantidadeKeys]);
	(*irmao)->quantidadeKeys--;
	(*pai)->keys[indicePai] = (*irmao)->keys[0];
	return *pai;
}

nodo_t* emprestadoEsquerda(nodo_t** pai, nodo_t** remover, nodo_t** irmao,int indicePai){
	(*remover)->keys[(*remover)->quantidadeKeys] = (*irmao)->keys[(*irmao)->quantidadeKeys];
	(*remover)->offsets[(*remover)->quantidadeKeys] = (*irmao)->offsets[(*irmao)->quantidadeKeys];
	(*remover)->quantidadeKeys++;
	//AJUSTA O IRMAO
	(*irmao)->keys[(*irmao)->quantidadeKeys] = NULL;
	mataOffsets((*irmao)->offsets[(*irmao)->quantidadeKeys]);
	(*irmao)->quantidadeKeys--;
	(*pai)->keys[indicePai-1] = (*remover)->keys[0];
	return *pai;
}

nodo_t *removeElemento(nodo_t* remover,nodo_t* pai, nodo_t* arvore, int indice, int ordem){
	int i,j,qtdMinima = (ordem-1)/2;
	nodo_t* irmao;
	
	if (!remover || !arvore)//CASO O NODO COM O ITEM A REMOVER SEJA NULO RETORNA
		return NULL;
	
	if(remover->folha){
		indice++;
		while(indice <= remover->quantidadeKeys){
			remover->keys[indice-1] = remover->keys[indice];
			remover->offsets[indice-1] = remover->offsets[indice];
			indice++;
		}
		remover->keys[indice-1] = NULL;
		mataOffsets(remover->offsets[indice-1]);
		remover->quantidadeKeys--;
		
		if(remover->quantidadeKeys >= qtdMinima)//AINDA RESTAM O MINIMO DE ELEMENTOS
			return arvore;
		
		if(!remover->pai){//A REMOCAO FOI NA RAIZ
			if(remover->quantidadeKeys){//VERIFICA SE RESTA ELEMENTOS NO N?
				return arvore;
			}else{
				return NULL;
			}
		}
		
		//NAO TERM A QUANTIDADE DE ELEMTNTOS MINIMO
		//ENCONTRA O INDICE NO PAI PARA CONFERIR SE ? POSSIVEL PEGAR EMPRESTADO DO IRMAO A ESQUERDA
		for(i = 0; i <= remover->pai->quantidadeFilhos; i++){
			if(remover->pai->filhos[i] == remover){
				break;
			}
		}
		if(remover->pai->quantidadeFilhos > i){//EXISTE UM IRMAO A DIREITA?
			irmao = remover->pai->filhos[i+1];
			if(irmao->quantidadeKeys > qtdMinima){//POSSO PEGAR EMPRESTA DESSE IRMAO?
				pai = emprestadoDireita(&pai,&remover,&irmao,i);
				return arvore;
			}
		}else{
			if(i){//EXISTE IRMAO A ESQUERDA?
				irmao = remover->pai->filhos[i-1];
				if(irmao->quantidadeKeys > qtdMinima){//POSSO PEGAR EMPRESTA DESSE IRMAO?
					pai = emprestadoEsquerda(&pai,&remover,&irmao,i);
					return arvore;
				}
			}
		}
		
	}else{//NAO FOLHA
		
	}
}


//			}else{//---------------------SE NAO FOLHA
//			
//				//AGRUPAR TODAS AS CHAVES A ESQUERDA DO NO
//				
//				if(){ //VERIFICA SE O NO TEM A ORDEM MINIMA
//					
//					//RETORNA 0, OK
//					
//				}else{ //SE NAO TER A ORDEM
//					
//					if(){ //VERIFICA SE UM DOS IRMAOS PODE EMPRESTAR
//						
//						if(){ //E O IRMAO A ESQUERDA
//							
//							//A CHAVE DO NO PAI QUE APONTA PARA O NO ATUAL SE TORNA A PRIMEIRA CHAVE DO NO ATUAL
//							//A ULTIMA CHAVE DO IRMAO A ESQUERDA SE TORNA A CHAVE PAI, RETORNA ZERO
//							
//						}else{ //E O IRMAO DA DIREITA
//							
//							//A CHAVE DO NO PAI QUE APONTA PARA O NO ATUAL SE TORNA A PRIMEIRA CHAVE DO NO ATUAL
//							//A PRIMEIRA CHAVE DO IRMAO A DIREITA SE TORNA CHAVE PAI, RETORNA ZERO
//							
//						}
//						
//					}else{ // SE OS IRMAOS NAO PODEREM EMPRESTAR
//						
//						if(){ //TEM IRMAO A ESQUERDA
//							
//							//A CHAVE PAI SE TORNA A PRIMEIRA CHAVE DO NO
//							//TODAS AS CHAVES DO IRMAO A ESQUERDA PASSAM PARA O NO ATUAL
//							//CHAMA RECURSIVAMENTE REMOVE ELEMENTO
//							//PARAMENTROS: PAI ATUAL, CHAVE PAI ATUAL, ORDEM
//							//CHAMA A FUNCAO MATA A ARVORE PASSANDO O NO ATUAL E RETORNA ZERO
//							
//						}else{ //IRMAO A DIREITA
//							
//							//A CHAVE PAI SE TORNA A ULTIMA CHAVE DO NO
//							//TODAS AS CHAVES DO NO PASSAM PARA O IRMAO A DIREITA
//							//CHAMA RECURSIVAMENTE REMOVE ELEMENTO
//							//PARAMENTROS: PAI ATUAL, INDICE ATUAL, ORDEM
//							//CHAMA A FUNCAO MATA ARVORE PASSANDO O NO ATUAL E RETORNA ZERO
//							
//						}
//					}
//				}
//			}
//		}
