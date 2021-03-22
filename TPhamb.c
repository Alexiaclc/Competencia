# include  <stdio.h>       // libreria estandar
# include  <stdlib.h>      // para usar exit y funciones de la libreria standard
# include  <string.h>
# include  <pthread.h>     // para usar hilos
# include  <semaphore.h>   // para usar semaforos
# include  <unistd.h>
# include  <limits.h>


# define  LIMITE  50

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// creo estructura de semaforos
struct semaforos {
	sem_t sem_cortar;
    sem_t sem_mezclar;
    sem_t sem_hornear;
    sem_t sem_salar;
    sem_t sem_armarmedallon;
    sem_t sem_cocinarmed;
    sem_t sem_cortarlechytom;
    sem_t sem_armarhambu;    
};

// creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes [ 4 ] [LIMITE];  
};

// creo los parametros de los hilos
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];     //creo array de pasos
};

// funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion ( void *data, char *accionIn) {
	struct parametro *mydata = data;
	// calculo la longitud del array de pasos
	int sizeArray = (int)(sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	// indice para recorrer array de pasos
	int i;
	for(i = 0; i <sizeArray; i++){
		// pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0 ){
		printf("\tEquipo %d - accion %s  \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//para imprimir dentro del archivo "salida"
		fprintf(salida, "\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		// calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]));
		// indice para recorrer array de ingredientes
		int h;
		printf ("\tEquipo %d ----------- ingredientes: ---------- \n " , mydata->equipo_param);
		fprintf(salida, "\tEquipo %d ----------- ingredientes: ---------- \n " , mydata->equipo_param);
		for(h = 0; h <sizeArrayIngredientes; h++){
			// consulto si la posicion tiene valor porque no hay cuantos ingredientes tengo por accion
			if(strlen(mydata->pasos_param[i].ingredientes[h])!= 0){
				printf("\tEquipo %d - ingredientes %d: %s \n" , mydata->equipo_param , h, mydata->pasos_param[i].ingredientes[h]);
				fprintf(salida, "\tEquipo %d - ingredientes %d: %s \n" , mydata->equipo_param , h, mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
}


void* cortar(void *data){
	// creo el nombre de la accion de la funcion
	char *accion = "cortar" ;
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	// uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	// doy la seÃ±al a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
	
    pthread_exit(NULL);
}

void* mezclar(void *data){
	char *accion = "mezclar" ;
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep( 20000 );
	// mezclar habilita salar
    sem_post(&mydata->semaforos_param.sem_salar);
	
    pthread_exit(NULL);
}

void* salar(void *data){
	//bloqueo el mutex para que nadie más lo use
	pthread_mutex_lock(&mutex);

	char *accion = "salar";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);

	//desbloqueo el mutex para que pueda ser utilizado
	pthread_mutex_unlock(&mutex);

	// salar habilita armar el medallon
    sem_post(&mydata->semaforos_param.sem_armarmedallon);

    pthread_exit(NULL);
}

void* armarmedallon(void *data){
	char *accion = "armar medallon";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);
	// armar el medallon habilita cocinarlo
    sem_post(&mydata->semaforos_param.sem_cocinarmed);
	
    pthread_exit(NULL);
}

void* cocinar(void *data){
	pthread_mutex_lock(&mutex);

	char *accion = "cocinar";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);
	// cocinar habilita armar hamburguesa
    sem_post(&mydata->semaforos_param.sem_armarhambu);

    pthread_mutex_unlock(&mutex);
	
    pthread_exit(NULL);
}

void* armarhambu(void *data){
	char *accion = "armar hamburguesa";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);
	// cocinar habilita armar hamburguesa
    sem_post(&mydata->semaforos_param.sem_armarhambu);
	
    pthread_exit(NULL);
}

void* hornear(void *data){
	pthread_mutex_lock(&mutex);

	char *accion = "hornear";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);

	// cocinar habilita armar hamburguesa
    sem_post(&mydata->semaforos_param.sem_armarhambu);
	
	pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void* cortarlechytom(void *data){
	char *accion = "cortar lechuga y tomate";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep(20000);

	// cocinar habilita armar hamburguesa
    sem_post(&mydata->semaforos_param.sem_armarhambu);
	
    pthread_exit(NULL);
}



void* ejecutarReceta(void *i){
	
	// variables semaforos
	sem_t sem_cortar;
	sem_t sem_mezclar;
	sem_t sem_hornear;
    sem_t sem_salar;
    sem_t sem_armarmedallon;
    sem_t sem_cocinarmed;
    sem_t sem_cortarlechytom;
    sem_t sem_armarhambu;

	
	// variables hilos
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;

	
	// numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	// reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	// seteo numero de grupo
	pthread_data->equipo_param = p;

	// seteo semaforos
	pthread_data->semaforos_param.sem_cortar = sem_cortar;
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_hornear = sem_hornear;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_armarmedallon = sem_armarmedallon;
	pthread_data->semaforos_param.sem_cocinarmed = sem_cocinarmed;
	pthread_data->semaforos_param.sem_cortarlechytom = sem_cortarlechytom;
	pthread_data->semaforos_param.sem_armarhambu = sem_armarhambu;
	// setear demas semaforos al struct aqui
	

	// seteo las acciones y los ingredientes que son levantados del archivo
	  for(i=0; !feof(file); i++){
    	

    	if(i==0){    //para que lea lo que está en la primera línea
    		char aux='0';
    		int j;
    		for(j=0; aux != '-'; j++){
				aux = fgetc(file);
				if(aux != '-'){
		    	    pasos_param[0].accion = aux;      //guarda la primera acción
    	    	}
    	    }
    	    for(j=0; aux != '-'; j++){
    		    aux = fgetc(file);
    		    if(aux != '-'){
    			    pasos_param[0].ingredientes[0] = aux;  //primer ingrediente
    		    }
    	    }
    	    for(j=0; aux != '-'; j++){
    		    aux = fgetc(file);
    		    if(aux != '-'){
    			    pasos_param[0].ingredientes[1] = aux;
    		    }
    	    }
    	    for(j=0; aux != '-'; j++){
    		    aux = fgetc(file);
    		    if(aux != '-'){
    			    pasos_param[0].ingredientes[2] = aux;
    		    }
    	    }
    	}
    	else{
			char aux='0';
			int j;
    		for(j=0; aux != '\n'; j++){
    		    aux = fgetc(file);
    		    if(aux != '-'){
    			    pasos_param[i].accion = aux;     //guarda segunda acción en la posición i (el resto son solo acciones,
    		    }                                    //no más ingredientes)
    	    }
    	}
    	
/*     	strcpy (pthread_data-> pasos_param [ 0 ]. accion , " cortar " );
	strcpy (pthread_data-> pasos_param [ 0 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 0 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 0 ]. ingredientes [ 2 ], " cebolla " );

	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " mezclar " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 2 ], " cebolla " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 3 ], " carne picada " );

	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " salar " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 2 ], " cebolla " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 3 ], " carne picada " );
	
	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " armar medallon " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 2 ], " cebolla " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 3 ], " carne picada " );

	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " cocinar " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 2 ], " cebolla " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 3 ], " carne picada " );

	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " hornear " );

	strcpy (pthread_data-> pasos_param [ 1 ]. accion , " armar hamburguesa " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 0 ], " ajo " );
        strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 1 ], " perejil " );
 	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 2 ], " cebolla " );
	strcpy (pthread_data-> pasos_param [ 1 ]. ingredientes [ 3 ], " carne picada " );*/

	// inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_cortar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarmedallon),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinarmed),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cortarlechytom),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarhambu),0,0);


	// creo los hilos a todos les paso el struct creado 
    int rc;

    rc = pthread_create(&p1,                        
                            NULL,                    
                                cortar,              
                                pthread_data);       

    rc = pthread_create(&p2,                            
                            NULL,                           
                                mezclar,              
                                pthread_data);        

    rc = pthread_create(&p3,                       
                            NULL,                   
                                salar,              
                                pthread_data);      

    rc = pthread_create(&p4,                     
                            NULL,                  
                                armarmedallon,      
                                pthread_data);

    rc = pthread_create(&p5,                      
                            NULL,                  
                                cocinar,            
                                pthread_data);

    rc = pthread_create(&p6,                      
                            NULL,                           
                                hornear,              
                                pthread_data);

    rc = pthread_create(&p7,                        
                            NULL,                    
                                cortarlechytom,   
                                pthread_data);

    rc = pthread_create(&p8,                        
                            NULL,                    
                                armarhambu,           
                                pthread_data);
	
	
	
	// join de todos los hilos
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	pthread_join(p3,NULL);
	pthread_join(p4,NULL);
	pthread_join(p5,NULL);
	pthread_join(p6,NULL);
	pthread_join(p7,NULL);
	pthread_join(p8,NULL);
	


	// valido que el hilo se alla creado bien
    if(rc){
       printf("Error: no se puede crear el hilo, %d  \n" , rc);
       exit(-1);
     }

	 
	// destruccion de los semaforos
	sem_destroy(&sem_cortar);
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armarmedallon);
	sem_destroy(&sem_cocinarmed);
	sem_destroy(&sem_hornear);
	sem_destroy(&sem_cortarlechytom);
	sem_destroy(&sem_armarhambu);

	
	// salida del hilo
	 pthread_exit(NULL);
}


int  main()
{
	
	
	
    
    //para levantar del archivo  
	
	FILE *file;
	file = fopen("/home/alexita/Documentos/receta.odt", "r"); 

	if(file == NULL){
		printf("No se puede abrir la receta.\n");
		exit(1);
	}
    char line[256];
    int cont = 0; 
    int i, j;
    while (!feof(file)){
    	fgets(line, sizeof(line), file);
    	cont++;
    }

        //para guardar la salida en un archivo
    FILE *salida;
	salida = fopen("/home/alexita/Documentos/salida de competencia.odt", "w+");
	
	
	// creo los nombres de los equipos
	int rc;
	int *equipoNombre1 = malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 = malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 = malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	// creo las variables los hilos de los equipos
	pthread_t equipo1;
	pthread_t equipo2;
	pthread_t equipo3;




    
 
	// inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                            
                            NULL,                           
                                ejecutarReceta,              
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                          
                            NULL,                           
                                ejecutarReceta,              
                                equipoNombre2);

    rc = pthread_create(&equipo3,                          
                            NULL,                           
                                ejecutarReceta,              
                                equipoNombre3);

   if(rc){
       printf(" Error: no se puede crear el hilo, %d \n", rc);
       exit(-1);
   } 

	   // join de todos los hilos
	   pthread_join (equipo1,NULL);
	   pthread_join (equipo2,NULL);
	   pthread_join (equipo3,NULL);

    fclose(file);
	fclose(salida);

    pthread_exit(NULL);
}


// Para compilar: gcc TPhamb.c -o ejecutable -lpthread
// Para ejecutar: ./ejecutable

