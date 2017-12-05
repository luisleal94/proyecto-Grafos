#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "GL/glut.h"
#include <math.h>

float cx=0,cy=0,x=0,y=0;
float PI=3.1416;
float radio=10;
int indicador=0;
float rx,ry;
float winWid = 1000,winHeight = 600;
int cont=0,z=0,contar_nodos=0,cont_click=0;
int X1,X2,Y1,Y2;  /*contenfra los valores del click izquierdo*/
float peso;
char cadena[6];
float cordenada_x,cordenada_y;
/*Estructura que contiene los tados en el archivo*/
struct ciudad{
	char nodo[20];
	float cor_y,cor_x;
}nod;	   

/*Estructura de tipo lista que contiene los vertices (SOLO LA USAREMOS EN LA PARTE GRAFICA)*/
struct vertice{
	char nodo[20];
	float medida;
	int marca;
	float cor_y,cor_x;
	struct arista *conexion;
	struct vertice *siguiente;
};

struct arista{
	char destino[20];
	char sucesor[20];
	float cor_y,cor_x;
	float peso;
	struct arista *siguiente;
};
/*En esta estructtura trabajaremos en el argoritmo, sustituira a vertices*/
struct router{
	char id[20];
	struct arista *conecta;
	float cor_y,cor_x;
	struct router *siguiente;
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

struct vertice *inicio=NULL;
struct router *rout=NULL;
struct marcado *marcados=NULL;
struct etiquetados *etiqueta=NULL;
struct router *rout1,*rout2,*rout3,*dato,*rout4;  /*Me servira para enlazar mis nodos*/

/********************  LIBERACION DE MEMORIA    ********************************/

void libera_marcado(struct marcado **marca){
	if(*marca==NULL){
		return ;
	}
	struct marcado *aux=*marca;
	*marca=(*marca)->siguiente;
	free(aux);
	libera_marcado(marca);
}

void libera_etiquetado(struct etiquetados **etiqueta){
	if(*etiqueta==NULL){
		return ;
	}
	struct etiquetados *aux=*etiqueta;
	*etiqueta=(*etiqueta)->siguiente;
	free(aux);
	libera_etiquetado(etiqueta);
}

/**********************************************************************************/
void dibuja_cadena(char *cadena,float x, float y);
int llenado()
{
	FILE *archivo;
	archivo=fopen("archi.dat", "a+b");
	if(!archivo)
	{
		printf("\nERROR DE ARCHIVO\n");
		system("pause");
		return 0;
	}
	printf("\nIDENTIFICADOR DEL NODO: ");
	fflush(stdin);
	gets(nod.nodo);
	printf("\nCORDENADA EN X (0-1000): ");
	scanf("%f",&nod.cor_x);
	printf("\nCORDENADA EN Y (0-600): ");
	scanf("%f",&nod.cor_y);
	fwrite(&nod,sizeof(nod),1,archivo);
	fclose(archivo);
	
	return 0;
}

int cuenta_nodos(struct vertice *inicio,int &cont){
	if(inicio==NULL){
		return 0;
	}
	cont=cont+1;
	cuenta_nodos(inicio->siguiente,cont);
}

int cuenta_conexiones(struct arista *inicio,int cont){
	if(inicio==NULL){
		return cont;
	}
	cont=cont+1;
	cuenta_conexiones(inicio->siguiente,cont);
}

int cuenta_router(struct router *inicio,int &cont){
	if(inicio==NULL){
		return cont;
	}
	cont=cont+1;
	cuenta_router(inicio->siguiente,cont);
}

int lee_archivo(struct vertice **inicio,struct router **rout){
	char ciudad[20];
	FILE *archivo;
	archivo=fopen("archi.dat","r+b");
	if(!archivo) {
		printf("\nERROR DE ARCHIVO");
		system("pause");
		return 0;
	}
	while(!feof(archivo))
	{
		struct vertice *nuevo=(struct vertice *)malloc(sizeof(struct vertice));
		if(!nuevo){
			return 0;
		}
		
		struct router *nuevo2=(struct router *)malloc(sizeof(struct router));
		if(!nuevo2){
			return 0;
		}
		
		if(fread(&nod,sizeof(nod),1,archivo)!=0)
		{
			printf("\n\nNodo: %s",nod.nodo);
			strcpy(ciudad,nod.nodo);
			printf("\nCoordenada en X: %0.2f",nod.cor_x);
			printf("\nCoordenada en Y: %0.2f\n",nod.cor_y);
			strcpy(nuevo->nodo,ciudad);
			nuevo->cor_x=nod.cor_x;
			nuevo->cor_y=nod.cor_y;
			nuevo->marca=0;
			nuevo->conexion=NULL;
			nuevo->siguiente=*inicio;
			*inicio=nuevo;
			
			strcpy(nuevo2->id,ciudad);
			nuevo2->cor_x=nod.cor_x;
			nuevo2->cor_y=nod.cor_y;
			nuevo2->conecta=NULL;
			nuevo2->siguiente=*rout;
			*rout=nuevo2;
			contar_nodos=contar_nodos+1;
		}
	}
	return 0;
}

struct arista *crea_arista(struct arista **inicio,struct vertice *dest,struct vertice *origen,float distancia){
	struct arista *nuevo=(struct arista*)malloc(sizeof(struct arista));
	if(!nuevo){
		return NULL;
	}
	strcpy(nuevo->destino,dest->nodo);
	strcpy(nuevo->sucesor,origen->nodo);
	nuevo->cor_x=dest->cor_x;
	nuevo->cor_y=dest->cor_y;
	nuevo->peso=distancia;
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return *inicio;
}

void mostrar_arista(struct arista *inicio){
	if(inicio){
		printf("\t %s -> ",inicio->destino);
		printf("\n\tCordenada x:%0.1f Cordenada Y:%0.1f\n\n",inicio->cor_x,inicio->cor_y);
		mostrar_arista(inicio->siguiente);
	} 
	return;
}

void mostrar_lista(struct vertice *inicio){
	if(!inicio){
		return ;
	}
	printf("\n\nNodo: %s",inicio->nodo);
	printf("     Cordenada en X: %0.2f",inicio->cor_x);
	printf("     Coordenada en Y: %0.2f\n",inicio->cor_y);
	mostrar_arista(inicio->conexion);
	mostrar_lista(inicio->siguiente);
}

int libera_lista(struct vertice **inicio){
	if(!*inicio){
		printf("\nMemoria liberada\n");
		return 0;
	}
	struct vertice *aux=*inicio;
	*inicio=(*inicio)->siguiente;
	free(aux);
	libera_lista(inicio);
}

struct vertice *busca_nodo(struct vertice *inicio,char *ciudad){
	if(!inicio){
		return NULL;
	}
	if(strcmp(inicio->nodo,ciudad)==0){
		return inicio;
	}
	busca_nodo(inicio->siguiente,ciudad);}

int enlaza(struct vertice **inicio){
	char nodo[20],nodo2[20];
	float distancia;
	struct vertice *aux,*aux2;
	if(!*inicio){
		return 0;
	}
	printf("Nodo orige: ");
	fflush(stdin);
	scanf("%[^\n]",nodo);
	aux=busca_nodo(*inicio,nodo);
	printf("Nodo destino: ");
	fflush(stdin);
	scanf("%[^\n]",nodo2);
	aux2=busca_nodo(*inicio,nodo2);
	
	if(!aux || !aux2){
		printf("Uno de los nodos no existe"); return 0;
	}
	printf("Distancia entre los nodos: ");
	scanf("%f,",&distancia);
	crea_arista(&aux->conexion,aux2,aux,distancia);
	crea_arista(&aux2->conexion,aux,aux2,distancia);
	return 1;
}  

int menuempleados(){
	int opc;
	char c;
	do{
		printf("1.Insertar cuidad\n2.Ver archivo\n3.Mostrar vertices\n4.Enlazar nodos\n5.SALIR");
		printf("\nOPCION:");
		scanf("%d",&opc);
		system("cls");
		switch(opc)
		{
		case 1: llenado();
		break;
		case 2: libera_lista(&inicio);
		lee_archivo(&inicio,&rout); break;
		case 3: mostrar_lista(inicio); break;
		case 4: enlaza(&inicio); break;
		}
		system("pause");
		system("cls");	
	}while(opc!=5);
	printf("\n *CLICK DERECHO, se crea nodo\n* CLICK IZQUIERDO SE ENLAZA NODOS...\n");
	scanf("%c",&c);
	system("pause");
	system("cls");
	return 0;
}

void mostrar_conexion(struct arista *aris){
	if(!aris){
		return ;
	}
	printf(" %s ",aris->destino);
	mostrar_conexion(aris->siguiente);
}
void mostrar_router(struct router *rout){
	if(!rout){
		return ;
	}
	printf("\n");
	printf("Origen: %s ->",rout->id);
	mostrar_conexion(rout->conecta);
	mostrar_router(rout->siguiente);
}

struct arista *crea_conexion(struct arista **inicio,struct router *dest,struct router *origen,float distancia){
	struct arista *nuevo=(struct arista*)malloc(sizeof(struct arista));
	if(!nuevo){
		return NULL;
	}
	strcpy(nuevo->destino,dest->id);
	strcpy(nuevo->sucesor,origen->id);
	nuevo->cor_x=dest->cor_x;
	nuevo->cor_y=dest->cor_y;
	nuevo->peso=distancia;
	nuevo->siguiente=*inicio;
	*inicio=nuevo;
	return *inicio;
}

struct router *busca_router(struct router *inicio,char rout[]){
	if(!inicio){
		return NULL;
	}
	if(strcmp(inicio->id,rout)==0){
		return inicio;
	}
	busca_router(inicio->siguiente,rout);
}


int enlazar_router(struct router *rout1,struct router *rout2){
	struct router *aux=busca_router(rout,rout1->id);
	struct router *aux2=busca_router(rout,rout2->id);
	printf("\naux:%s",aux->id);
	printf("\naux2:%s",aux2->id);
	srand (time(NULL));
	peso=(rand()%100)+1;
	printf("  Peso: %0.1f",peso);
	sprintf(cadena,"%0.1f",peso);
	if(aux->cor_x>aux2->cor_x){
		cordenada_x=aux->cor_x-60;
		if(aux->cor_y>aux2->cor_y){
			cordenada_y=aux->cor_y-40;
		}else{
			cordenada_y=aux->cor_y+40;
		}
	}else{
		cordenada_x=aux->cor_x+60;
		if(aux->cor_y>aux2->cor_y){
			cordenada_y=aux->cor_y-40;
		}else{
			cordenada_y=aux->cor_y+40;
		}
	}
	crea_conexion(&aux->conecta,rout2,rout1,peso); /*Provando metiendo pesos de 50*/
	crea_conexion(&aux2->conecta,rout1,rout2,peso);  /*Provando metiendo pesos de 500*/
	return 1;
}

int valida_nodo(struct router *inicio,float x,float y,struct router **aux){
	if(inicio==NULL){
		return 0;
	}
	if(inicio->cor_x>=(x-5) and inicio->cor_x<=(x+5)){
		if (inicio->cor_y>=(y-5) and inicio->cor_y<=(y+5)){
			*aux=inicio;
			return 1;
		}
	}
	valida_nodo(inicio->siguiente,x,y,aux);
}

/***********************************  PARTE GRAFICA************************************/
void pintar_barra(float x,float y)
{
	glPushMatrix();
	glTranslatef(x,y-15,0);
	glColor3f(1.0,1.0,0.0);
	glRectf(0,0,25,15);
	glPopMatrix();
	
}

void cambia_color(int pelotax,int pelotay){
	
	glPushMatrix(); 
	glColor3f(1.0,1.0,1.0); 
	glTranslatef(pelotax,pelotay,0); 
	glBegin(GL_TRIANGLE_FAN); 
	glVertex2f(0,0); 
	for (float ang = 0; ang <= PI*4; ang += PI/8) {
		cx=radio*cos(ang)+x;
		cy=radio*sin(ang)+y;
		glVertex2f(cx,cy);	
	}
	glFlush();
	glEnd(); 
	glPopMatrix(); 
}

void dibuja_cadena(char *cadena,float x, float y){
	unsigned int i;
	glColor3f(1,1,1);
	for (i=0; i<strlen(cadena); i++){
		glRasterPos3f (x,y,0);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, cadena[i]);	    
		x=x+8;	
	}
}

