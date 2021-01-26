#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "barnes.h"

#define G 0.0000000000667

double x_c = 0;
double y_c = 0;

double massa_cercata = -1;

/*
*	funzione ausiliaria che verrà usata quando è necessario creare un nuovo nodo
*/
nodo_t * nodo_vuoto(void)
{
	nodo_t * n = malloc(sizeof(nodo_t)); 	//tenta di allocare la memoria per il nodo
	if(n == NULL) 							// impossibile allocare la memoria
	{
		fprintf(stderr,"fatal error: impossibile allocare la memoria\n");
		return NULL;
	}
	n->x = 0.0;
	n->y = 0.0;
	n->massa = 0.0;
	n->NE = NULL;
	n->NW = NULL;
	n->SW = NULL;
	n->SE = NULL;
	return n;
}

int stringa_a_corpo(const char* s, double* x, double* y, double* m)
{
	int tmp;
	double a, b, c;
	tmp = sscanf(s, "%lf%lf%lf", &a, &b, &c);	//non ci vanno gli spazi tra i "%lf"?
	
	if( tmp == 3 )
	{
		*x = a;
		*y = b;
		*m = c;
		return 0;
	}
	else //istruzioni eseguite se la stringa non è riconosciuta con il corretto formato  
	{
		fprintf(stderr,"formato stringa non corretto\n");
		return -1;
	}
}

