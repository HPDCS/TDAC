#include "stdafx.h"

#include "generale.h"
#include "events_wait_queue.h"
#include "distribution_generator.h" 

double get_GVC_service_time(int event_type, event_content_type *event_content,GVC_lp_state_type * pointer) {
	// genera la distribuzione esponenziale
	return (double)TIME_FOR_GVC_INCREMENT_EXECUTION;
}



void serve_GVC_request(int me,double now, event_content_type *event_content, GVC_lp_state_type * pointer) {
	// occupa un servente
    int destination;
    double service_time;
	pointer->avaiable_servants=pointer->avaiable_servants-1;
    destination=event_content->origin_object;
    service_time=get_GVC_service_time(event_content->origin_event_type,event_content,pointer);
	// schedula un evento di risposta ed un evento per se;
	InsertEvent(destination, me, now +service_time, now, GVC_RESPONSE, event_content);
	InsertEvent(me, me, now +service_time, now, GVC_REQUEST_SERVED, event_content);	
}

void GVC_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, GVC_lp_state_type * pointer)
{

	
	switch (event_type)	{

		case INIT:
			if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: ricevuto evento INIT\n");
			//oper di inizializzazione
			pointer= (GVC_lp_state_type*)malloc(sizeof(GVC_lp_state_type));
			if (MALLOC_FAIL_VERBOSE && pointer==NULL) printf("-------- Funzione GVC_ProcessEvent: MALLOC FALLITA\n");
			SetPointer(me,pointer);
			pointer->GVC_op_seed=1;
			pointer->served_requests=0;
			pointer->queue_length=0;
            //inizializza la propria coda di attesa
            pointer->requests_queue.top=NULL;
			pointer->requests_queue.bottom=NULL;
			pointer->avaiable_servants=1;

			break;
		
		case GVC_REQUEST:
			
			if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: ricevuto evento GVC_REQUEST al tempo %f\n", now);
			if (GVC_VERBOSE) printf("-------- Funzione GVC serve_reques: serventi disponibili: %i\n", pointer->avaiable_servants);
			if (pointer->avaiable_servants>0){
				//ci sono serventi liberi quindi serve la richiesta
				serve_GVC_request(me, now, event_content, pointer);

			} else 	{
				//non c'è un servente libero quindi accoda la richiesta

		        copy_into_wait_queue(event_type,event_content,now,&pointer->requests_queue);
				int req =get_queue_length(&pointer->requests_queue);
				if (req>pointer->queue_length) {
					//printf("Numero massimo di richieste per GVC in coda: %i\n",req);
					pointer->queue_length=req;
				}
				if (GVC_QUEUE_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: richiesta in coda: %i\n",get_queue_length(&pointer->requests_queue));
				if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: richiesta accodata\n");

			}

			break;

		case GVC_REQUEST_SERVED:
			if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: ricevuto evento REQUEST_SERVED al tempo %f\n", now);
			pointer->served_requests=pointer->served_requests+1;
			if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: richieste servite: %i\n", pointer->served_requests);
			// libera un servente
			pointer->avaiable_servants=pointer->avaiable_servants+1;
			//controlla se ce una richiesta in coda e la serve
			if (!is_empty(&pointer->requests_queue)){
				int new_event_type;
				event_content_type*new_event_content_pointer;
				// prende il primo evento in cosa di attesa
        	    get_from_wait_queue(&new_event_type,&new_event_content_pointer,&pointer->requests_queue);				
				// serve la richiesta
				serve_GVC_request(me, now, new_event_content_pointer, pointer);
				free(new_event_content_pointer);
			} else {
				if (GVC_VERBOSE) printf("-------- Funzione GVC_ProcessEvent: non ci sono richieste in coda\n");
			}

			break;
	}
}
