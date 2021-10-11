#include "stdafx.h"

#include <stdlib.h>
#include "generale.h"
#include "events_wait_queue.h"
#include "distribution_generator.h"

#define SEED 3458767;

void stat_add_tx_run(event_content_type* event_content, double now) {
		event_content_type new_event_content;
		new_event_content.tx_id=event_content->tx_id;
		new_event_content.tx_class_id=event_content->tx_class_id;
		new_event_content.tx_run_number=event_content->tx_run_number;
		new_event_content.time=now;
		if (event_content->tx_run_number==0)
			InsertEvent(STAT_COLLECTOR, CLIENT, now, now, STAT_ADD_tx, &new_event_content);
		InsertEvent(STAT_COLLECTOR, TM, now, now, STAT_ADD_tx_run, &new_event_content);
}

void stat_add_op(event_content_type* event_content, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_class_id=event_content->tx_class_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=event_content->op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CLIENT, now, now, STAT_ADD_op, &new_event_content);
}

void stat_set_run_commit(event_content_type* event_content, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_class_id=event_content->tx_class_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=event_content->op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, TM, now, now, STAT_SET_run_commit, &new_event_content);
}

void stat_set_run_abort(event_content_type* event_content, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_class_id=event_content->tx_class_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=event_content->op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, TM, now, now, STAT_SET_run_abort, &new_event_content);
}

void TM_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, TM_lp_state_type * pointer) {
	switch (event_type)	{

		case INIT:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento INIT\n");
            pointer = (TM_lp_state_type*)malloc(sizeof(TM_lp_state_type));
			if (MALLOC_FAIL_VERBOSE && pointer==NULL) printf("---- Funzione TM_ProcessEvent: MALLOC FALLITA\n");
			SetPointer(me,pointer);
			//inizializza la coda di attesa per le richieste di BEGIN
            pointer->begin_request_wait_queue.top=NULL;
			pointer->begin_request_wait_queue.bottom=NULL;
			//inizializza il puntatore alla lista di transazioni attive
			pointer->active_transactions_list=NULL;
			pointer->actives_transactions=0;
			// valori per le distribuzioni
			pointer->data_item_access_distribution=DATA_ITEMS_ACCESS_DISTRIBUTION;
			pointer->data_items_zipf_const=DATA_ITEMS_ZIPF_CONST;

			break;


		case TM_BEGIN:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_BEGIN al tempo %f\n", now);
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_add_tx_run(event_content,now);
			/**************************************/
			// lo invia al CC
			InsertEvent(CC, me, now, now, TM_BEGIN, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_BEGIN al CC\n");

            break;

		case TM_READ:
			event_content->op_type=TM_READ;
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_READ al tempo %f\n", now);
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_add_op(event_content, now);
			/**************************************/
			// lo invia al CC
			InsertEvent(CC, me, now, now, TM_READ, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_READ al CC\n");
			break;

		case TM_WRITE:
			event_content->op_type=TM_WRITE;
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_WRITE al tempo %f\n", now);
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_add_op(event_content, now);
			/**************************************/
			// lo invia al CC
			InsertEvent(CC, me, now, now, TM_WRITE, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_WRITE al CC\n");
			break;

		case TM_COMMIT:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_COMMIT al tempo %f\n", now);
			// lo invia al CC
			InsertEvent(CC, me, now, now, TM_COMMIT, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_COMMIT al CC\n");
			break;

		case TM_BEGIN_EXECUTED:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_BEGIN_EXECUTED al tempo %f\n", now);
			// risponde al client
			InsertEvent(CLIENT, me, now, now, TM_BEGIN_EXECUTED, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_BEGIN_EXECUTED al CLIENT\n");
			break;

		case TM_OPERATION_EXECUTED:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_OPERATION_EXECUTED al tempo %f\n", now);
			// risponde al client
			InsertEvent(CLIENT, me, now, now, TM_OPERATION_EXECUTED, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_OPERATION_EXECUTED al CLIENT\n");
			break;

		case TM_COMMIT_EXECUTED:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_COMMIT_EXECUTED al tempo %f\n", now);
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_set_run_commit(event_content, now);
			/**************************************/
			// lo invia al CLIENT
			InsertEvent(CLIENT, me, now, now, TM_COMMIT_EXECUTED, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_COMMIT_EXECUTED al CLIENT\n");
			break;

		case TM_ABORT_EXECUTED:
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: ricevuto evento TM_ABORT_EXECUTED al tempo %f\n", now);
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_set_run_abort(event_content, now);
			/**************************************/
			// lo invia al CLIENT
			InsertEvent(CLIENT, me, now+simbolicDelay, now, TM_ABORT_EXECUTED, event_content);
			if (TM_VERBOSE) printf("---- Funzione TM_ProcessEvent: inviato evento TM_ABORT_EXECUTED al CLIENT\n");
			break;
	}
}