nodo_t* inserisci(double m, double x, double y, nodo_t * root)
{
	/*
	*	questo controllo effettivamente è un po' strano ma direi di lasciarlo comunque perchè penso che serva
	*	 CONTROLLA LA CONDIZIONE DELL'IF CHE HO CAMBIATO!!
	*	NON CONTROLLA CHE SIA MASSIMO CON _s == RADIUS, MA SEPLICEMENTE CHE OGNI VOLTA SIA DENTRO
	*/
	//per prima cosa controlliamo che il corpo sia all'interno del nostro quadrante
	if (  x > x_c + _s || y > y_c + _s || x < x_c + -_s || y < y_c - _s  )
	{
		fprintf(stderr,"le coordinate della massa non sono interne alla galassia\n");
		return root;
		/*
		*	root va benissimo perchè non cambiamo nulla a root e torna quello che era senza alcuna modifica
		*/
	}

	if(root == NULL) // nodo riconosciuto come NULL
	{
		// crea un nodo vuoto e lo riempie con l'elemento che bisogna inserire
		root = nodo_vuoto();
		root->x = x;
		root->y = y;
		root->massa = m;
		
		return root;	
	}
	else if(root->massa == 0.0) // nodo vuoto
	{
		// lo riempie con le coordinate desiderate
		root->x = x;
		root->y = y;
		root->massa = m;
		
		return root;
	}
	else if( (root->NW) == NULL && (root->NE)== NULL && (root->SE)== NULL && (root->SW)== NULL) // foglia
	{
		/*
		*	Per gestire il caso in cui 
		*/
		nodo_t * prev_el = nodo_vuoto();
		
		//la massa nella foglia viene spostata nel sotto-albero
		prev_el->x = root->x;
		prev_el->y = root->y;
		prev_el->massa = root->massa;
		
		//aggiorno il centro di massa di root inserendo la nuova massa
		root->x = (root->massa * root->x + m * x)/(root->massa + m);
		root->y = (root->massa * root->y + m * y)/(root->massa + m);
		root->massa = root->massa + m;
		
		//posiziona la vecchia massa nei sottolberi
		if(prev_el->x > x_c && prev_el->y >= y_c) //NE
		{
			// vecchia massa spostata nel sotto-albero NE
			root->NE = prev_el;
			root->NW = nodo_vuoto();
			root->SW = nodo_vuoto();
			root->SE = nodo_vuoto();
		}
		else if(prev_el->x > x_c && prev_el->y < y_c) //SE
		{
			// vecchia massa spostata nel sotto-albero SE
			root->SE = prev_el;
			root->NW = nodo_vuoto();
			root->SW = nodo_vuoto();
			root->NE = nodo_vuoto();
		}
		else if(prev_el->x <= x_c && prev_el->y >= y_c)  //NW
		{
			// vecchia massa spostata nel sotto-albero NW
			root->NW = prev_el;
			root->NE = nodo_vuoto();
			root->SW = nodo_vuoto();
			root->SE = nodo_vuoto();
		}
		else  //SW
		{
			// vecchia massa spostata nel sotto-albero SW
			root->SW = prev_el;
			root->NW = nodo_vuoto();
			root->NE = nodo_vuoto();
			root->SE = nodo_vuoto();
		}
		
		//posiziona la nuova massa chiamando ricorsivamente inserisci
		if(x > x_c && y >= y_c) //NE
		{
			//la nuova massa appartiene al sotto-albero NE
			//ridimensiona il lato e trova il centro del sottoquadrante
			_s /= 2;
			x_c += _s;
			y_c += _s;
			root->NE = inserisci(m, x, y, root->NE); //ripete ricorsivamente
			//riporta il lato e il centro alla situazione precedente
			x_c -= _s;
			y_c -= _s;
			_s *= 2;
			return root;
		}
		else if(x > x_c && y < y_c) //SE
		{
			//la nuova massa appartiene al sotto-albero SE
			_s /= 2;
			x_c += _s;
			y_c -= _s;
			root->SE = inserisci(m, x, y, root->SE);
			x_c -= _s;
			y_c += _s;
			_s *= 2;
			return root;
		}
		else if(x <= x_c && y >= y_c) //NW
		{
			//la nuova massa appartiene al sotto-albero NW
			_s /= 2;
			x_c -= _s;
			y_c += _s;
			root->NW = inserisci(m, x, y, root->NW);
			x_c += _s;
			y_c -= _s;
			_s *= 2;
			return root;
		}
		else  //SW
		{
			//la nuova massa appartiene al sotto-albero SW
			_s /= 2;
			x_c -= _s;
			y_c -= _s;
			root->SW = inserisci(m, x, y, root->SW);
			x_c += _s;
			y_c += _s;
			_s *= 2;
			return root;
		}
	}
	else  //root è stato riconosciuto come un nodo interno
	{
		//ricalcola il centro di massa
		root->x = (root->massa * root->x + m * x)/(root->massa + m);
		root->y = (root->massa * root->y + m * y)/(root->massa + m);
		root->massa = root->massa + m;
		
		if(x > x_c && y >= y_c) //NE
		{
			//ripete l'inserimento nel sotto-albero NE
			_s /= 2;
			x_c += _s;
			y_c += _s;
			root->NE = inserisci(m, x, y, root->NE);
			x_c -= _s;
			y_c -= _s;
			_s *= 2;
			return root;
		}
		else if(x > x_c && y < y_c) //SE
		{
			//ripete l'inserimento nel sotto-albero SE
			_s /= 2;
			x_c += _s;
			y_c -= _s;
			root->SE = inserisci(m, x, y, root->SE);
			x_c -= _s;
			y_c += _s;
			_s *= 2;
			return root;
		}
		else if(x <= x_c && y >= y_c) //NW
		{
			//ripete l'inserimento nel sotto-albero NW
			_s /= 2;
			x_c -= _s;
			y_c += _s;
			root->NW = inserisci(m, x, y, root->NW);
			x_c += _s;
			y_c -= _s;
			_s *= 2;
			return root;
		}
		else  //SW
		{
			//ripete l'inserimento nel sotto-albero SW
			_s /= 2;
			x_c -= _s;
			y_c -= _s;
			root->SW = inserisci(m, x, y, root->SW);
			x_c += _s;
			y_c += _s;
			_s *= 2;
			return root;
		}
	}
}

void free_albero(nodo_t * root)
{
	//se non trova un puntatore nullo, ripete ricorsivamente la free dell'albero
	if(root != NULL){
	
		free_albero(root -> NE);
		free_albero(root -> NW);
		free_albero(root -> SE);
		free_albero(root -> SW);
		
		free(root);
	}
}

