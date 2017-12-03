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
int cont=0,z=0,contar_nodos=0;
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

struct vertice *inicio=NULL;
struct router *rout=NULL;
struct router *rout1,*rout2,*dato;  /*Me servira para enlazar mis nodos*/

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
	busca_nodo(inicio->siguiente,ciudad);
}

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
	printf(" Destino: %s",aris->destino);
	mostrar_conexion(aris->siguiente);
}
void mostrar_router(struct router *rout){
	if(!rout){
		return ;
	}
	printf("\n");
	printf("Origen: %s ",rout->id);
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
	peso=rand()%100;
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
	//cordenada_x=aux->cor_x;
	//cordenada_y=aux->cor_y;
	printf("\ncordenada para cadena en X:%0.1f  Y:%0.1f\n",fabs(cordenada_x),fabs(cordenada_y));
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
			printf("\nnodo:%s\n",inicio->id);
			return 1;
		}
	}
	valida_nodo(inicio->siguiente,x,y,aux);
}

/***********************************  PARTE GRAFICA************************************/
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
	/*glutPostRedisplay();*/
}

void dibuja_nodo(){
	int cont=0,cont2=0,cont3=0;
	cuenta_nodos(inicio,cont);
	cuenta_router(rout,cont3);
	printf("\nCantidad de router: %d",cont3);
	struct vertice *aux=inicio;
	printf("\nCantidad de vertices: %d",cont);
	for (int i = 0; i <cont; i++){
		dibujarEsfera(aux->cor_x,aux->cor_y);
		dibuja_cadena(aux->nodo,aux->cor_x-10,aux->cor_y+10);
		cont2=cuenta_conexiones(aux->conexion,0);  /*Contamos las aristas de mi vertices*/
		/*printf("\nCantidad de conexiones: %d\n",cont2);*/
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
		printf("CLICK IZQUIERDO");
		dibujarEsfera(rx,ry);
		dibuja_cadena(cadena,rx-20,ry+10);
		mete_archivo(cadena,rx,ry);
	}
	if(button== GLUT_RIGHT_BUTTON){
		printf("CLICK DERECHO");
		if(state == GLUT_DOWN){
			if(valida_nodo(rout,rx,ry,&dato)==1){
				printf("\nSe encontro nodo");
				cont=cont+1;
				if(cont==1){
					X1=rx;
					Y1=ry;
					rout1=dato;
					printf("X1: %d, X2:%d",X1,Y2);
				}
				if(cont==2){
					printf("SE dio dos clicks derechos");
					X2=rx;
					Y2=ry;
					rout2=dato;
					printf("X1: %d, X2:%d,Y1:%d,Y2:%d",X1,X2,Y1,Y2);
					crea_enlace2(X1,X2,Y1,Y2);
					cont=0;
					//printf("\nNodo origen:%s  -  Nodo destino: %s\n",rout1->id,rout2->id);
					enlazar_router(rout1,rout2);
					mostrar_router(rout);
					dibuja_cadena(cadena,fabs(cordenada_x),fabs(cordenada_y));
					
				}
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
	/*glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);  Anteriormente lo tenia así*/
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

/*
-Falta enlazar los vertices   (Ya quedó)
-IDEAS GENERADAS
-Ver primero como se hace linea  (Ya quedó)
-Copiar la funcion que ya tenemos

-En el for , contar los nodos que tiene  (Ya quedó)
conectados en ese momento el vertice.

-Despues dentro de ese for  (Ya quedó)
crear otro for, que llamara a su vez crea_linea,como parametro,
tendra las corrdenas origen y destino, o los apuntadores.

-Insertar el nombre de los nodos   (ya quedó)
-Falta la funcion del mause, al dar click derecho, crear nodo (Ya quedo)
-Tener en el meni de mause una opcion de enlazar nodos 
-Insertar algoritmo de Dijstra  (Ya se hizo el algoritmo, solo falta implementarlo en el proyecto)
-Ya le insertamos un ID al router cada vezque se crea un nodo con click izquerdo  (Ya quedo)
-Guardamos los nuevos nodos en una estructura, esta nos servira para el algrotimo (Ya quedo)
-Enlazar los nodos de la nueva estructua segun su enlaze en la parte grafica
*/
