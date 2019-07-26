#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pila.h"
#include "abb.h"

typedef struct abb_nodo{
	char* clave;
	void* dato;
	struct abb_nodo* izq;
	struct abb_nodo* der;
}abb_nodo_t;

struct abb {
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
};

struct abb_iter {
	pila_t* pila;
};

abb_nodo_t* abb_nodo_crear(const char *clave, void *dato){
	abb_nodo_t* abb_nodo = malloc(sizeof(abb_nodo_t));
	if(!abb_nodo) return NULL;
	char* clave_aux = strdup(clave);
	abb_nodo->clave = clave_aux;
	abb_nodo->dato = dato;
	abb_nodo->izq = NULL;
	abb_nodo->der = NULL;
	return abb_nodo;
}

void destruir_nodo(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	free(nodo->clave);
	if(destruir_dato) destruir_dato(nodo->dato);
}

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	if(!abb) return NULL;
	abb->raiz = NULL;
	abb->cmp = cmp;
	abb->destruir_dato = destruir_dato;
	abb->cantidad = 0;
	return abb;
}

abb_nodo_t* abb_buscar_padre(abb_nodo_t* nodo,const char* clave,abb_comparar_clave_t cmp){
	int comparar = cmp(clave,nodo->clave);
	if(comparar == 0) return nodo;
	if(comparar < 0) {
		if(nodo->izq) {
			if(cmp(clave,nodo->izq->clave) == 0) return nodo;
			return abb_buscar_padre(nodo->izq,clave,cmp);
		}
		return nodo;
	}
	else{
		if(nodo->der) {
			if(cmp(clave,nodo->der->clave) == 0) return nodo;
			return abb_buscar_padre(nodo->der,clave,cmp);
		}
		return nodo;
	}
}

abb_nodo_t* abb_buscar_nodo(abb_nodo_t* nodo,const char* clave,abb_comparar_clave_t cmp){
	if(!nodo) return NULL;
	int comparar = cmp(clave,nodo->clave);
	if(comparar == 0) return nodo;
	if(comparar < 0) return abb_buscar_nodo(nodo->izq,clave,cmp);
	return abb_buscar_nodo(nodo->der,clave,cmp);
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_buscar_nodo(arbol->raiz,clave,arbol->cmp);
	if(!nodo) return false;
	return true;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	abb_nodo_t* aux = abb_nodo_crear(clave,dato);
	if(!aux) return false;
	if(arbol->cantidad == 0) {
		arbol->raiz = aux;
		arbol->cantidad++;
	}
	else {
		abb_nodo_t* nodo = abb_buscar_nodo(arbol->raiz,clave,arbol->cmp);
		if(!nodo){
			abb_nodo_t* padre = abb_buscar_padre(arbol->raiz,clave,arbol->cmp);
			int comparar = arbol->cmp(clave,padre->clave);
			if(comparar < 0) padre->izq = aux;
			else padre->der = aux;
			arbol->cantidad++;
		}
		else{
			free(nodo->clave);
			if(arbol->destruir_dato) arbol->destruir_dato(nodo->dato);
			nodo->clave = strdup(clave);
			nodo->dato = dato;
			free(aux->clave);
			free(aux);
		}	
	}
	return true;
}

void* borrar_hoja(abb_nodo_t* padre,abb_nodo_t* borrar,abb_t* arbol){
	void* aux = borrar->dato;
	int comparar = arbol->cmp(borrar->clave,padre->clave);
	destruir_nodo(borrar,NULL);
	if(comparar == 0) arbol->raiz = NULL;
	if(comparar < 0) padre->izq = NULL;
	else padre->der = NULL;
	free(borrar);	
	arbol->cantidad--;
	return aux;
}

void swap(abb_nodo_t* borrar, abb_nodo_t* borrar_aux){
	char* clave_aux = borrar->clave;
	void* valor_aux = borrar->dato;
	borrar->clave = borrar_aux->clave;
	borrar->dato = borrar_aux->dato;
	borrar_aux->clave = clave_aux;
	borrar_aux->dato = valor_aux;
}

abb_nodo_t* buscar_remplazante(abb_nodo_t* nodo){ 
	if(!nodo->izq) return nodo;
	return buscar_remplazante(nodo->izq);
}

void* borrar_nodo_con_un_hijo(abb_nodo_t* padre,abb_nodo_t* borrar,abb_t* arbol){
	void* dato = borrar->dato;
	abb_nodo_t* borrar_aux;
	if( borrar->izq) borrar_aux = borrar->izq;
	else borrar_aux = borrar->der;
	int comparar = arbol->cmp(borrar->clave,padre->clave);
	if(comparar == 0) arbol->raiz = borrar_aux;
	if(comparar < 0) padre->izq = borrar_aux;
	else  padre->der = borrar_aux;
	destruir_nodo(borrar,NULL);
	free(borrar);
	arbol->cantidad--;
	return dato;
}

