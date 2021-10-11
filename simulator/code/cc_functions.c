#include "stdafx.h"
#include <stdlib.h>
#include "generale.h"

// puntatore al primo puntatore della lista delle transazioni
extern cc_transaction_descriptor **cc_tx_list; //da inizializzare dal modulo che usa questo modulo 
// puntatore al primo puntatore della lista dei data item committati

// inserisce una transazione alla fine della lista
void cc_enqueue_transaction(int tx_id,int tx_run_number, double start_timestamp) {
	// crea la nuova tx
	cc_transaction_descriptor *tx_descr=(cc_transaction_descriptor *)malloc(sizeof(cc_transaction_descriptor));
	tx_descr->tx_id=tx_id;
	tx_descr->tx_run_number=tx_run_number;
	tx_descr->start_timestamp=start_timestamp;
	tx_descr->cc_first_tx_write_set_item=NULL;
	tx_descr->cc_first_tx_read_set_item=NULL;
	tx_descr->next=NULL;
	// scorre la lista fino alla prima tx con next nullo
  	if((*cc_tx_list)==NULL){
        	*cc_tx_list=tx_descr;
	} else {
		cc_transaction_descriptor *tx_list_temp=*cc_tx_list;
      		while(tx_list_temp->next) {
			tx_list_temp=tx_list_temp->next;	
		}
		tx_list_temp->next=tx_descr;
	}

}

//restituisce la transazione tx_id 
cc_transaction_descriptor *cc_get_transaction(int tx_id) {
	//scorre la lista delle tx fino a quando non trova l'id della tx
	if(*cc_tx_list==NULL){
        	return NULL;
	} else {
		cc_transaction_descriptor *tx_temp=*cc_tx_list;
      		while(tx_temp->tx_id!=tx_id) {
			tx_temp=tx_temp->next;
			if (tx_temp==NULL)
		        	return NULL;	
		}
		return tx_temp;
	}
}

//restituisce il cc_tx_write_set_item se di_id è presente nel write set della tx
cc_tx_write_set_item *get_write_set_item_by_id(cc_transaction_descriptor* cc_tx_descr, int di_id) {
	
	//scorre il wite set fino a quando non trova l'id del dataitems
	if(cc_tx_descr->cc_first_tx_write_set_item==NULL){
        	return NULL;
	} else {
		cc_tx_write_set_item *item_temp=cc_tx_descr->cc_first_tx_write_set_item;
		while(item_temp != NULL) {
			if(item_temp->di_id == di_id)
				return item_temp;
			item_temp=item_temp->next;	
		}
		return NULL;
	}
}
//restituisce il cc_tx_read_set_item se di_id è presente nel read set della tx
cc_tx_read_set_item *get_read_set_item_by_id(cc_transaction_descriptor* cc_tx_descr, int di_id) {
	
	//scorre il wite set fino a quando non trova l'id del dataitems
	if(cc_tx_descr->cc_first_tx_read_set_item==NULL){
        	return NULL;
	} else {
		cc_tx_read_set_item *item_temp=cc_tx_descr->cc_first_tx_read_set_item;
		while(item_temp != NULL) {
			if(item_temp->di_id == di_id)
				return item_temp;
			item_temp=item_temp->next;	
		}
		return NULL;
	}
}
// svuota read e write set
void remove_read_and_write_set(cc_transaction_descriptor *cc_tx_descr){
	// read set
	cc_tx_read_set_item *next_read_set_item;
	cc_tx_read_set_item *read_set_item;
	read_set_item=cc_tx_descr->cc_first_tx_read_set_item;
	while(!read_set_item==0){
		next_read_set_item=read_set_item->next;
		free(read_set_item);
		read_set_item=next_read_set_item;
	}	

	// write set
	cc_tx_write_set_item *next_write_set_item;
	cc_tx_write_set_item *write_set_item;
	write_set_item=cc_tx_descr->cc_first_tx_write_set_item;
	while(!write_set_item==0){
		next_write_set_item=write_set_item->next;
		free(write_set_item);
		write_set_item=next_write_set_item;
	}
}
// elimina la transazione tx_id dalla lista con tutti i dataitem aggiornati
void cc_remove_transaction(int tx_id) {
	if((*cc_tx_list)->tx_id==tx_id){
		cc_transaction_descriptor *to_delete=*cc_tx_list;
		*cc_tx_list=(*cc_tx_list)->next;
		// svuota read e write set
		remove_read_and_write_set(to_delete);
		// libera la memoria della transazione
		free(to_delete);
	} else {
		cc_transaction_descriptor *tx_temp=*cc_tx_list;
    	while(tx_temp->next->tx_id!=tx_id) {
			tx_temp=tx_temp->next;
		}
		cc_transaction_descriptor *to_delete=tx_temp->next;
		tx_temp->next=tx_temp->next->next;
		// svuota read e write set
		remove_read_and_write_set(to_delete);
		// libera la memoria della transazione
		free(to_delete);
	}
}


