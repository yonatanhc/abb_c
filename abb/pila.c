#include "pila.h"
#include <stdlib.h>

#define CANTIDAD_INICIAL 10000
#define FACTOR_DISMINUIR 0.25
#define FACTOR_VARIACION 2

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...
pila_t* pila_crear(void) {

	pila_t* pila = malloc(sizeof(pila_t));

	if (pila == NULL) return NULL;		
	
	pila->datos = malloc( CANTIDAD_INICIAL * sizeof(void*));

	if (CANTIDAD_INICIAL > 0 && pila->datos == NULL){
		free(pila);
		return NULL;
	}

	pila->cantidad = 0;	
	pila->capacidad = CANTIDAD_INICIAL;

	return pila;
}

void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}


bool pila_esta_vacia(const pila_t *pila) {

	return pila->cantidad == 0;

}

bool redimensionar(pila_t* pila,size_t tamanio){
	void** aux = realloc(pila->datos,tamanio*sizeof(void*));
	if(aux == NULL) return false;
	
	pila->datos = aux;
	pila->capacidad = tamanio;
	return true;
}

bool pila_apilar(pila_t *pila, void* valor) {

	if ( pila->cantidad >= pila->capacidad)	{
		if (!redimensionar(pila,FACTOR_VARIACION * pila->capacidad)){
			return false;
		}
	}
	
	pila->cantidad ++;
	pila->datos[pila->cantidad - 1] = valor;

	return true;
}

void* pila_ver_tope(const pila_t *pila) {

	if (pila_esta_vacia(pila)) return NULL;
	
	return pila->datos[pila->cantidad - 1];

}

void* pila_desapilar(pila_t *pila) {
	if ( pila_esta_vacia(pila)) return NULL;
	
	if (pila->capacidad > CANTIDAD_INICIAL && pila->cantidad/pila->capacidad <= FACTOR_DISMINUIR){
		redimensionar(pila,pila->capacidad/FACTOR_VARIACION);
	}
	void* aux=pila->datos[pila->cantidad - 1];

	pila->cantidad --;

	return aux;
}
