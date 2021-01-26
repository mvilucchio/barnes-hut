/* 
\file
  \authors informatica (CDS Fisica) 2017/18
  \brief intestazione delle funzioni da implementare
 */

/* protezione inclusioni multiple */
#ifndef __BARNES__H
#define __BARNES__H
#include <stdio.h>


/** variabile globale che contiene il raggio complessivo dell'area
rappresentata dall'albero */
double _s;

/** tipo del nodo del quad-tree */
typedef struct nodo {
  /** coordinate e massa del corpo (foglia) 
      o del centro di massa (nodo interno) */
  double x, y; 
  double massa;
  /* puntatori ai nodi figli */
  struct nodo * NW, *NE, *SE, *SW;
} nodo_t;

/** Estrae da una stringa di caratteri contenente la coordinata x 
   la coordinata y e la massa  di un corpo separate
    da spazi bianchi ' ', ad esempio 
         "400 -600 1980"
    i tre valori e li assegna alle variabili puntate da x, y e m 
    \param s stringa da convertire (non viene modificata)
    \param x,y,m puntatori alle variabili che conterranno 
                 le coordinate e la massa del corpo
    
    \retval 0 se la conversione \`e stata svolta corretamente
    \retval -1 se si e' verificato un errore 
               (in questo caso *x, *y, *m no nvengono modificati)

*/
int stringa_a_corpo (const char* s, double* x, double* y, double* m);

/** Inserisce un nuovo corpo di coordinate (x,y) e massa m nel quad-tree.
    \param x,y,m coordinate e massa del copro da inserire
    \param root puntatore alla radice del quad-tree

    \retval p puntatore alla radice del nuovo albero 
 */
nodo_t* inserisci (double m, double x, double y, nodo_t * root);

/** FORNITA DAI DOCENTI -- NON MODIFICARE
  Funzione che stampa il quad-tree su file
  \param f file (gia' aperto)
  \param root radice dell'albero da stampare
*/
void stampa_albero (FILE * f, int modo, nodo_t * root);

/** libera tutta la memoria occupata dall'albero
    \param root la radice del quad-tree da deallocare
 */
void free_albero (nodo_t * root);

/** trova la massa doi un copro di coordinate date
    \param x,y coordinate del corpo da ricercare 
    \param root radice del quad-tree

    \retval 0 se il corpo no n\`e presente
    \retval m massa del corpo di coordinate (x,y) se e' presente
*/
double massadi (double x, double y, nodo_t* root);

/** stima la forza 
    \param x,y coordinate del corpo 
    \param theta soglia
    \param fx, fy puntatori alle variabili 
           che conterranno la componente x ed y 
           della forza complessiva esercitata 
	   ( sono entrambe messe a 0 
	   se il corpo non viene trovato nell'albero )
    \param root radice dell'albero
*/
void stima_forza (double x, double y, double *fx, double* fy, double theta, nodo_t* root);
#endif

