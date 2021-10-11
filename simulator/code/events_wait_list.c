#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include "generale.h"

// copia l'evento nella lista di attesa
void copy_into_wait_list(int event_type, event_content_type *event_content, wait_event**locks_event_wait_list) {
	wait_event *event_item;
	event_content_type *new_event_content=(event_content_type*)malloc(sizeof(event_content_type));
	if (MALLOC_FAIL_VERBOSE && new_event_content==NULL) printf("-------- copy_into_wait_list: MALLOC FALLITA\n");
  	memcpy(new_event_content,event_content,sizeof(event_content_type));
    event_item=(wait_event*)malloc(sizeof(wait_event));
	if (MALLOC_FAIL_VERBOSE && event_item==NULL) printf("-------- copy_into_wait_list: MALLOC FALLITA\n");
	event_item->event_type=event_type;
	event_item->event_content=new_event_content;
	event_item->next=NULL;

  	if((*locks_event_wait_list)==NULL){
        	*locks_event_wait_list=event_item;

	} else {
		wait_event *event_temp=*locks_event_wait_list;
      		while(event_temp->next) {
			event_temp=event_temp->next;
		}
		event_temp->next=event_item;
	}
}

// prende il primo evento in attesa del detaitem di_id e lo elimina dalla lista, restituisce 1 se lo trova, 0 altrimenti
int get_first_waiting_event(int di_id, int *event_type, event_content_type **event_content, wait_event**locks_event_wait_list) {
   	//scorre la lista delgli eventi i attesa fino a quando non trova in evento in attesa dsu dataitem di_id
	if((*locks_event_wait_list)==NULL){
		*event_content=NULL;
		return 0;
	} else {
		if((*locks_event_wait_list)->event_content->di_id==di_id){
			wait_event *event_item=*locks_event_wait_list;
			*locks_event_wait_list=(*locks_event_wait_list)->next;
			// memorizza i dati dell'evento per restituirli
			*event_type=event_item->event_type;
			*event_content=event_item->event_content;
			// elimina l'evento
			free(event_item);
			return 1;
		} else {
			wait_event *event_temp=*locks_event_wait_list;
      		while(event_temp->next) {
				if(event_temp->next->event_content->di_id==di_id ){
					wait_event *event_item=event_temp->next;
					event_temp->next=event_temp->next->next;
					// memorizza i dati dell'evento per restituirli
					*event_type=event_item->event_type;
					*event_content=event_item->event_content;
					// elimina l'evento
					free(event_item);
					return 1;
				}
				event_temp=event_temp->next;
			}
			*event_type=0;
			*event_content=NULL;
			return 0;
		}
	}
}
// prende il tipo del primo evento in attesa del detaitem di_id, restituisce 0 se non lo trova, 1 se lo READ, 2 se WRITE
int get_type_first_waiting_event(int di_id,int table_id, wait_event**locks_event_wait_list) {
   	//scorre la lista delgli eventi i attesa fino a quando non trova in evento in attesa dsu dataitem di_id
	if((*locks_event_wait_list)==NULL){
		return 0;
	} else {
			wait_event *event_temp=*locks_event_wait_list;
      		while(event_temp) {
				if(event_temp->event_content->di_id==di_id){
					if(event_temp->event_content->op_type == TM_READ)
						return 1;
					if(event_temp->event_content->op_type == TM_WRITE)
						return 2;
				}
				event_temp=event_temp->next;
			}
			return 0;
	}
}

// prende il tx_id del primo evento in attesa del detaitem di_id, restituisce 0 se non lo trova
int get_tx_id_of_first_waiting_event(int di_id,int table_id, wait_event**locks_event_wait_list) {
   	//scorre la lista delgli eventi i attesa fino a quando non trova in evento in attesa dsu dataitem di_id
	if((*locks_event_wait_list)==NULL){
		return 0;
	} else {
			wait_event *event_temp=*locks_event_wait_list;
      		while(event_temp) {
				if(event_temp->event_content->di_id==di_id){
					return event_temp->event_content->tx_id;
				}
				event_temp=event_temp->next;
			}
			return 0;
	}
}
// prende il primo evento di read in attesa del detaitem di_id e lo elimina dalla lista, restituisce 1 se lo trova, 0 altrimenti
int get_first_read_waiting_event(int di_id,int table_id, int *event_type, event_content_type **event_content, wait_event**locks_event_wait_list) {
   	//scorre la lista degli eventi in attesa fino a quando non trova un evento in attesa su dataitem di_id
	if((*locks_event_wait_list)==NULL){
		*event_content=NULL;
		return 0;
	} else {
		if((*locks_event_wait_list)->event_content->di_id==di_id && (*locks_event_wait_list)->event_content->op_type == TM_READ ){
			wait_event *event_item=*locks_event_wait_list;
			*locks_event_wait_list=(*locks_event_wait_list)->next;
			// memorizza i dati dell'evento per restituirli
			*event_type=event_item->event_type;
			*event_content=event_item->event_content;
			// elimina l'evento
			free(event_item);
			return 1;
		} else {
			wait_event *event_temp=*locks_event_wait_list;
      		while(event_temp->next) {
				if(event_temp->next->event_content->di_id==di_id  && (*locks_event_wait_list)->event_content->op_type == TM_READ ){
					wait_event *event_item=event_temp->next;
					event_temp->next=event_temp->next->next;
					// memorizza i dati dell'evento per restituirli
					*event_type=event_item->event_type;
					*event_content=event_item->event_content;
					// elimina l'evento
					free(event_item);
					return 1;
				}
				event_temp=event_temp->next;
			}
			*event_type=0;
			*event_content=NULL;
			return 0;
		}
	}
}

// se in lista è presente un evento della transazione tx_id restituisce l'id del dataitem che attende, 0 altrimenti
int get_waiting_data_item(int tx_id, int table_id, int di_id, wait_event**locks_event_wait_list){
	wait_event *event_temp=*locks_event_wait_list;
	while(event_temp) {
		if (event_temp->event_content->tx_id==tx_id) {
			di_id=event_temp->event_content->di_id;
			return 1;
		}
		event_temp=event_temp->next;
	}
	return 0;
}


// restituisce al lunghezza della lista
int get_list_length(wait_event**locks_event_wait_list) {
	int i=0;
	if((*locks_event_wait_list)) {
		wait_event *ev=*locks_event_wait_list;
		while(ev) {
			ev=ev->next;
			 i++;
		}
	}
	return i;
}





