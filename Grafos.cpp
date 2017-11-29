#include"stdio.h"
#include"stdlib.h"
#include"stdlib.h"
#include"string.h"
#include "GL/glut.h"
#include <math.h>

float cx=0,cy=0,x=0,y=0;
float PI=3.1416;
float radio=10;
int indicador=0;

/*Estructura que contiene los tados en el archivo*/
struct ciudad{
	char nodo[20];
    float cor_y,cor_x;
}nod;	   

/*Estructura de tipo lista que contiene los vertices*/
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

struct vertice *inicio=NULL;

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

int lee_archivo(struct vertice **inicio){
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
	printf("Distancia entre los nodos");
	scanf("%f,",&distancia);
	crea_arista(&aux->conexion,aux2,aux,distancia);
	crea_arista(&aux2->conexion,aux,aux2,distancia);
	return 1;
}  

int menuempleados()
{
 	int opc;
 	do
 	{
	  	printf("1.Insertar cuidad\n2.Ver archivo\n3.Mostrar vertices\n4.Enlazar nodos\n5.SALIR");
	  	printf("\nOPCION:");
	  	scanf("%d",&opc);
	  	system("cls");
	  	switch(opc)
	  	{
	        case 1: llenado();
				    break;
			case 2: libera_lista(&inicio);
					lee_archivo(&inicio); break;
			case 3: mostrar_lista(inicio); break;
			case 4: enlaza(&inicio); break;
		}
		system("pause");
		system("cls");	
    }while(opc!=5);
    return 0;
}
/*
void esfera(float x, float y)
{
	//glColor3f(0.5f,0.5f,1.0f);
	glPushMatrix();
	glTranslatef(x,y,0.0);
	glColor3d(1.0, 0.0, 0.0);
	glutSolidSphere(15,50,50);
	glPopMatrix();
}
*/

void dibuja_cadena(char *cadena,float x, float y){
	unsigned int i;
	glColor3f(1,1,1);
	for (i=0; i<strlen(cadena); i++){
		glRasterPos3f (x,y,0);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, cadena[i]);	    
		x=x+8;	
	}
}

int crea_enlace(struct arista *conecta,struct vertice *conecta2){
	GLfloat ancho = 5.0f;  //variable que contiene el ancho de la linea
	glLineWidth(ancho); 
	glPushMatrix();
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_LINES);
	glVertex3f(conecta->cor_x,conecta->cor_y,0);  /* V0*/
	glVertex3f(conecta2->cor_x,conecta2->cor_y,0);  /* V1*/
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
	int cont=0,cont2=0;
	cuenta_nodos(inicio,cont);
	struct vertice *aux=inicio;
	printf("\nCantidad de vertices: %d",cont);
	for (int i = 0; i <cont; i++){
		//esfera(aux->cor_x,aux->cor_y);
		dibujarEsfera(aux->cor_x,aux->cor_y);
		dibuja_cadena(aux->nodo,aux->cor_x-10,aux->cor_y+10);
		cont2=cuenta_conexiones(aux->conexion,0);  /*Contamos las aristas de mi vertices*/
		/*printf("\nCantidad de conexiones: %d\n",cont2);*/
		struct arista *conecta=aux->conexion;
		for (int i = 0; i < cont2; i++){
			crea_enlace(conecta,aux);	
			conecta=conecta->siguiente;
		}
		aux=aux->siguiente;
	}
	glFlush();
	glutSwapBuffers();

}

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
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (1000,600);
	glutInitWindowPosition (180,80);
	glutCreateWindow ("Proyecto");
	glClearColor(0,0,0,0);
	dibuja_nodo();
	glutDisplayFunc(dibuja_nodo);
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
-Falta enlazar los vertices   (Ya quedÃƒÂ³)
	-IDEAS GENERADAS
		-Ver primero como se hace linea  (Ya quedÃƒÂ³)
			-Copiar la funcion que ya tenemos

		-En el for , contar los nodos que tiene  (Ya quedÃƒÂ³)
		 conectados en ese momento el vertice.

		-Despues dentro de ese for  (Ya quedÃƒÂ³)
		 crear otro for, que llamara a su vez crea_linea,como parametro,
		 tendra las corrdenas origen y destino, o los apuntadores.
		 
-Insertar el nombre de los nodos   (ya quedÃƒÂ³)
-Falta la funcion del mause, al dar click derecho, crear nodo
-Tener en el meni de mause una opcion de enlazar nodos
-Insertar algoritmo de Dijstra
*/