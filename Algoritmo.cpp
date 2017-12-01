#include "stdio.h"
#include "stdlib.h"
#include "string.h"
struct vertices{
	char ciudad[20];
	int marca;
	struct arista *unir;
	struct vertices *siguiente;
};

struct arista{
	char nodo[20];
	char antecesor[20];
	float peso;
	float suma;
	struct arista *siguiente;
};

struct marcado{
	char ciudad[20];
	int marca;
	char padre[20];
	float suma;
	struct marcado *siguiente;
};

struct etiquetados{
	char ciudad[20];
	float suma;
	int marca;
	char padre[20];
	struct etiquetados *siguiente;
};

struct marcado *crea_nodo(struct marcado **inicio,char nodo[],float peso,char padre[]){
	struct marcado *nuevo=(struct marcado *)malloc(sizeof(struct marcado));
	if(!nuevo){
		return NULL;
	}
	strcpy(nuevo->ciudad,nodo);
	strcpy(nuevo->padre,padre);
	nuevo->suma=peso;
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return *inicio;
}

struct etiquetados *crea_etique(struct etiquetados **inicio,char nodo[],float peso,char padre[]){
	struct etiquetados *nuevo=(struct etiquetados *)malloc(sizeof(struct etiquetados));
	if(!nuevo){
		return NULL;
	}
	strcpy(nuevo->ciudad,nodo);
	strcpy(nuevo->padre,padre);
	nuevo->suma=peso;
	nuevo->marca=0;
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return *inicio;
}

void mostrar_etiquetados(struct etiquetados *inicio){
	if(inicio){
		printf("\n%s   Padre:%s Peso:%0.1f",inicio->ciudad,inicio->padre,inicio->suma);
		mostrar_etiquetados(inicio->siguiente);
	}
	return ;
}

struct vertices *crea_vertice(struct vertices **inicio){
	struct vertices *nuevo=(struct vertices*)malloc(sizeof(struct vertices));
	if(!nuevo){
		return NULL;
	}
	printf("Nombre de la ciudad:");
	fflush(stdin);
	scanf("%[^\n]",&nuevo->ciudad);
	nuevo->marca=0;
	nuevo->unir=NULL;
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return nuevo;
}

struct arista *crea_arista(struct arista **inicio,char nodo[],float peso,char sucesor[]){
	struct arista *nuevo=(struct arista*)malloc(sizeof(struct arista));
	if(!nuevo){
		return NULL;
	}
	nuevo->peso=peso;
	nuevo->suma=0;
	strcpy(nuevo->nodo,nodo);
	strcpy(nuevo->antecesor,sucesor);
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return nuevo;
}

void mostrar_arista(struct arista *aris){
	if(aris){
		printf(" %s - antecesor:%s",aris->nodo,aris->antecesor);
		mostrar_arista(aris->siguiente);
	}
	return ;
}

void mostrar_vertices(struct vertices *inicio){
	if(inicio){
		printf("\n %s -> ",inicio->ciudad);
		mostrar_arista(inicio->unir);
		mostrar_vertices(inicio->siguiente);
	}
	return ;
}

struct vertices *busca_nodo(struct vertices *ciudad,char nodo[]){
	if(!ciudad){
		return NULL;
	}
	if(strcmp(nodo,ciudad->ciudad)==0){
		return ciudad;
	}
	busca_nodo(ciudad->siguiente,nodo);
}

int enlazar(struct vertices **inicio){
	struct vertices *aux1,*aux2;
	float peso;
	if(!*inicio){
		printf("\nNo existe ningun nodo"); 
		return 0;
	}
	printf("Ciudades posibles\n");
	mostrar_vertices(*inicio);
	char nodo[20],nodo2[20];
	printf("\nVertice oringen: ");
	fflush(stdin);
	scanf("%[^\n]",nodo);
	printf("\nVertice a conectar:");
	fflush(stdin);
	scanf("%[^\n]",nodo2);
	aux1=busca_nodo(*inicio,nodo);
	aux2=busca_nodo(*inicio,nodo2);
	if(aux1==NULL or aux2==NULL){
		printf("\nNodos no existentes");
		return 0;
	}
	printf("Distancia entre los nodos: ");
	scanf("%f,",&peso);
	crea_arista(&aux1->unir,nodo2,peso,nodo);
	crea_arista(&aux2->unir,nodo,peso,nodo2);
	return 0;
}

int mejor_ruta(struct etiquetados *inicio,struct etiquetados **esclavo){
	if(!inicio){
		return 0;
	}
	if(inicio->suma<(*esclavo)->suma){
		if(inicio->marca==0){
			*esclavo=inicio;	
		}else{
			mejor_ruta(inicio->siguiente,esclavo);	
		}
	}
	mejor_ruta(inicio->siguiente,esclavo);
}

struct marcado *busca_marcado(struct marcado *marca,char nodo[]){
	if(!marca){
		return NULL;
	}
	if(strcmp(nodo,marca->ciudad)==0){
		printf("\nExiste un nodo en marcados");
		return marca;
	}
	busca_marcado(marca->siguiente,nodo);
}

struct etiquetados *busca_etique(struct etiquetados *inicio,char nodo[]){
	if(!inicio){
		return NULL;
	}
	if(strcmp(nodo,inicio->ciudad)==0){
		return inicio;
	}
	busca_etique(inicio->siguiente,nodo);	
}

int elimina_etiqueta(struct etiquetados **inicio,char nodo[]){
	if(*inicio==NULL){
		return 0;
	}
	if(strcmp(nodo,(*inicio)->ciudad)==0){
			struct etiquetados *aux=*inicio;
			*inicio=(*inicio)->siguiente;
			free(aux);	
			return 0;
	}
	elimina_etiqueta(&(*inicio)->siguiente,nodo);
}