// inserisce un dataitem al write set della tx
void cc_add_write_set_item(int tx_id, int di, int run_op_number) {
	// prende la tx
    cc_tx_write_set_item *write_set_item;
	cc_transaction_descriptor *tx_temp=cc_get_transaction(tx_id);
	// scorre la lista
  	if(tx_temp->cc_first_tx_write_set_item==NULL){
		// crea il nuovo item
        write_set_item=(cc_tx_write_set_item *)malloc(sizeof(cc_tx_write_set_item));
		write_set_item->di_id=di;
		write_set_item->run_op_number=run_op_number;
		write_set_item->next=NULL; 
		// lo aggiunge
        tx_temp->cc_first_tx_write_set_item=write_set_item;
	} else {
		if ( tx_temp->cc_first_tx_write_set_item->di_id==di) 
			return;  // il dataitem è già presente
		cc_tx_write_set_item *wsi_temp=tx_temp->cc_first_tx_write_set_item;
      	while(wsi_temp->next) {
			wsi_temp=wsi_temp->next;	
			if (wsi_temp->di_id==di) {
				return;  // il dataitem è già presente
			}
		}
		// crea il nuovo dataitem
        write_set_item=(cc_tx_write_set_item *)malloc(sizeof(cc_tx_write_set_item));
		write_set_item->di_id=di;
		write_set_item->run_op_number=run_op_number;
		write_set_item->next=NULL;
		// lo aggiunge alla fine
		wsi_temp->next=write_set_item;
	}	
}

// inserisce un dataitem al read set della tx
void cc_add_read_set_item(int tx_id, int di, int run_op_number) {
	// prende la tx
    cc_tx_read_set_item *read_set_item;
	cc_transaction_descriptor *tx_temp=cc_get_transaction(tx_id);
	// scorre la lista
  	if(tx_temp->cc_first_tx_read_set_item==NULL){
		// crea il nuovo item
        read_set_item=(cc_tx_read_set_item *)malloc(sizeof(cc_tx_read_set_item));
		read_set_item->di_id=di;
		read_set_item->run_op_number=run_op_number;
		read_set_item->next=NULL;
		// lo aggiunge
        tx_temp->cc_first_tx_read_set_item=read_set_item;
	} else {
		if ( tx_temp->cc_first_tx_read_set_item->di_id==di) 
			return;  // il dataitem è già presente
		cc_tx_read_set_item *wsi_temp=tx_temp->cc_first_tx_read_set_item;
      		while(wsi_temp->next) {
			wsi_temp=wsi_temp->next;	
			if (wsi_temp->di_id==di) {
				return;  // il dataitem è già presente
			}
		}
		// crea il nuovo dataitem
        read_set_item=(cc_tx_read_set_item *)malloc(sizeof(cc_tx_read_set_item));
		read_set_item->di_id=di;
		read_set_item->run_op_number=run_op_number;
		read_set_item->next=NULL;
		// lo aggiunge alla fine
		wsi_temp->next=read_set_item;
	}	
}
// prende un item del write set della tx per posizione
cc_tx_write_set_item *cc_get_tx_write_set_item_by_position(int tx_id, int position) {
	// prende la tx
	cc_transaction_descriptor *tx_temp=cc_get_transaction(tx_id);
	// prende il primo item della tx
	cc_tx_write_set_item *first_item=tx_temp->cc_first_tx_write_set_item;
	//scorre la lista dei dataitems fino a quando non trova l'id del dataitems
	if(first_item==NULL){
        	return NULL;
	} else {
		int i=1;
		cc_tx_write_set_item *item_temp=first_item;
      		while(i<position) {
			item_temp=item_temp->next;	
			i++;
			if (item_temp==NULL)
		        	return NULL;
		}
			return item_temp;
	}
}

// prende un item del read set della tx per posizione
cc_tx_read_set_item *cc_get_tx_read_set_item_by_position(int tx_id, int position) {
	// prende la tx
	cc_transaction_descriptor *tx_temp=cc_get_transaction(tx_id);
	// prende il primo item della tx
	cc_tx_read_set_item *first_item=tx_temp->cc_first_tx_read_set_item;
	//scorre la lista dei dataitems fino a quando non trova l'id del dataitems
	if(first_item==NULL){
        	return NULL;
	} else {
		int i=1;
		cc_tx_read_set_item *item_temp=first_item;
      		while(i<position) {
			item_temp=item_temp->next;	
			i++;
			if (item_temp==NULL)
		        	return NULL;
		}
			return item_temp;
	}
}