void crea_enlace2(int x1,int x2, int y1,int y2){
	glColor3f(0,.8,.5);		/*COLOR VERDE AGUA*/
	/*glColor3f(1.0,1.0,0.0); COLOR AMARILLO*/
	GLfloat ancho = 5.0f;  /*variable que contiene el ancho de la linea*/
	glLineWidth(ancho); 
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(x1,y1, 0);
	glVertex3f(x2,y2, 0);
	glEnd();
	glPopMatrix();
}

void dibujarEsfera(int pelotax,int pelotay){
	
	glPushMatrix(); 
	glColor3f(0.8,0.2,0.9); 
	glTranslatef(pelotax,pelotay,0); 
	glBegin(GL_TRIANGLE_FAN); 
	glVertex2f(0,0); 
	for (float ang = 0; ang <= PI*4; ang += PI/8) {
		cx=radio*cos(ang)+x;
		cy=radio*sin(ang)+y;
		glVertex2f(cx,cy);	
	}
	glFlush();
	glEnd(); 
	glPopMatrix();
}

void dibuja_nodo(){
	int cont=0,cont2=0,cont3=0;
	cuenta_nodos(inicio,cont);
	cuenta_router(rout,cont3);
	printf("\nCantidad de router: %d",cont3);
	struct vertice *aux=inicio;
	for (int i = 0; i <cont; i++){
		dibujarEsfera(aux->cor_x,aux->cor_y);
		dibuja_cadena(aux->nodo,aux->cor_x-10,aux->cor_y+10);
		cont2=cuenta_conexiones(aux->conexion,0);  /*Contamos las aristas de mi vertices*/
		struct arista *conecta=aux->conexion;
		for (int i = 0; i < cont2; i++){
			crea_enlace2(conecta->cor_x,aux->cor_x,conecta->cor_y,aux->cor_y);	
			conecta=conecta->siguiente;
		}
		aux=aux->siguiente;
	}
	glFlush();
	glutSwapBuffers();
	
}