double massadi(double x, double y, nodo_t* root)
{

	double mass; //variabile che conterrà la massa ricercata
	
	if(root == NULL) //ricerca arrivata ad un nodo nullo
		return 0.0;
		
	else if( (root->NE) == NULL && (root->NW) == NULL && (root->SW) == NULL && (root->SE) == NULL) //nodo riconosciuto come foglia
	{
		if( root->x == x && root->y == y )
		{
			//la foglia cercata
			return root->massa;
		} 
		// altrimenti esce dalla funzione 
		return 0.0;
	}
	else if(x > x_c && y >= y_c) //NE
	{
		//adatto le dimensioni del lato e traslo il centro del quadrante, ripeto ricorsivamente
		_s /= 2;
		x_c += _s;
		y_c += _s;
		mass = massadi(x, y, root->NE);
		// riporto il lato e il centro nella situazione precedente
		x_c -= _s;
		y_c -= _s;
		_s *= 2;
	}
	else if(x <= x_c && y >= y_c) //NW
	{
		_s /= 2;
		x_c -= _s;
		y_c += _s;
		mass = massadi(x, y, root->NW);
		x_c += _s;
		y_c -= _s;
		_s *= 2;
	}
	else if(x > x_c && y < y_c) //SE
	{
		_s /= 2;
		x_c += _s;
		y_c -= _s;
		mass = massadi(x, y, root->SE);
		x_c -= _s;
		y_c += _s;
		_s *= 2;
	}
	else //SW
	{
		_s /= 2;
		x_c -= _s;
		y_c -= _s;
		mass = massadi(x, y, root->SW);
		x_c += _s;
		y_c += _s;
		_s *= 2;
	}
	return mass;
}

void stima_forza(double x, double y, double *fx, double* fy, double theta, nodo_t* root)
{
	if(massa_cercata == -1)//è la prima volta che viene chiamata stima_forza
	{
		
		if (root == NULL) //radice nulla => errore
		{
			fprintf(stderr,"root NULL.\n");
			return;
		}
		
		*fx = 0;
		*fy = 0;
		
		massa_cercata = massadi(x,y,root); //calcoliamo la massa del corpo su cui vogliamo calcolare la forza
		
		if( massa_cercata == 0 ) // massa non trovata
		{	
			massa_cercata = -1;
			fprintf(stderr,"massa non trovata.\n");
			return;
		}
		else if( (root->NE) == NULL && (root->NW) == NULL && (root->SW) == NULL && (root->SE) == NULL ) //root riconosciuto come foglia
		{
			//se alla prima chiamata, si ha una foglia, c'è un solo corpo, allora f_tot == 0
			massa_cercata=-1;
			return;
		}
		
		//adatto il lato e ripeto ricorsivamente
		_s /= 2;
		stima_forza(x, y, fx, fy, theta, root->NW);
		stima_forza(x, y, fx, fy, theta, root->SW);
		stima_forza(x, y, fx, fy, theta, root->SE);
		stima_forza(x, y, fx, fy, theta, root->NE); 
		//riporto tutto alle condizioni iniziali
		_s *= 2;
		
		massa_cercata = -1;
		return;
	}
	else //si entra nell'else dopo la prima chiamata
	{
		//se vede che root == NULL o il nodo è vuoto esce subito dalla funzione senza modificare la forza
		if(root == NULL || root->massa == 0.0)
			return;
			
		else if( (root->NE) == NULL && (root->NW) == NULL && (root->SW) == NULL && (root->SE) == NULL) //root riconosciuto come foglia
		{
			if(root->x == x && root->y == y && root->massa == massa_cercata) 	//è la foglia contenente il corpo su cui vogliamo calcolare la forza
				return;															//non agisce su se stessa
				
			else //è una foglia dove c'è un'altra massa
			{
				//calcola la distanza e la forza totale esercitata sul corpo
				double d = sqrt( pow((root->x - x), 2) + pow((root->y - y), 2) ); // distanza tra i due corpi
				
				double f_tot = (G * root->massa * massa_cercata) / (d*d);  // forza gravitazionale esercitata
				//calcola le componenti della forza e le somma a quelle precedentemente trovate
				*fx += f_tot * (root->x - x) / d;
				*fy += f_tot * (root->y - y) / d;
				return;
			}
		}
		else//root è un nodo interno
		{
			//calcola la distanza e controlla se è approssimabile
			double d = sqrt( pow((root->x - x), 2) + pow((root->y - y), 2) );
			
			if( (_s / d ) < theta ) //approssimabile al centro di massa
			{
				// calcolo della forza e scomposizione in componenti
				double f_tot = G * (root->massa * massa_cercata) / (d*d);
				
				*fx += f_tot * (root->x - x) / d;
				*fy += f_tot * (root->y - y) / d;
				return;
			}
			else //non approssimabile,ripeto ricorsivamente la funzione
			{
				_s /= 2;
				stima_forza(x, y, fx, fy, theta, root->NW);
				stima_forza(x, y, fx, fy, theta, root->SW);
				stima_forza(x, y, fx, fy, theta, root->SE);
				stima_forza(x, y, fx, fy, theta, root->NE);
				_s *= 2;
				return;
			}
		}
	}
}