int expande(struct arista **inicio,struct marcado **marca,struct etiquetados **etiqueta,char padre[]){
	if(*inicio==NULL){
		return 0;
	}
	printf("\npadre:%s acomulado:%0.1f",(*marca)->ciudad,(*marca)->suma);
	struct marcado *aux2=busca_marcado(*marca,(*inicio)->nodo); 	//Tengo que buscar si existe en marcados
	if(!aux2){
		struct etiquetados *aux3=busca_etique(*etiqueta,(*inicio)->nodo);
		if(aux3){
			printf("\nExiste un nodo en etiqetados aux3:%s  distancia:%0.1f",aux3->ciudad,aux3->suma);
			printf("\nNodo a comparar: %s   peso:%0.1f",(*inicio)->nodo,(*inicio)->peso+(*marca)->suma);  
			if(((*inicio)->peso+(*marca)->suma)<aux3->suma){
				printf("\nes menor,eliminamos a aux3 de etiquetados"); 
				elimina_etiqueta(etiqueta,aux3->ciudad);  //Eliminamos
				crea_etique(etiqueta,(*inicio)->nodo,(*marca)->suma+(*inicio)->peso,(*marca)->ciudad);  //insertamos el nuevo nodo menor
			}else{
				expande(&(*inicio)->siguiente,marca,etiqueta,padre);
			}
		}else{
			struct etiquetados *aux=crea_etique(etiqueta,(*inicio)->nodo,(*marca)->suma+(*inicio)->peso,padre);
			printf("\ninsertado:%s",aux->ciudad);	
		}
	}
	expande(&(*inicio)->siguiente,marca,etiqueta,padre);
}

int contar_marcados(struct marcado *inicio,int cont){
	if(!inicio){
		return cont;
	}
	cont=cont+1;
	contar_marcados(inicio->siguiente,cont);
}

int contar_vertices(struct vertices *inicio,int cont){
	if(!inicio){
		return cont;
	}
	cont=cont+1;
	contar_vertices(inicio->siguiente,cont);
}

int marcar(struct etiquetados **inicio){
	(*inicio)->marca=1;
	return 0;
}

int busca_indicador(struct etiquetados *inicio,char nodo[]){
	if(!inicio){
		return 0;
	}
	if(strcmp(nodo,inicio->ciudad)==0){
		if(inicio->marca==1){
			printf("Ya esta en marcados, se eligira otro con un menor costo");
			return 1;
		}
	}
	busca_indicador(inicio->siguiente,nodo);
}

int parte2(struct vertices **inicio,struct marcado **marca,struct etiquetados **etique,char nodo[],struct vertices *nodos){

	struct etiquetados *esclavo; 
	struct vertices *aux;
	printf("\nNodo a expandir:%s",(*inicio)->ciudad);
	if(*marca==NULL){
		crea_nodo(marca,(*inicio)->ciudad,0,(*inicio)->ciudad);
	} 
	expande(&(*inicio)->unir,marca,etique,(*inicio)->ciudad);  //Expandimos el nodo donde nos encontramos
	esclavo=*etique;
	mejor_ruta((*etique)->siguiente,&esclavo);
	if(busca_indicador(*etique,esclavo->ciudad)==1){
		esclavo=*etique;
		mejor_ruta((*etique)->siguiente,&esclavo);
	}
	printf("\nMejor ruta: %s  Distancia:%0.1f\n",esclavo->ciudad,esclavo->suma);
	marcar(&esclavo);
	crea_nodo(marca,esclavo->ciudad,esclavo->suma,esclavo->padre);  //Lo metemos en los marcados
	aux=busca_nodo(nodos,esclavo->ciudad);
	//elimina_etiqueta(etique,esclavo->ciudad);  Tenemos que tomar en cuenta que aun sigue en etiquetados y puede repetirce
	mostrar_etiquetados(*etique);
	printf("\n");
	system("pause");
	if(contar_marcados(*marca,0)==(contar_vertices(nodos,0)-1)){
		printf("\nSe marcaron todos los nodos\n");
		printf("\nRuta minima de: %s para cada nodo es de:\n",nodo);
		mostrar_etiquetados(*etique);
		return 0;		
	}
	parte2(&aux,marca,etique,nodo,nodos);	
	return 0;
}

int algritmo(struct vertices **inicio){
	char nodo[20];
	struct vertices *aux;
	struct marcado *marcados=NULL;
	struct etiquetados *etiqueta=NULL;
	printf("\nNodo inicial: ");
	fflush(stdin);
	scanf("%[^\n]",nodo);
	aux=busca_nodo(*inicio,nodo);
	if(!aux){
		printf("\nEl nodo no existe");
	}
	parte2(&aux,&marcados,&etiqueta,nodo,*inicio);
	return 1;
}

void menu(struct vertices **inicio){
	int opc;
	do{
		printf("1.Crear vertice\n2.Mostrar vertices\n3.Enlazar vertices\n4.Algoritmo\n5.Salir\nOpcion: ");
		scanf("%d",&opc);
		switch(opc){
			case 1: crea_vertice(inicio); break;
			case 2: mostrar_vertices(*inicio); break;
			case 3: enlazar(inicio); break;
			case 4: algritmo(inicio); break;
		}	
	printf("\n");	
	system("pause");
	system("cls");
	}while(opc!=5);
}


int main(){
	struct vertices *inicio=NULL;
	menu(&inicio);
	return 0;
}
