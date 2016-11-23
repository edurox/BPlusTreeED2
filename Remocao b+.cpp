nodo_t *removeElemento(nodo_t* remover, nodo_t* arvore, int indice, int ordem){
	int i,j,qtdMinima = (ordem-1)/2;
	
	if (!remover || !arvore)//CASO O NODO COM O ITEM A REMOVER SEJA NULO RETORNA
		return NULL;
	
	if(remover->folha){
		mataOffsets(remover->offsets[indice]);
		indice++;
		while(indice <= remover->quantidadeKeys){
			remover->keys[indice-1] = remover->keys[indice];
			remover->offsets[indice-1] = remover->offsets[indice];
			indice++;
		}
		remover->keys[indice-1] = NULL;
		mataOffsets(remover->offsets[indice-1]);
		remover->quantidadeKeys--;
		
		if(!remover->pai){//A REMOCAO FOI NA RAIZ
			if(remover->quantidadeKeys){//VERIFICA SE RESTA ELEMENTOS NO NÓ
				return arvore;
			}else{
				return NULL;
			}
		}
		
		if(remover->quantidadeKeys >= qtdMinima){//AINDA RESTAM O MINIMO DE ELEMENTOS
			return arvore;
		}else{//NAO TERM A QUANTIDADE DE ELEMTNTOS MINIMO
			//ENCONTRA O INDICE NO PAI PARA CONFERIR SE É POSSIVEL PEGAR EMPRESTADO DO IRMAO A ESQUERDA
			for(i = 0; i <= remover->pai->quantidadeFilhos; i++){
				if(remover->pai->filhos[i] == remover){
					break;
				}
			}
			
			if(remover->pai->quantidadeFilhos > i){//EXISTE UM IRMAO A DIREITA?
				if(remover->pai->filhos[i+1]->quantidadeKeys > qtdMinima){//POSSO PEGAR EMPRESTA DESSE IRMAO?
					remover->keys[remover->quantidadeFilhos+1] = remover->pai->filhos[i+1]->keys[0];
					remover->offsets[remover->quantidadeFilhos+1] = remover->pai->filhos[i+1]->offsets[0];
					//AGRUPO OS ELEMENTOS NO INICIO NOVAMENTE
					mataOffsets(remover->pai->filhos[i+1]->offsets[0]); 
					for(j = 1; j <= remover->pai->filhos[i+1]->quantidadeFilhos; j++){
						remover->pai->filhos[i+1]->keys[j-1] = remover->pai->filhos[i+1]->keys[j];
						remover->pai->filhos[i+1]->offsets[j-1] = remover->pai->filhos[i+1]->offsets[j];
					}
					//MATA A ULTIMA LISTA DE OFFSET, COLOCO COMO NUL A ULTIMA HASH E DIMINUI O NUMERO DE FILHOS
					mataOffsets(remover->pai->filhos[i+1]->offsets[remover->pai->filhos[i+1]->quantidadeKeys]);
					remover->pai->filhos[i+1]->keys[remover->pai->filhos[i+1]->quantidadeKeys] = NULL;
					remover->pai->filhos[i+1]->quantidadeKeys--;
				}
			}else{
				if(i){//EXISTE UM IRMAO A ESQUERDA?
					if(remover->pai->filhos[i-1]->quantidadeKeys > qtdMinima){//POSSO PEGAR EMPRESTADO DESSE IRMAO?
						//EMPURO UMA POSICAO A ESQUERDA TODOS OS ELEMENTOS
						for(j = remover->quantidadeKeys; j >= 0; j++){
							remover->keys[j+1] = remover->keys[j];
							remover->offsets[j+1] = remover->offsets[j];
						}
						//RECEBE O NOVO ELEMENTO
						mataOffsets(remover->offsets[0]);
						remover->keys[0] = remover->pai->filhos[i-1]->keys[remover->pai->quantidadeKeys];
						remover->offsets[0] = remover->pai->filhos[i-1]->offsets[remover->pai->quantidadeKeys];
						remover->quantidadeFilhos++;
						//APAGA O ELEMENTO DO IRMAO
						remover->pai->filhos[i-1]->keys[remover->pai->filhos[i-1]->quantidadeFilhos] = NULL;
						mataOffsets(remover->pai->filhos[i-1]->offsets[remover->pai->filhos[i-1]->quantidadeFilhos]);
						remover->pai->filhos[i-1]->quantidadeFilhos--;
					}
				}
			}
		}
	}else{
		
	}
	return arvore;
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