void* borrar_nodo_con_2_hijos(abb_nodo_t* borrar,abb_t* arbol){
	abb_nodo_t* aux_borrar = buscar_remplazante(borrar->der);
	abb_nodo_t* padre = abb_buscar_padre(arbol->raiz,aux_borrar->clave,arbol->cmp);
	swap(borrar,aux_borrar);

	if(arbol->cmp(borrar->der->clave,aux_borrar->clave) == 0){
		if(aux_borrar->der) borrar->der = aux_borrar->der;
		else borrar->der = NULL;
		void* aux = aux_borrar->dato;
		destruir_nodo(aux_borrar,NULL);
		free(aux_borrar);
		arbol->cantidad--;
		return aux;
	}
	else{
		if(!aux_borrar->der && !aux_borrar->izq) return borrar_hoja(padre,aux_borrar,arbol);
		return borrar_nodo_con_un_hijo(padre,aux_borrar,arbol);
	}
	
}

void* abb_metodos_borrar(abb_t* arbol,abb_nodo_t* nodo_a_borrar){
	abb_nodo_t* padre = abb_buscar_padre(arbol->raiz,nodo_a_borrar->clave,arbol->cmp);
	if(!nodo_a_borrar->izq && !nodo_a_borrar->der) return borrar_hoja(padre,nodo_a_borrar,arbol);
	if(!nodo_a_borrar->izq || !nodo_a_borrar->der) return borrar_nodo_con_un_hijo(padre,nodo_a_borrar,arbol);
	return borrar_nodo_con_2_hijos(nodo_a_borrar,arbol);
}

void *abb_borrar(abb_t *arbol, const char *clave){
	if(abb_cantidad(arbol) == 0) return NULL;
	abb_nodo_t* nodo_a_borrar = abb_buscar_nodo(arbol->raiz,clave,arbol->cmp);
	if(!nodo_a_borrar) return NULL;
	return abb_metodos_borrar(arbol,nodo_a_borrar);
	
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_buscar_nodo(arbol->raiz,clave,arbol->cmp);
	if(!nodo) return NULL;
	return nodo->dato;
}

void  destruir_posorden(abb_nodo_t* nodo, abb_t* arbol){
	if(!nodo) return ;
	destruir_posorden(nodo->izq,arbol);
	destruir_posorden(nodo->der,arbol);
	abb_nodo_t* padre = abb_buscar_padre(arbol->raiz,nodo->clave,arbol->cmp);
	void* aux = borrar_hoja(padre,nodo,arbol);
	if(arbol->destruir_dato) arbol->destruir_dato(aux);
}

void abb_destruir(abb_t *arbol){
	destruir_posorden(arbol->raiz,arbol);
	free(arbol);
}

void abb_iter_apilar_nodo_y_rama_izq(pila_t* pila, abb_nodo_t* nodo_desde){
   	if(nodo_desde){
    	pila_apilar(pila, nodo_desde);
    	abb_iter_apilar_nodo_y_rama_izq(pila,nodo_desde->izq);
   	}
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {

    pila_t* pila = pila_crear();
    if (!pila) return;
   
    abb_iter_apilar_nodo_y_rama_izq(pila, arbol->raiz);
    abb_nodo_t* actual = pila_desapilar(pila);

    while (actual && visitar(actual->clave, actual->dato, extra)) {
        abb_iter_apilar_nodo_y_rama_izq(pila, actual->der);
        actual = pila_desapilar(pila);
    }
    pila_destruir(pila);
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
  	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
  	if(!iter) return NULL;
  	pila_t* pila_abb = pila_crear();
  	if(!pila_abb){
   	 	free(iter);
    	return NULL;
  	}
  	abb_iter_apilar_nodo_y_rama_izq(pila_abb, arbol->raiz);
  	iter->pila = pila_abb;
  	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
  if(abb_iter_in_al_final(iter)) return false;

  	abb_nodo_t* abb_nodo_desapilado = pila_desapilar(iter->pila);
  	abb_iter_apilar_nodo_y_rama_izq(iter->pila, abb_nodo_desapilado->der);
	return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	
	if(abb_iter_in_al_final(iter)) return NULL;
	 abb_nodo_t* nodo = pila_ver_tope(iter->pila);
	 return nodo->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}