/*****************************    ALGORITMO     **************************************/
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

struct marcado *crea_marca(struct marcado **inicio,char nodo[],float peso,char padre[]){
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

struct marcado *busca_marcado(struct marcado *marca,char nodo[]){
	if(!marca){
		return NULL;
	}
	if(strcmp(nodo,marca->ciudad)==0){
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
	struct marcado *aux2=busca_marcado(*marca,(*inicio)->destino); 	//Tengo que buscar si existe en marcados
	if(!aux2){
		struct etiquetados *aux3=busca_etique(*etiqueta,(*inicio)->destino);
		if(aux3){
			if(((*inicio)->peso+(*marca)->suma)<aux3->suma){
				elimina_etiqueta(etiqueta,aux3->ciudad);  //Eliminamos
				crea_etique(etiqueta,(*inicio)->destino,(*marca)->suma+(*inicio)->peso,(*marca)->ciudad);  //insertamos el nuevo nodo menor
			}else{
				expande(&(*inicio)->siguiente,marca,etiqueta,padre);
			}
		}else{
			crea_etique(etiqueta,(*inicio)->destino,(*marca)->suma+(*inicio)->peso,padre);
		}
	}
	expande(&(*inicio)->siguiente,marca,etiqueta,padre);
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

int busca_indicador(struct etiquetados *inicio,char nodo[]){
	if(!inicio){
		return 0;
	}
	if(strcmp(nodo,inicio->ciudad)==0){
		if(inicio->marca==1){
			//printf("Ya esta en marcados, se eligira otro con un menor costo");
			return 1;
		}
	}
	busca_indicador(inicio->siguiente,nodo);
}

int marcar(struct etiquetados **inicio){
	(*inicio)->marca=1;
	return 0;
}

void mostrar_etiquetados(struct etiquetados *inicio){
	if(inicio){
		printf("\n%s   Padre:%s Peso:%0.1f",inicio->ciudad,inicio->padre,inicio->suma);
		mostrar_etiquetados(inicio->siguiente);
	}
	return ;
}

int contar_marcados(struct marcado *inicio,int cont){
	if(!inicio){
		return cont;
	}
	cont=cont+1;
	contar_marcados(inicio->siguiente,cont);
}

int contar_router(struct router *inicio,int cont){
	if(!inicio){
		return cont;
	}
	cont=cont+1;
	contar_router(inicio->siguiente,cont);
}

int parte2(struct router **inicio,struct marcado **marca,struct etiquetados **etique,char nodo[],struct router *nodos){
	
	struct etiquetados *esclavo; 
	struct router *aux;
	if(*marca==NULL){
		crea_nodo(marca,(*inicio)->id,0,(*inicio)->id);
	} 
	expande(&(*inicio)->conecta,marca,etique,(*inicio)->id);  //Expandimos el nodo donde nos encontramos
	esclavo=*etique;
	mejor_ruta((*etique)->siguiente,&esclavo);
	if(busca_indicador(*etique,esclavo->ciudad)==1){
		esclavo=*etique;
		mejor_ruta((*etique)->siguiente,&esclavo);
	}
	marcar(&esclavo);
	crea_nodo(marca,esclavo->ciudad,esclavo->suma,esclavo->padre);  //Lo metemos en los marcados
	aux=busca_router(rout,esclavo->ciudad);  //Aqui me marca error
	if(contar_marcados(*marca,0)==(contar_router(rout,0)-1)){
		printf("\nSe marcaron todos los nodos\n");
		printf("\nRuta minima de: %s para cada nodo es de:\n",nodo);
		mostrar_etiquetados(*etique);
		printf("\n\n");
		system("pause");
		return 0;		
	}
	parte2(&aux,marca,etique,nodo,nodos);	
}

int algritmo(struct router **inicio){
	printf("\nNodo inicial: %s",(*inicio)->id);
	parte2(inicio,&marcados,&etiqueta,(*inicio)->id,*inicio);
	return 1;
}
/*********************        FUNCIONES PARA EL MAUSE         ******************/
void dibuja(){
	glPushMatrix(); 
	glColor3f(0.8,0.2,0.9); 
	glTranslated(rx,ry,0); 
	glBegin(GL_TRIANGLE_FAN); 
	glVertex2f(0,0); 
	for (float ang = 0; ang <= PI*4; ang += PI/8) {
		cx=radio*cos(ang)+x;
		cy=radio*sin(ang)+y;
		glVertex2f(cx,cy);	
	}
	glEnd(); 
	glPopMatrix();
}


void redraw( void )/*Este metodo nos servira para redibujar nuestra figura*/
{
	dibuja();  /* SI LA COMENTO  HACE OTRAS FUNCIONES*/
	glutSwapBuffers();
}

void motion(int x, int y)/*este metodo realizara las funciones para darle coordenadas a nuestro rectangulo al mover el mouse*/
{
	printf("\nmotion -> X: %d Y:%d",x,y);
	rx = float(x);
	ry = winHeight;
	ry=ry-float(y);
}

int mete_archivo(char nodo[],float x,float y){ //Esta funcion mete los nodos creados por el mause
	FILE *archivo;
	archivo=fopen("archi.dat", "a+b");
	if(!archivo)
	{
		printf("\nERROR DE ARCHIVO\n");
		system("pause");
		return 0;
	}
	strcpy(nod.nodo,nodo);
	nod.cor_x=x;
	nod.cor_y=y;
	fwrite(&nod,sizeof(nod),1,archivo);
	fclose(archivo);
	
	struct router *nuevo=(struct router *)malloc(sizeof(struct router));
	if(!nuevo){
		return 0;
	}
	strcpy(nuevo->id,nodo);
	nuevo->cor_x=x;
	nuevo->cor_y=y;
	nuevo->conecta=NULL;
	nuevo->siguiente=rout;
	rout=nuevo;
}

int recorrido(struct etiquetados *cami,char origen[]){
	if(strcmp(origen,cami->ciudad)==0){
		return 0;
	}
	struct etiquetados *hijo=busca_etique(etiqueta,cami->padre);
	//struct router *hijo2=busca_router(rout,hijo->ciudad);
	if(!hijo){
		return 0;
	}
	struct router *hijo2=busca_router(rout,hijo->ciudad);
	Sleep(2000);
	pintar_barra(hijo2->cor_x,hijo2->cor_y);
	recorrido(hijo,origen);	
}

void mousebutton(int button, int state, int x, int y)/*metodo para dar moviemiento a traves del mouse*/
{
	printf("\nMauseButton -> X: %d Y:%d",x,y);
	rx = float(x);
	ry = winHeight;
	ry=ry-float(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		char cadena[20];
		sprintf(cadena,"Router %d",contar_nodos+1);
		contar_nodos=contar_nodos+1;
		dibujarEsfera(rx,ry);
		dibuja_cadena(cadena,rx-20,ry+10);
		mete_archivo(cadena,rx,ry);
	}
	if(button== GLUT_RIGHT_BUTTON){
		if(state == GLUT_DOWN){
			if(valida_nodo(rout,rx,ry,&dato)==1){
				cont=cont+1;
				if(cont==1){
					X1=rx;
					Y1=ry;
					rout1=dato;
				}
				if(cont==2){ /*Se dio un segundo click derecho*/
					X2=rx;
					Y2=ry;
					rout2=dato;
					crea_enlace2(X1,X2,Y1,Y2);
					cont=0;
					enlazar_router(rout1,rout2);
					mostrar_router(rout);
					dibuja_cadena(cadena,fabs(cordenada_x),fabs(cordenada_y));
				}
			}
		}		
	}
	if(button==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN){
		if(valida_nodo(rout,rx,ry,&dato)==1){
			cont_click=cont_click+1;
			if(cont_click==1){
				rout3=dato;
				/*Iniciamos algoritmo*/
				struct router *aux=busca_router(rout,rout3->id);
				algritmo(&aux);
			}
			if(cont_click==2){
				cont_click=0;
				rout4=dato;
				struct etiquetados *aux=busca_etique(etiqueta,rout4->id);
				printf("\nLa distancia minia de %s a %s es de:%0.1f\n",rout3->id,aux->ciudad,aux->suma);
				pintar_barra(rout3->cor_x,rout3->cor_y);
				Sleep(1500);
				pintar_barra(rout4->cor_x,rout4->cor_y);  //mostramos el mensaje grafico
				recorrido(aux,rout3->id);
				libera_etiquetado(&etiqueta); /*Tenemos que liberar memoria*/
				libera_marcado(&marcados);
			}
		}
	}
	glFlush();
	glutSwapBuffers();
}
/**************************************************************/
void reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D(0,w,0,h);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_SINGLE);
	glutInitWindowSize (1000,600);
	glutInitWindowPosition (180,80);
	glutCreateWindow ("Proyecto");
	glClearColor(0,0,0,0);
	dibuja_nodo();
	glutDisplayFunc(dibuja_nodo);
	glutMouseFunc(mousebutton); /*funcion exclusiva para el mouse */
	glutReshapeFunc (reshape_cb);
}

int main(int argc, char *argv[]){
	glutInit (&argc, argv);
	if(indicador==0){
		menuempleados();
		indicador=1;
	}
	initialize();
	glutMainLoop();
	return 0;
}
